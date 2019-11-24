#ifndef SPIDER_H
#define SPIDER_H

#include "Resource.h"
#include "Patient.h"
#include <vector>
#include <iostream>

#include "sqlite3.h"

using std::endl;

struct Opens {
    Appointment appt[3];  //found openings
    int weekNums[3];
    Appointment convertAvail[2][3]; //doc and room avail appts to be converted; 0:docs 1:rooms
    bool isGood[3] = {false, false, false};  //counter: false = still need more opens, all true means no
    Resource* strands[3] = {nullptr, nullptr, nullptr};  //spider strand to each room
    void printIsGood(ostream& out) {
        for (int i = 0; i < 3; i++) {
            out << ((isGood[i]) ? "true" : "false") << ", ";
        }
        out << endl;
    }
};

//stiches to together patients and resources for appointments
class Spider {
    
 private:
    std::vector<Resource*> resrcs;
    std::vector<Patient*> pats;
    sqlite3 * db;
    void saveAppointment(int doctorId, int roomId, int procedureId, int patientId, string startDatetime, string endDatetime, int week);
    void updateAvailability(Resource * resource);

 public:
    void convertToCommit(Resource*, Patient*, Opens &, int);
    void cancel(Appointment appt);
    Opens findAppts(Resource*, Patient*, Requirement, int = 0);
    bool findMatchTime(Resource*, Opens &);  //takes an appt and checks another resource for avail
    
    void addResrc(Resource* r) { resrcs.push_back(r); }  //Doctors, rooms ...
    Resource* setResrc(int i) { return resrcs.at(i); }
    Resource* getResrc(int i) const { return resrcs.at(i); }
    
    void addPat(Patient* p) { pats.push_back(p); }
    Patient* setPat(int i) { return pats.at(i); }
    Patient* getPat(int i) const { return pats.at(i); }
    void removePat(string);
    
    int getNumResrcs() { return static_cast<int> (resrcs.size()); }
    int getNumPats() { return static_cast<int> (pats.size()); }
    
    void printPats();
    int printDocs();  //prints out doctors from resrcs, returns number of doctors
    void printProced(Resource*);  //prints out qualified procedures with a given doctor
    void printAvails(Opens);  //prints out availability slots for patient to choose from
    void setDb(sqlite3 * p) {
        db = p;
    };
    sqlite3 * getDb() {
        return db;
    }
    void saveAllAvailability();
    void loadAvailability(Resource * resource);
    void loadAppointments(Resource * resource);
};

#endif
