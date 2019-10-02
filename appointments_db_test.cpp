#include <iostream>
#include <sqlite3.h>
#include <cstddef>

using namespace std;

const char * GET_ROOMS = "select r.number as room_number, e.name as equipment_name from room r left join room_equipment re on re.room_id = r.id left join equipment e on re.equipment_id = e.id";

const char * GET_PROCEDURES = "select p.name as procedure_name, e.name as equipment_name, p.length as procedure_length from procedure p left join procedure_equipment pe on pe.procedure_id = p.id left join equipment e on e.id = pe.equipment_id";

const char * GET_DOCTORS = "select d.last_name as doctor_name, p.name as procedure_name from doctor d left join doctor_procedure dp on dp.doctor_id = d.id left join procedure p on p.id = dp.procedure_id";


const char * CREATE_TABLE_DOCTOR = "create table doctor (id int, first_name varchar(30), last_name varchar(30))";

const char * INSERT_TABLE_DOCTOR = "insert into doctor (id, first_name, last_name) values (1, 'Alex', 'Brown'), (2, 'Jessica', 'White'), (3, 'Samir', 'Sreedhar'), (4, 'Zelda', 'Nuremburg')";


const char * CREATE_TABLE_ROOM = "create table room (id int, number varchar(30), capacity int)";

const char * INSERT_TABLE_ROOM = "insert into room (id, number, capacity) values (1, '3A', 4), (2, '3B', 4), (3, '4', 3), (4, 'XR', 2), (5, '12', 7)";


const char * CREATE_TABLE_PROCEDURE = "create table procedure (id, name, length)";

const char * INSERT_TABLE_PROCEDURE = "insert into procedure (id, name, length) values (1, 'Checkup', 45), (2, 'Blood Draw', 30), (3, 'Trauma Exam', 60), (4, 'Physical Therapy', 45)";


const char * CREATE_TABLE_EQUIPMENT = "create table equipment (id, name)";

const char * INSERT_TABLE_EQUIPMENT = "insert into equipment (id, name) values (1, 'Exam Table'), (2, 'Phlebotomy Chair'), (3, 'X-Ray Machine'), (4, 'Physiotherapy Machine')";


const char * CREATE_TABLE_ROOM_EQUIPMENT = "create table room_equipment (id, room_id, equipment_id)";

const char * INSERT_TABLE_ROOM_EQUIPMENT = "insert into room_equipment (id, room_id, equipment_id) values (1, 1, 1), (2, 2, 1), (3, 3, 2), (4, 4, 3), (5, 5, 4), (6, 5, 1)";


const char * CREATE_TABLE_PROCEDURE_EQUIPMENT = "create table procedure_equipment (id, procedure_id, equipment_id)";

const char * INSERT_TABLE_PROCEDURE_EQUIPMENT = "insert into procedure_equipment (id, procedure_id, equipment_id) values (1, 1, 1), (2, 2, 2), (3, 3, 3), (4, 4, 4)";


const char * CREATE_TABLE_DOCTOR_PROCEDURE = "create table doctor_procedure (id, doctor_id, procedure_id)";

const char * INSERT_TABLE_DOCTOR_PROCEDURE = "insert into doctor_procedure (id, doctor_id, procedure_id) values (1, 1, 3), (2, 3, 3), (3, 2, 2), (4, 4, 4), (5, 1, 1), (6, 2, 1), (7, 3, 1), (8, 4, 1)";

static int callback(void * NotUsed, int argc, char ** argv, char ** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

int main() {
    sqlite3 * db;
    char * zErrMsg = NULL;
    int rc = sqlite3_open("appointments.db", &db);
    if (rc) {
        cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully" << endl << endl;
    }
    // Create the tables.
    const int TABLES = 7;
    const char * CREATE_SQL[TABLES];
    cout << "Creating tables:" << endl;
    CREATE_SQL[0] = CREATE_TABLE_DOCTOR;
    CREATE_SQL[1] = CREATE_TABLE_ROOM;
    CREATE_SQL[2] = CREATE_TABLE_PROCEDURE;
    CREATE_SQL[3] = CREATE_TABLE_EQUIPMENT;
    CREATE_SQL[4] = CREATE_TABLE_ROOM_EQUIPMENT;
    CREATE_SQL[5] = CREATE_TABLE_PROCEDURE_EQUIPMENT;
    CREATE_SQL[6] = CREATE_TABLE_DOCTOR_PROCEDURE;
    for (int i = 0; i < TABLES; i++) {
        rc = sqlite3_exec(db, CREATE_SQL[i], callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL ERROR CREATING TABLE " << i << ": " << sqlite3_errmsg(db) << endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            return 0;
        }
    }


    // Populate the tables.
    const char * INSERT_SQL[TABLES];
    cout << "Populating tables:" << endl;
    INSERT_SQL[0] = INSERT_TABLE_DOCTOR;
    INSERT_SQL[1] = INSERT_TABLE_ROOM;
    INSERT_SQL[2] = INSERT_TABLE_PROCEDURE;
    INSERT_SQL[3] = INSERT_TABLE_EQUIPMENT;
    INSERT_SQL[4] = INSERT_TABLE_ROOM_EQUIPMENT;
    INSERT_SQL[5] = INSERT_TABLE_PROCEDURE_EQUIPMENT;
    INSERT_SQL[6] = INSERT_TABLE_DOCTOR_PROCEDURE;
    for (int i = 0; i < TABLES; i++) {
        rc = sqlite3_exec(db, INSERT_SQL[i], callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL ERROR POPULATING TABLE " << i << ": " << sqlite3_errmsg(db) << endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            return 0;
        }
    }


    // Query the tables.
    cout << "Getting all rooms with their equipment..." << endl;
    rc = sqlite3_exec(db, GET_ROOMS, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING ROOMS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
    }

    cout << "Getting all procedures with their equipment..." << endl;
    rc = sqlite3_exec(db, GET_PROCEDURES, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING PROCEDURES: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
    }

    cout << "Getting all doctors and the procedures they can perform..." << endl;
    rc = sqlite3_exec(db, GET_DOCTORS, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL ERROR GETTING DOCTORS: " << sqlite3_errmsg(db) << endl;
        sqlite3_free(zErrMsg);
    }

    sqlite3_close(db);
    return 0;
}
