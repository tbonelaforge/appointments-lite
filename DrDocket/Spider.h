#ifndef SPIDER_H
#define SPIDER_H

#include "Resource.h"
#include "Patient.h"
#include <vector>

struct Opens {
    Appointment appt[3];  //found openings
    int nodeNum[2][3] {{0,0,0}};  //where can the nodes be found for each open and each resource; 0:doc 1:room
    bool isGood[3] {false, false, false};  //counter: false = still need more opens, all true means no
    Time dur;  //passed duration of first resources's avail window
    Resource* strands[3] {nullptr, nullptr, nullptr};  //spider strand to each room
};

//stiches to together patients and resources for appointments
class Spider {
    
 private:
    std::vector<Resource*> resrcs;
    std::vector<Patient*> pats;
    
 public:
    void convertToCommit(Appointment appt);
    void cancel(Appointment appt);
    Opens findAppts(Resource*, Resource*, Requirement, int = 0);
    bool findMatchTime(Resource*, Opens &);  //takes an appt and checks another resource for avail
    
    void addResrc(Resource* r) { resrcs.push_back(r); }  //Doctors, rooms ...
    Resource* setResrc(int i) { return resrcs.at(i); }
    
    void addPat(Patient* p) { pats.push_back(p); }
    void removePat(string);
    
    int getNumResrcs() { return static_cast<int> (resrcs.size()); }
    int getNumPats() { return static_cast<int> (pats.size()); }
    
    void printPats();
    int printDocs();  //prints out doctors from resrcs, returns number of doctors
    void printProced(Resource*);  //prints out qualified procedures with a given doctor
    void printAppts(Opens);  //prints out possible appts for patient to choose from
};

#endif
