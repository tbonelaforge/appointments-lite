#include "Spider.h"
#include "Appointment.h"
#include <iostream>

using namespace std;

const char * INSERT_APPOINTMENT_TEMPLATE = "insert into appointment (doctor_id, room_id, procedure_id, patient_id, start, end, week_number) values (%d, %d, %d, %d, '%s', '%s', %d)";

char INSERT_APPOINTMENT_QUERY[1000];


const char * DELETE_AVAILABILITY_TEMPLATE = "delete from availability where resource_type = '%s' and resource_id = %d";

char DELETE_AVAILABILITY_QUERY[1000];

const char * INSERT_AVAILABILITY_TEMPLATE = "insert into availability (resource_type, resource_id, start, end) values ('%s', %d, '%s', '%s')";

char INSERT_AVAILABILITY_QUERY[1000];


const char * SELECT_AVAILABILITY_TEMPLATE = "select resource_type, resource_id, start, end from availability where resource_type = '%s' and resource_id = %d order by datetime(start)";

char SELECT_AVAILABILITY_QUERY[1000];


const char * GET_APPOINTMENTS_BY_RESOURCE_TEMPLATE = "select procedure_id, start, end, week_number from appointment where %s = %d order by datetime(start)";

char GET_APPOINTMENTS_BY_RESOURCE_QUERY[1000];


void prepareInsertAppointmentQuery(int doctorId, int roomId, int procedureId, int patientId, string start, string end, int week) {
        INSERT_APPOINTMENT_QUERY[0] = '\n';
        sprintf(
                INSERT_APPOINTMENT_QUERY,
                INSERT_APPOINTMENT_TEMPLATE,
                doctorId,
                roomId,
                procedureId,
                patientId,
                start.c_str(),
                end.c_str(),
                week
        );
}

int insertCallback(void * NotUsed, int argc, char ** argv, char ** colNames) {
    return 0;
}

void prepareDeleteAvailabilityQuery(string resource_type, int resource_id) {
    DELETE_AVAILABILITY_QUERY[0] = '\n';
    sprintf(
            DELETE_AVAILABILITY_QUERY,
            DELETE_AVAILABILITY_TEMPLATE,
            resource_type.c_str(),
            resource_id
    );
}


int deleteAvailabilityCallback(void * NotUsed, int argc, char ** argv, char ** colNames) {
    return 0;
}



void prepareInsertAvailabilityQuery(string resource_type, int resource_id, string start, string end) {
    INSERT_AVAILABILITY_QUERY[0] = '\n';
    sprintf(
            INSERT_AVAILABILITY_QUERY,
            INSERT_AVAILABILITY_TEMPLATE,
            resource_type.c_str(),
            resource_id,
            start.c_str(),
            end.c_str()
    );
}

int insertAvailabilityCallback(void * NotUsed, int argc, char ** argv, char ** colNames) {
    return 0;
}


void prepareSelectAvailabilityQuery(string resource_type, int resource_id) {
    SELECT_AVAILABILITY_QUERY[0] = '\n';
    sprintf(
            SELECT_AVAILABILITY_QUERY,
            SELECT_AVAILABILITY_TEMPLATE,
            resource_type.c_str(),
            resource_id
    );
}


int selectAvailabilityCallback(void * resource, int argc, char ** argv, char ** colNames) {
    /* Expecting:
    resource_type  resource_id  start             end
    -------------  -----------  ----------------  ----------------
    Dr             1            2019-01-01 08:00  2019-01-01 17:00

    */
    Resource * r = reinterpret_cast<Resource *>(resource);
    string resource_type = argv[0];
    int resource_id = atoi(argv[1]);
    string availabilityStart = argv[2];
    string availabilityEnd = argv[3];
    Date startDate, endDate;
    Time startTime, endTime;
    Appointment::parseDatetime(availabilityStart, startDate, startTime);
    Appointment::parseDatetime(availabilityEnd, endDate, endTime);
    Time duration = endTime - startTime;
    Appointment availabilityAppointment("Available", startTime, duration, startDate);
    ((Resource *) resource)->addAppt(availabilityAppointment);
    return 0;
}


void prepareGetAppointmentsByResourceQuery(string resource_type, int resource_id) {
    GET_APPOINTMENTS_BY_RESOURCE_QUERY[0] = '\n';
    sprintf(
            GET_APPOINTMENTS_BY_RESOURCE_QUERY,
            GET_APPOINTMENTS_BY_RESOURCE_TEMPLATE,
            (resource_type == "Dr") ? "doctor_id" : (resource_type == "Room") ? "room_id" : "patient_id",
            resource_id
    );
}


int getAppointmentByResourceCallback(void * r, int argc, char ** argv, char ** colNames) {
    /* Expecting:
    procedure_id  start             end               week_number
    ------------  ----------------  ----------------  -----------
    1             2019-01-04 08:00  2019-01-04 08:45  0
    */
    Resource * resource = reinterpret_cast<Resource *>(r);
    int procedure_id = atoi(argv[0]);
    string appointmentStart = argv[1];
    string appointmentEnd = argv[2];
    int weekNum = atoi(argv[3]);
    Date startDate, endDate;
    Time startTime, endTime;
    string appointmentType;
    Time expectedDuration;
    Appointment::parseRequirement(Requirement(procedure_id), expectedDuration, appointmentType);
    Appointment::parseDatetime(appointmentStart, startDate, startTime);
    Appointment::parseDatetime(appointmentEnd, endDate, endTime);
    Time duration = endTime - startTime;
    if (duration != expectedDuration) {
        throw "Invalid duration for appointment";
    }
    Appointment committedAppointment(appointmentType, startTime, duration, startDate);
    resource->addAppt(committedAppointment, 0);
    if (resource->getType() == "Dr") {
        resource->addAdminAppointment(committedAppointment);
    }
    return 0;
}

void Spider::removePat(const string nm) {
    int loc = 0;
    bool found = false;
    for (int i = 0; i < pats.size(); ++i ) {
        if (pats.at(i)->getName() == nm) {
            loc = i;
            found = true;
            break;
        }
    }
    if (found) pats.erase(pats.begin() + loc);
}

//find a matchup a time interval of a room resource for a given doctor
bool Spider::findMatchTime(Resource* rm, Opens &opens) {
    int index = 0;
    if (!opens.isGood[0]) index = 0;
    else if (!opens.isGood[1]) index = 1;
    else if (!opens.isGood[2]) index = 2;
    else throw "Bad Opens Record!";
    
    Date d = opens.appt [index] .getDay();
    int weeknum = d.getWeekNum();
    Time dur = opens.appt [index] .getDuration();
    Time start = opens.appt [index] .getStart();
    Time early, earlyDur, late;  //early / late start of avail time; duration of early window
    ApptNode* current;
    bool found = false;
    
    if (rm->maxAvail[weeknum] >= dur) {  //if even enough time in a week
        current = rm->oblig[weeknum][1];
        
        for (int i = 0; i < rm->nodeInv[weeknum][1]; ++i) {  //find day / time / node
            if (current->appt.getDay() == d) {  //day
                if (current->appt.getDuration() >= dur) {  //found enough time
                    if (start < current->appt.getStart()) {  //which window is first
                        early = start;
                        earlyDur = opens.convertAvail [1][index] .getDuration(); // 1 = rooms
                        late = current->appt.getStart();
                    } else {
                        early = current->appt.getStart();
                        earlyDur = current->appt.getDuration();
                        late = start;
                    }
                    if (late + dur < early + earlyDur) {  //margins in sync!!
                        opens.isGood[index] = true;
                        opens.convertAvail[1][index] = current->appt;
                        opens.appt [index] .setStart(late);
                        opens.strands[index] = rm;
                        found = true;
                        ++index;
                        if (index > 2) break;  //if >2 then done finding openings
                        
                        //find more slots if time permits
                        Time startNew = late + dur;
                        Time durNew = earlyDur - (late - early) - dur;
                        if (durNew > dur) { //doctor requires more time
                            if (index == 1) {
                                opens.isGood[1] = true;
                                opens.convertAvail[1][1] = current->appt;
                                opens.convertAvail[0][1] = opens.convertAvail[0][0];  //fill doc appt slot from prior
                                opens.appt [1] .setStart( startNew );
                                opens.appt [1] .setDay(d);
                                opens.strands[1] = rm;
                                ++index;
                                startNew = startNew + dur;
                                durNew = durNew - dur;
                                if ( !(durNew > dur) ) continue;  //no more time
                            }
                            if (index == 2) {
                                opens.isGood[2] = true;
                                opens.convertAvail[1][2] = current->appt;
                                opens.convertAvail[0][2] = opens.convertAvail[0][1];  //fill doc appt slot from prior
                                opens.appt [2] .setStart( startNew );
                                opens.appt [2] .setDay(d);
                                opens.strands[2] = rm;
                                break;  //done
                            }
                        }
                    }
                }
            }
            current = current->next;
        }
    }
    return found;
}

Opens Spider::findAppts(Resource* doc, Patient* pat, Requirement req, int browse) {
    Opens opens;
    Time dur;
    string type;
    Date today = Date::getCurrentDate();
    Appointment::parseRequirement(req, dur, type);
    opens.appt[0] = Appointment(type, 1, dur, req);
    opens.appt[1] = Appointment(type, 1, dur, req);
    opens.appt[2] = Appointment(type, 1, dur, req);
    
    //finds 3 openings, minus browse to browse ahead for next openings
    int count = 0 - browse;  //number of found opens
    ApptNode* current = nullptr;

    int i = today.getWeekNum();  //start on today's week number
    for (; i < 53; ++i) {  //first find week
        if (doc->maxAvail[i] > dur) {  //make sure enough time somewhere in week
            current = doc->oblig[i][1];
            for (int j = 0; j < doc->nodeInv[i][1]; ++j) {
                Date currNodeDay = current->appt.getDay();
                if (today > currNodeDay || today == currNodeDay) continue;  //find opening after today

                //keep patients from having more than one appt in a day
                if (pat->nodeInv[i][0] > 0) {
                    ApptNode* patCur = pat->oblig[i][0];
                    bool skip = false;
                    
                    for (int k = 0; k < pat->nodeInv[i][0]; ++k) {
                        if (patCur->appt.getDay() != current->appt.getDay()) {
                            patCur = patCur->next;
                        }
                        else {
                            skip = true;
                            break;
                        }
                    }
                    if (skip) {
                        current = current->next;
                        continue;
                    }
                }
                
                if (current->appt.getDuration() > dur) {  //find slot with more time, always add admin for paperwork with Dr's
                   if (count > -1) {  //if browsing push to different time
                       
                        for (int k = 0; k < resrcs.size(); ++k) {
                            if (resrcs.at(k)->getType() == "Room" && resrcs.at(k) -> matchTag(req)) {
                                opens.appt [count] .setDay( current->appt.getDay() );
                                opens.appt [count] .setStart( current->appt.getStart() );
                                opens.convertAvail [0][count] = current->appt;
                                opens.weekNums[count] = i;
                                if (findMatchTime(resrcs.at(k), opens)) {
                                    if (opens.isGood[2]) return opens;
                                    else if (opens.isGood[1]) count = 2;
                                    else if (opens.isGood[0]) count = 1;
                                }
                            }
                        }
                   }
                   else ++count;
                }
                current = current->next;
                if (count > 2) return opens;
            }
            if (count > 2) return opens;
        }
    }
    return opens;
}
void Spider::convertToCommit(Resource* doc, Patient* pat, Opens &opens, int slot) {
    Appointment appt = opens.appt[slot];
    int weekNum = opens.weekNums[slot];
    Appointment docAvail = opens.convertAvail[0][slot];
    Appointment rmAvail = opens.convertAvail[1][slot];
    Resource* room = opens.strands[slot];
    appt.setRList(room->getName() + " " + pat->getName() + " " + doc->getName());  //each appointment should list its components
    pat->addAppt(appt, 0);  //commit patient to appt
    doc->convertAvailabilityToAppointment(docAvail, appt);
    room->convertAvailabilityToAppointment(rmAvail, appt);
    saveAppointment(
            doc->getId(),
            room->getId(),
            appt.getReq(0),
            pat->getId(),
            appt.formatStartDatetime(),
            appt.formatEndDatetime(),
            weekNum
    );
    updateAvailability(doc);
    updateAvailability(room);




}
void Spider::printPats() {
    for (int i = 0; i < pats.size(); ++i) {
        cout << " < " << i << " > " << pats.at(i) -> getName() << endl;
    }
    cout << endl;
}
int Spider::printDocs() {
    int count = 0;
    for (int i = 0; i < resrcs.size(); ++i) {
        if (resrcs.at(i)->getType() == "Dr")
            cout << " < " << count++ << " > " << resrcs.at(i) -> getName() << endl;
    }
    cout << endl;
    return count;
}
void Spider::printAvails(Opens opens) {
    for (int i = 0; i < 3; ++i) {
        if (opens.isGood[i]) {
            cout << " < " << i << " > ";
            opens.appt [i] .getDay() .outDate(cout);
            cout << " @ ";
            opens.appt [i] .getStart() .timeOut(cout);
            cout << endl;
        }
    }
    cout << endl;
}

void Spider::printProced(Resource* doc) {
    Requirement rq;
    int count = 0;
    for (int i = 0; i < doc->MAX_QUALT; ++i) {
        rq = doc->getQualTag(i);
        if (rq != NIL) {
            switch (rq) {
                case EXAM: cout << " < " << count++ << " > " << "Exam" << endl; break;
                case THEREPY: cout << " < " << count++ << " > " << "Physiotherepy" << endl; break;
                case XRAY: cout << " < " << count++ << " > " << "X-Ray" << endl; break;
                case BLOOD: cout << " < " << count++ << " > " << "Bloodwork" << endl; break;
                default:;
            }
        }
    }
    cout << endl;
}



void Spider::saveAppointment(int doctorId, int roomId, int procedureId, int patientId, string start, string end, int week) {
    int rc;
    char * zErrMsg = NULL;
    prepareInsertAppointmentQuery(doctorId, roomId, procedureId, patientId, start, end, week);
    rc = sqlite3_exec(db, INSERT_APPOINTMENT_QUERY, insertCallback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR INSERTING APPOINTMENT" << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't save appointment");
    }
}

void Spider::updateAvailability(Resource *resource) {
    int rc;
    char * zErrMsg = NULL;
    prepareDeleteAvailabilityQuery(resource->getType(), resource->getId());
    rc = sqlite3_exec(db, DELETE_AVAILABILITY_QUERY, deleteAvailabilityCallback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR DELETING AVAILABILITIES: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't update availability");
    }

    for (int i = 0; i < 53; i++) {
        ApptNode * current = resource->oblig[i][1];
        for (int j = 0; j < resource->nodeInv[i][1]; ++j) {
            Appointment availabilityAppointment = current->appt;
            prepareInsertAvailabilityQuery(
                    resource->getType(),
                    resource->getId(),
                    availabilityAppointment.formatStartDatetime(),
                    availabilityAppointment.formatEndDatetime()
            );
            rc = sqlite3_exec(db, INSERT_AVAILABILITY_QUERY, insertAvailabilityCallback, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                cout << "SQL ERROR INSERTING AVAILABILITY: " << sqlite3_errmsg(db) << endl;
                sqlite3_free(zErrMsg);
                throw string("Can't update availability");
            }
            current = current->next;
        }

    }
}

void Spider::saveAllAvailability() {
    for (int i = 0; i < resrcs.size(); i++) {
        updateAvailability(resrcs[i]);
    }
}

void Spider::loadAvailability(Resource *resource) {
    string resourceType = resource->getType();
    int resource_id = resource->getId();
    char * error_message = NULL;
    int rc;
    prepareSelectAvailabilityQuery(resourceType, resource_id);
    rc = sqlite3_exec(db, SELECT_AVAILABILITY_QUERY, selectAvailabilityCallback, resource, &error_message);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR SELECTING AVAILABILITY" << sqlite3_errmsg(db) << endl;
        sqlite3_free(error_message);
        throw string("Can't load availability");
    }
}

void Spider::loadAppointments(Resource *resource) {
    string resourceType = resource->getType();
    int resource_id = resource->getId();
    char * error_message = NULL;
    int rc;
    prepareGetAppointmentsByResourceQuery(resourceType, resource_id);
    rc = sqlite3_exec(
            db,
            GET_APPOINTMENTS_BY_RESOURCE_QUERY,
            getAppointmentByResourceCallback,
            resource,
            &error_message
    );
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR SELECTING APPOINTMENTS BY RESOURCE" << sqlite3_errmsg(db) << endl;
        sqlite3_free(error_message);
        throw string("Can't load appointments");
    }
}
