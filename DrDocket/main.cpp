#include <fstream>
#include <iostream>
#include <string>

#include "Interface.h"
#include "Resource.h"
#include "Spider.h"
#include "Patient.h"

#include "sqlite3.h"

using std::cout;
using std::endl;

const char * GET_DOCTORS_COUNT = "select count(*) from doctor";

const char * GET_DOCTORS_WITH_PROCEDURES = "select d.id, d.last_name, p.id, p.name from doctor d left join doctor_procedure dp on dp.doctor_id = d.id left join procedure p on p.id = dp.procedure_id";

const char * GET_ROOM_COUNT = "select count(*) from room";

const char * GET_ROOMS_WITH_EQUIPMENT = "select r.id as room_id, r.number as room_number, e.id as equipment_id, e.name as equipment_name from room r left join room_equipment re on re.room_id = r.id left join equipment e on e.id = re.equipment_id";

const char * GET_PATIENTS_COUNT = "select count(*) from patient";

const char * GET_PATIENTS = "select p.id, p.first_name, p.last_name from patient p";

// Loading state variables for loading resources
int resource_offset = 0;

// Loading state variables for loading doctors
int numDoctors = 0;
int currentDoctorIndex = -1;
int currentQualTagIndex = -1;
string * docs = NULL;
int * doc_ids = NULL;

// Loading state variables for loading rooms
int numRooms = 0;
int currentRoomIndex = -1;
int currentRoomTagIndex = -1;
string * roomNumbers = NULL;
int * room_ids = NULL;
Availability nonLaborAvail;

// Loading state variables for loading patients
int numPatients = 0;
int currentPatientIndex = -1;
string * patientNames = NULL;
int * patient_ids = NULL;

// Each loading callback needs to add to spdr
Spider spdr;


static int doctor_count_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {
    numDoctors = atoi(argv[0]);
    return 0;
}

static int doctor_procedure_row_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {

    // Expecting rows having: d.id, d.last_name, p.id, p.name
    // e.g: 1|Alex|Brown|1|Checkup

    // Expecting spdr to have a valid db.

    int doctor_id = atoi(argv[0]);
    string doctor_last_name = argv[1];
    int procedure_id = atoi(argv[2]);
    if (currentDoctorIndex < 0 || doc_ids[currentDoctorIndex] != doctor_id) {
        currentDoctorIndex += 1;
        docs[currentDoctorIndex] = doctor_last_name;
        doc_ids[currentDoctorIndex] = doctor_id;
        Resource* d = new Resource("Dr", docs[currentDoctorIndex]);
        d->setId(doctor_id);
        spdr.loadAvailability(d);
        spdr.loadAppointments(d);
        spdr.addResrc(d);
        currentQualTagIndex = -1;
    }
    currentQualTagIndex += 1;
    spdr.setResrc(currentDoctorIndex)->setQualTag(
            currentQualTagIndex,
            Requirement (procedure_id)
    );
    return 0;
}


static int room_count_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {
    numRooms = atoi(argv[0]);
    return 0;
}

static int room_equipment_row_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {

    // Expecting rows having: r.id, r.number, e.id, e.name
    // e.g: 1|3A|1|Exam Table

    // Expecting spdr to have a valid db.

    int room_id = atoi(argv[0]);
    string room_number = argv[1];
    int equipment_id = atoi(argv[2]);
    if (currentRoomIndex < 0 || room_ids[currentRoomIndex] != room_id) {
        currentRoomIndex += 1;
        roomNumbers[currentRoomIndex] = room_number;
        room_ids[currentRoomIndex] = room_id;
        Resource* r = new Resource("Room", roomNumbers[currentRoomIndex], nonLaborAvail);
        r->setId(room_id);
        spdr.loadAvailability(r);
        spdr.loadAppointments(r);
        spdr.addResrc(r);
        currentRoomTagIndex = -1;
    }
    currentRoomTagIndex += 1;
    spdr.setResrc(resource_offset + currentRoomIndex)->setQualTag(
            currentRoomTagIndex,
            Requirement (equipment_id)
    );
    return 0;
}


static int patient_count_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {
    numPatients = atoi(argv[0]);
    return 0;
}

static int patient_row_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {

    // Expecting rows having: p.id, p.first_name, p.last_name
    // e.g: 1|Terry|Ford

    // Expecting spdr to have a valid db.

    int patient_id = atoi(argv[0]);
    string first_name = argv[1];
    string last_name = argv[2];
    currentPatientIndex += 1;
    patientNames[currentPatientIndex] = first_name + " " + last_name;
    patient_ids[currentPatientIndex] = patient_id;
    Patient* p = new Patient(patientNames[currentPatientIndex]);
    p->setId(patient_id);
    spdr.addPat(p);
    spdr.loadAppointments(p);
    return 0;
}


void open_database(sqlite3 ** db) {
    int rc = sqlite3_open("appointments.db", db);
    if (rc) {
        cout << "Error opening database: " << sqlite3_errmsg(*db) << endl;
        throw string("Can't open database");
    } else {
        cout << "Opened database successfully" << endl;
    }
}


void load_doctors() {
    int rc;
    char * errorMessage = NULL;
    rc = sqlite3_exec(spdr.getDb(), GET_DOCTORS_COUNT, doctor_count_callback, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING DOCTORS: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load doctors");
    }
    docs = new string[numDoctors];
    doc_ids = new int[numDoctors];
    rc = sqlite3_exec(
            spdr.getDb(),
            GET_DOCTORS_WITH_PROCEDURES,
            doctor_procedure_row_callback,
            0,
            &errorMessage
    );
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING DOCTORS WITH PROCEDURES: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load doctors");
    }
    resource_offset = spdr.getNumResrcs();
}


void load_rooms() {
    int rc;
    char * errorMessage = NULL;
    rc = sqlite3_exec(spdr.getDb(), GET_ROOM_COUNT, room_count_callback, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING ROOMS: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load rooms");
    }
    roomNumbers = new string[numRooms];
    room_ids = new int[numRooms];
    nonLaborAvail.setAvail(true, 1, 22);
    nonLaborAvail.setAvail(true, 0, 7, 'D');
    nonLaborAvail.setAvail(true, 0, 12, 'M');
    rc = sqlite3_exec(
            spdr.getDb(),
            GET_ROOMS_WITH_EQUIPMENT,
            room_equipment_row_callback,
            0,
            &errorMessage
    );
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING ROOMS WITH EQUIPMENT: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load rooms");
    }
}


void load_patients() {
    int rc;
    char * errorMessage = NULL;
    rc = sqlite3_exec(spdr.getDb(), GET_PATIENTS_COUNT, patient_count_callback, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING PATIENTS: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load patients");
    }
    patientNames = new string[numPatients];
    patient_ids = new int[numPatients];
    rc = sqlite3_exec(
            spdr.getDb(),
            GET_PATIENTS,
            patient_row_callback,
            0,
            &errorMessage
    );
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING PATIENTS: " << sqlite3_errmsg(spdr.getDb()) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load patients");
    }
}


int main(int argc, char *argv[]) {
    cout << "Welcome to DrDocket!" << endl;
    sqlite3 * db;
    try {
        open_database(&db);
        spdr.setDb(db);
        load_doctors();
        load_rooms();
        load_patients();
    
        Interface handler;
        handler.command = MODE;    //command for operating the interface. starts at 'M' for Mode select menu
    
        //menu gets input from user, then loops to fetch access from spider or tell spider to perfom something
        //think of main as back end, and interface.cpp as front end;
        while (handler.command != QUIT) {
            handler.numPats = spdr.getNumPats();
            handler.Menu(handler.command);
        
            //fetches access
            switch (handler.fetch) {
        
            case DOC:
                for (int i = 0; i < spdr.getNumResrcs(); ++i) {
                    if (spdr.setResrc(i)->getType() == "Dr")
                        if ( i == handler.numD) handler.resrc = spdr.setResrc(i);
                }
                break;
                
            case PAT:
                for (int i = 0; i < spdr.getNumPats(); ++i) {
                    if (spdr.getPat(i)->getType() == "Patient")
                        if ( i == handler.numP) handler.pat = spdr.getPat(i);
                }
                break;
                
            case AVAIL:
                //fetches requirement
                for (int i = 0; i < handler.resrc->MAX_QUALT; ++i) {
                    if (spdr.setResrc(handler.numD)->getQualTag(i) != NIL)
                        if ( i == handler.num1) handler.req = spdr.setResrc(handler.numD)->getQualTag(i);
                }
                
                //fetches openings
                handler.opens = spdr.findAppts(handler.resrc, spdr.setPat(handler.numP), handler.req);
                break;
         
            case BROWSE:
    
                //fetches different openings
                handler.opens = spdr.findAppts(handler.resrc, spdr.setPat(handler.numP), handler.req, handler.num2);
                break;
                
            default:;
            }
            handler.fetch = BLANK;  //clear request
        
            //tells spider what to do
            switch (handler.tell) {
                
            case PRINT_PATS: spdr.printPats(); break;
            case PRINT_DOCS: handler.numDocs = spdr.printDocs(); break;
            case PRINT_D_APPTS: handler.resrc->printAppts(handler.num1); break;
            case PRINT_P_APPTS: handler.pat->printAppts(); break;
            case PRINT_PROCED: spdr.printProced(handler.resrc); break;
            case PRINT_AVAIL: spdr.printAvails(handler.opens); break;
            case APPT: spdr.convertToCommit(handler.resrc, spdr.setPat(handler.numP), handler.opens, handler.num2); break;
                
            default:;
            }
            handler.tell = BLANK;  //clear request
        }
    } catch (string& exception_string) {
        cout << exception_string;
    }
    cout << "Saving data..." << endl;
    spdr.saveAllAvailability();
    cout << "Bye!" << endl;
    delete[] docs;
    return 0;
}
