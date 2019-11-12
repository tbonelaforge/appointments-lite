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

int numDoctors = 0;
int currentDoctorIndex = -1;
int currentQualTagIndex = -1;
string * docs = NULL;
int * doc_ids = NULL;
Spider spdr;


static int doctor_count_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {
    numDoctors = atoi(argv[0]);
    return 0;
}

static int doctor_procedure_row_callback(void * NotUsed, int argc, char ** argv, char ** col_names) {

    // Expecting rows having: d.id, d.last_name, p.id, p.name
    // e.g: 1|Alex|Brown|1|Checkup

    int doctor_id = atoi(argv[0]);
    string doctor_last_name = argv[1];
    int procedure_id = atoi(argv[2]);
    if (currentDoctorIndex < 0 || doc_ids[currentDoctorIndex] != doctor_id) {
        currentDoctorIndex += 1;
        docs[currentDoctorIndex] = doctor_last_name;
        doc_ids[currentDoctorIndex] = doctor_id;
        Resource* d = new Resource("Dr", docs[currentDoctorIndex]);
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

void open_database(sqlite3 ** db) {
    int rc = sqlite3_open("appointments.db", db);
    if (rc) {
        cout << "Error opening database: " << sqlite3_errmsg(*db) << endl;
        throw string("Can't open database");
    } else {
        cout << "Opened database successfully" << endl;
    }
}


void load_doctors(sqlite3 * db) {
    int rc;
    char * errorMessage = NULL;
    rc = sqlite3_exec(db, GET_DOCTORS_COUNT, doctor_count_callback, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING DOCTORS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load doctors");
    }
    docs = new string[numDoctors];
    doc_ids = new int[numDoctors];
    rc = sqlite3_exec(
            db,
            GET_DOCTORS_WITH_PROCEDURES,
            doctor_procedure_row_callback,
            0,
            &errorMessage
    );
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING DOCTORS WITH PROCEDURES: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(errorMessage);
        throw string("Can't load doctors");
    }
}

int main(int argc, char *argv[]) {
    cout << "Welcome to DrDocket!" << endl;
    sqlite3 * db;
    try {
        open_database(&db);
        load_doctors(db);

        Availability nonLaborAvail;
        nonLaborAvail.setAvail(true, 1, 22);
        nonLaborAvail.setAvail(true, 0, 7, 'D');
        nonLaborAvail.setAvail(true, 0, 12, 'M');
        string rooms[5] = {"Exam A", "Exam B", "Lab", "X-ray", "Physiotherapy"};
        for (int i = 0; i < 5; ++i) {
            Resource* r = new Resource("Room", rooms[i], nonLaborAvail);
            spdr.addResrc(r);
        }
        spdr.setResrc(4)->setQualTag(0, EXAM);
        spdr.setResrc(5)->setQualTag(0, EXAM);
        spdr.setResrc(6)->setQualTag(0, BLOOD);
        spdr.setResrc(7)->setQualTag(0, XRAY);
        spdr.setResrc(8)->setQualTag(0, THEREPY);

        string intake[3] = {"Fay Zhong", "Shane Hightower", "Brad Bradford"};
        for (int i = 0; i < 3; ++i) {
            Patient* p = new Patient(intake[i]);
            spdr.addPat(p);
        }
    
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
    cout << "Bye!" << endl;
    delete[] docs;
    return 0;
}
