#include "Spider.h"
#include <iostream>

using namespace std;

const char * INSERT_APPOINTMENT_TEMPLATE = "insert into appointment (doctor_id, room_id, procedure_id, start, end, week_number) values (%d, %d, %d, '%s', '%s', %d)";

char INSERT_APPOINTMENT_QUERY[1000];

void prepareInsertAppointmentQuery(int doctorId, int roomId, int procedureId, string start, string end, int week) {
        INSERT_APPOINTMENT_QUERY[0] = '\n';
        sprintf(
                INSERT_APPOINTMENT_QUERY,
                INSERT_APPOINTMENT_TEMPLATE,
                doctorId,
                roomId,
                procedureId,
                start.c_str(),
                end.c_str(),
                week
        );
}

int insertCallback(void * NotUsed, int argc, char ** argv, char ** colNames) {
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

void printAppointmentList(ApptNode * current) {
    int count = 1;
    while (current != nullptr) {
        Appointment appt = current->appt;
        appt.prettyPrint(cout, count);
        cout << endl;
        count += 1;
        current = current->next;
    }
}

//find a matchup a time interval of a room resource for a given doctor
bool Spider::findMatchTime(Resource* rm, Opens &opens) {
    cout << "Inside findMatchTime, got called..." << endl;
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
                        cout << "Inside findMatchTime, I think late + dur < early + earlyDur..." << endl;
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
    switch (req) {
            
        case EXAM:
            dur.setMn(45);
            type = "Exam";
            break;
            
        case BLOOD:
            dur.setMn(15);
            type = "Lab";
            break;
            
        case THEREPY:
            dur.setHr(1);
            type = "Therepy";
            break;
            
        case XRAY:
            dur.setMn(20);
            type = "X-Ray";
            break;
            
        case NIL: throw "Invalid Requirement for Appointment";
    }
    opens.appt[0] = Appointment(type, 1, dur, req);
    opens.appt[1] = Appointment(type, 1, dur, req);
    opens.appt[2] = Appointment(type, 1, dur, req);
    
    //finds 3 openings, minus browse to browse ahead for next openings
    int count = 0 - browse;  //number of found opens
    ApptNode* current = nullptr;

    for (int i = 0; i < 53; ++i) {  //first find week
        if (doc->maxAvail[i] > dur) {  //make sure enough time somewhere in week
            current = doc->oblig[i][1];
            for (int j = 0; j < doc->nodeInv[i][1]; ++j) {

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
    Appointment newAvail = docAvail;  //same day, may be same start time
    Time afterAdmin = appt.getStart() + appt.getDuration() + Time(0,5);  //time start after admain
    Appointment admin = Appointment("Admin", appt.getStart() + appt.getDuration(), Time(0,5), docAvail.getDay());  //paperwork for doc
    Resource* room = opens.strands[slot];
    appt.setRList(room->getName() + " " + pat->getName() + " " + doc->getName());  //each appointment should list its components
    pat->addAppt(appt, 0);  //commit patient to appt
    
    //make up to two new available appt refunded from the open slot used
    Time RefundPrior = appt.getStart() - docAvail.getStart();
    Time RefundAfter = docAvail.getDuration() - RefundPrior - appt.getDuration() - Time(0,5);
    doc->removeAppt(docAvail.getStart(), docAvail.getDay());  //remove open slot
    if (RefundPrior > Time(0)) {
        newAvail.setDuration(RefundPrior);
        doc->addAppt(newAvail);
    }
    doc->addAppt(appt, 0);  //commit doc to procedure and patient appt
    doc->addAppt(admin, 0);  //commit to paperwork time
    if (RefundAfter > Time(0)) {
        newAvail.setStart(afterAdmin);
        newAvail.setDuration(RefundAfter);
        doc->addAppt(newAvail);
    }
    RefundPrior = appt.getStart() - rmAvail.getStart();
    RefundAfter = rmAvail.getDuration() - RefundPrior - appt.getDuration();
    room->removeAppt(rmAvail.getStart(), rmAvail.getDay());
    if (RefundPrior > Time(0)) {
        newAvail.setDuration(RefundPrior);
        room->addAppt(newAvail);
    }
    room->addAppt(appt, 0);  //commit room to procedure and patient appt
    if (RefundAfter > Time(0)) {
        newAvail.setStart(appt.getStart() + appt.getDuration());
        newAvail.setDuration(RefundAfter);
        room->addAppt(newAvail);
    }
    saveAppointment(
            doc->getId(),
            room->getId(),
            appt.getReq(0),
            appt.formatStartDatetime(),
            appt.formatEndDatetime(),
            weekNum
    );



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



void Spider::saveAppointment(int doctorId, int roomId, int procedureId, string start, string end, int week) {
    int rc;
    char * zErrMsg = NULL;
    prepareInsertAppointmentQuery(doctorId, roomId, procedureId, start, end, week);
    rc = sqlite3_exec(db, INSERT_APPOINTMENT_QUERY, insertCallback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR INSERTING APPOINTMENT" << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't save appointment");
    }
}
