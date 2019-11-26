#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
using std::string;

#include "Appointment.h"

struct Availability {
    //open 1:open 0:cancel, what: H hrs, D days, M months; duration = hours
    void setAvail(bool open, int start, int duration, char what = 'H');    

    bool hours[24] =
     //00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};    //8 to 16 (4:59pm)
    bool weekDays[7]  = {1, 1, 1, 1, 1, 0, 0};    //M-F
    bool months[12]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
};

class Spider;  //forward declaration (friend class)

struct ApptNode {
    Appointment appt;
    ApptNode* next = nullptr;
};

class Resource {
 public: static const int MAX_QUALT = 5;
    
 protected:
    ApptNode* oblig[53][2];  //obligations, array of linked lists; 53 weeks, then either [0]:committed or [1]:open
    //node inventory, keeps track of number of nodes in each sorted linked list (1:ascending, 0:chrono)
    int nodeInv[53][2] = {{0, 0}};
    Time maxAvail[53];  //holds max available time slot for week
    string name;
    Availability general;  //general availability only, used to create initial avail appts
    Requirement qualTags[MAX_QUALT] = {NIL};  //for various qualifications that might be necessary for a given Appt
    int numProced = 0;
    
private:
    const string type;    //type of resource: doctor, room, portable equipment, etc..
    void setMaxAvail(int i, Time dur) { maxAvail[i] = dur; }  //sets max avail for each week
    void spoolAvail();  //spools up availability appts for a given resource
    int id;
    
public:
    Resource(string, string);  //provide type and name
    Resource(string, string, Availability);  //type, name, and general availability
    ~Resource();
    
    void addAppt(Appointment, bool = true);  //bool= add which appt 1:open, 0:committed
    void removeAppt(Time, Date, bool = true); //looks for appt by start time and date, bool for open or committment
    
    int getNodeInv(unsigned int i1, unsigned int i2) const { return nodeInv[i1][i2]; }
    void printAppts(int);  //prints to screen appts for the week
    
    string getType() const { return type; }    //type does not change
        
    string getName() const { return name; }
    void setName(string n) { name = n; }
    
    Availability getGenAvail() const { return general; };
    void setGenAvail(Availability a) { general = a; }
    
    Requirement getQualTag(unsigned int i) const { return qualTags[i]; }
    void setQualTag(unsigned int i, Requirement t);
    bool matchTag(Requirement);  //checks for match of passed tag
    
    int getNumProced() const { return numProced; }

    int getId() const { return id; }

    void setId(int resourceId) { id = resourceId; }

    void convertAvailabilityToAppointment(Appointment availability, Appointment appointment);

    void addAdminAppointment(Appointment appointment);

    friend class Spider;
};

#endif
