#include <iostream>
#include <string>
#include <cstddef>

#include "sqlite3.h"

#include "Patient.h"
#include "Doctor.h"
#include "Procedure.h"



const char * GET_PATIENTS_BY_LAST_NAME = "select id, first_name, last_name from patient order by last_name";

const char * GET_PATIENTS_COUNT = "select count(*) from patient";


const char * GET_PROCEDURES_BY_NAME = "select id, name, length from procedure order by name";

const char * GET_PROCEDURES_COUNT = "select count(*) from procedure";




Patient * allPatients = NULL;
int numPatients = 0;
int currentPatientIndex = -1;

Procedure * allProcedures = NULL;
int numProcedures = 0;
int currentProcedureIndex = -1;


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


void displayMenu(int state) {
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
        state = 3;
    } catch (std::exception e) {
        cout << "Invalid procedure selection: Enter a number between 1 and " << numProcedures << endl;
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
        while (state < 3) {
            displayMenu(state);
            cin >> command;
            if (state == 0) {
                handleSelectPatientCommand(command, state, selectedPatientIndex);
            } else if (state == 1) {
                handleSelectProcedureCommand(command, state, selectedProcedureIndex);
            }
        }
    } catch (string& exception) {
        cout << exception;
    }
    delete[] allPatients;
    sqlite3_close(db);
    return 0;
}
