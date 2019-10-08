#include <iostream>
#include <string>
#include <cstddef>
#include <cstdio>

#include "sqlite3.h"

#include "Patient.h"
#include "Doctor.h"
#include "Procedure.h"



const char * GET_PATIENTS_BY_LAST_NAME = "select id, first_name, last_name from patient order by last_name";

const char * GET_PATIENTS_COUNT = "select count(*) from patient";


const char * GET_PROCEDURES_BY_NAME = "select id, name, length from procedure order by name";

const char * GET_PROCEDURES_COUNT = "select count(*) from procedure";


const char * GET_SUITABLE_DOCTORS_COUNT_TEMPLATE = "select count(*) from doctor_procedure dp join doctor d on d.id = dp.doctor_id where dp.procedure_id = %d";

char GET_SUITABLE_DOCTORS_COUNT_QUERY[1000];

const char * GET_SUITABLE_DOCTORS_TEMPLATE = "select d.id, d.first_name, d.last_name from doctor_procedure dp join doctor d on d.id = dp.doctor_id where dp.procedure_id = %d";


char GET_SUITABLE_DOCTORS_QUERY[1000];

void prepare_suitable_doctors_queries(int procedure_id) {
    GET_SUITABLE_DOCTORS_COUNT_QUERY[0] = '\n';
    sprintf(GET_SUITABLE_DOCTORS_COUNT_QUERY, GET_SUITABLE_DOCTORS_COUNT_TEMPLATE, procedure_id);
    GET_SUITABLE_DOCTORS_QUERY[0] = '\n';
    sprintf(GET_SUITABLE_DOCTORS_QUERY, GET_SUITABLE_DOCTORS_TEMPLATE, procedure_id);
    cout << "After prepare_suitable_doctors_queries, THE GET_SUITABLE_DOCTORS_QUERY is:" << GET_SUITABLE_DOCTORS_QUERY << endl;
}

Patient * allPatients = NULL;
int numPatients = 0;
int currentPatientIndex = -1;

Procedure * allProcedures = NULL;
int numProcedures = 0;
int currentProcedureIndex = -1;

Doctor * allSuitableDoctors = NULL;
int numSuitableDoctors = 0;
int currentSuitableDoctorIndex = -1;


static int patient_count_callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    numPatients = atoi(argv[0]);
    return 0;
}


static int patient_row_callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    int id = atoi(argv[0]);
    string firstName(argv[1]);
    string lastName(argv[2]);
    Patient patient(id, firstName, lastName);
    currentPatientIndex += 1;
    allPatients[currentPatientIndex] = patient;
    return 0;
}


static int procedure_count_callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    numProcedures = atoi(argv[0]);
    return 0;
}


static int procedure_row_callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    int id = atoi(argv[0]);
    string name(argv[1]);
    int length = atoi(argv[2]);
    Procedure procedure(id, name, length);
    currentProcedureIndex += 1;
    allProcedures[currentProcedureIndex] = procedure;
    return 0;
}


static int  suitable_doctors_count_callback(void * NotUsed, int argc, char ** argv, char ** azXolName) {

    cout << "Inside suitable_doctors_count_callback, got called with argc:...\n" << argc << endl;
    numSuitableDoctors = atoi(argv[0]);
    cout << "Just set numSuitableDoctors to : " << numSuitableDoctors << endl;
    return 0;
}


static int suitable_doctors_callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    cout << "Inside suitable_doctors_callback, got callled....with argc:..." << argc << endl;
    int id = atoi(argv[0]);
    cout << "line1" << endl;
    string firstName(argv[1]);
    cout << "line2" << endl;
    string lastName(argv[2]);
    cout << "line3" << endl;
    Doctor doctor(id, firstName, lastName);
    cout << "line4" << endl;
    currentSuitableDoctorIndex += 1;
    cout << "line5" << endl;
    allSuitableDoctors[currentSuitableDoctorIndex] = doctor;
    cout << "line6" << endl;
    return 0;
}

void load_suitable_doctors(sqlite3 * db) {
    int rc;
    char * zErrMsg = NULL;
    Procedure procedure = allProcedures[currentProcedureIndex];
    prepare_suitable_doctors_queries(procedure.getId());
    cout << "about to execute the GET_SUITABLE_DOCTORS_COUNT_QUERY...\n";
    rc = sqlite3_exec(db, GET_SUITABLE_DOCTORS_COUNT_QUERY, suitable_doctors_count_callback, 0, &zErrMsg);
    cout << "made it past the get_suitable_doctors_count_query...\n";
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING SUITABLE DOCTORS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't load suitable doctors");
    }
    allSuitableDoctors = new Doctor[numSuitableDoctors];
    cout << "About to execute the GET_SUITABLE_DOCTORS_QUERY...\n";
    rc = sqlite3_exec(db, GET_SUITABLE_DOCTORS_QUERY, suitable_doctors_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING SUITABLE DOCTORS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't load suitable doctors");
    }
}


void load_patients(sqlite3 * db) {
    int rc;
    char * zErrMsg = NULL;
    rc = sqlite3_exec(db, GET_PATIENTS_COUNT, patient_count_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING PATIENTS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't load patients");
    }
    allPatients = new Patient[numPatients];
    rc = sqlite3_exec(db, GET_PATIENTS_BY_LAST_NAME, patient_row_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING PATIENTS BY LAST NAME: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Error Loading Patients");
    }
}


void load_procedures(sqlite3 * db) {
    int rc;
    char * zErrMsg = NULL;
    rc = sqlite3_exec(db, GET_PROCEDURES_COUNT, procedure_count_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR COUNTING PROCEDURES: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't load procedures");
    }
    allProcedures = new Procedure[numProcedures];
    rc = sqlite3_exec(db, GET_PROCEDURES_BY_NAME, procedure_row_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING PROCEDURES BY LAST NAME: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
        throw string("Can't load procedures");
    }
}


void displayMenu(int state) {
    cout << "Inside displayMenu, got called with state: " << state << endl;
    if (state == 0) {
        cout << "Select a patient by number:" << endl;
        for (int i = 0; i < numPatients; i++) {
            Patient patient = allPatients[i];
            cout << "Type " << i + 1 << " to select " << patient.getFullName() << endl;
        }
        cout << "Type Q to quit" << endl;
    } else if (state == 1) {
        cout << "Select a procedure by number:" << endl;
        for (int i = 0; i < numProcedures; i++) {
            Procedure procedure = allProcedures[i];
            cout << "Type " << i + 1 << " to select " << procedure.getName() << endl;
        }
        cout << "Type Q to quit" << endl;
    } else if (state == 2) {
        cout << "Select a suitable doctor by number:" << endl;
        for (int i = 0; i < numSuitableDoctors; i++) {
            Doctor doctor = allSuitableDoctors[i];
            cout << "Type " << i + 1 << " to select " << doctor.getFullName() << endl;
        }
        cout << "Type Q to quit" << endl;
    }
}


void handleSelectPatientCommand(string command, int &state, int &selectedPatientIndex) {
    if (command == "Q") {
        state = 3;
        return;
    }
    int selection;
    try {
        selection = stoi(command);
        selectedPatientIndex = selection - 1;
        cout << "Selected " 
             << allPatients[selectedPatientIndex].getFullName() << endl;
        state = 1;
    } catch (std::exception e) {
        cout << "Invalid patient selection: Enter a number between 1 and " << numPatients << endl;
    }
}


void handleSelectProcedureCommand(string command, int &state, int &selectedProcedureIndex) {
    if (command == "Q") {
        state = 3;
        return;
    }
    int selection;
    try {
        selection = stoi(command);
        selectedProcedureIndex = selection - 1;
        cout << "Selected " 
             << allProcedures[selectedProcedureIndex].getName() << endl;
        state = 2;
    } catch (std::exception e) {
        cout << "Invalid procedure selection: Enter a number between 1 and " << numProcedures << endl;
    }
}

void handleSelectDoctorCommand(string command, int &state, int &selectedDoctorIndex) {
    if (command == "Q") {
        state = 3;
        return;
    }
    int selection;
    try {
        selection = stoi(command);
        selectedDoctorIndex = selection - 1;
        cout << "Selected "
             << allSuitableDoctors[selectedDoctorIndex].getFullName() << endl;
        state = 3;
    } catch (std::exception e) {
        cout << "Invalid doctor selection: Enter a number between 1 and " << numSuitableDoctors << endl;
    }
}


void open_database(sqlite3 ** db ) {
    int rc = sqlite3_open("appointments.db", db);
    if (rc) {
        cout << "Error opening database: " << sqlite3_errmsg(*db) << endl;
        throw string("Can't open database");
    } else {
        cout << "Opened database successfully" << endl;
    }
}







void print_suitable_doctors() {
    for (int i = 0; i < numSuitableDoctors; i++) {
        Doctor doctor = allSuitableDoctors[currentSuitableDoctorIndex];
        
    }
}


int main() {
    sqlite3 * db;

    try {
        open_database(&db);
        load_patients(db);
        load_procedures(db);
        int state = 0;
        string command;
        int selectedPatientIndex = -1;
        int selectedProcedureIndex = -1;
        int selectedDoctorIndex = -1;
        while (state < 3) {
            displayMenu(state);
            cin >> command;
            if (state == 0) {
                handleSelectPatientCommand(command, state, selectedPatientIndex);
            } else if (state == 1) {
                handleSelectProcedureCommand(command, state, selectedProcedureIndex);
                load_suitable_doctors(db);
                cout << "Made it past the load_suitable_doctors CALL..\n";
            } else if (state == 2) {
                handleSelectDoctorCommand(command, state, selectedDoctorIndex);
            }
        }
    } catch (string& exception) {
        cout << exception;
    }
    delete[] allPatients;
    sqlite3_close(db);
    return 0;
}
