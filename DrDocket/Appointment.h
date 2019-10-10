#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using std::string;

#include "Time.h"
#include "Date.h"

#define MAX_REQS 5

enum Requirement { NIL, EXAM, BLOOD, XRAY, THEREPY};

class Appointment {
    
 private:
    string type;
    Requirement reqs[MAX_REQS];    //various requirements
    string rList;    //list of necessary resource types, quantity
    int numRTypes = 0;    //number of types of resources
    Time start;
    Time duration;
    Date day;

 public:
    Appointment() {};
    //commitment with doctor
    Appointment(string doc, int numR, Time dur, Requirement rq) { type = doc; numRTypes = numR; duration = dur; reqs[0] = rq; }
    //availability
    Appointment(string t, Time st, Time dur, Date d) { type = t; start = st; duration = dur; day = d; }
    
    string getType() const { return type; }    //type does not change
    
    Requirement getReq(unsigned int i) const { return reqs[i]; }
    void setReq(unsigned int i, Requirement t) { if (i < MAX_REQS) reqs[i] = t; }
    
    string getRList() const { return rList; }
    void setRList(string s) { rList = s; }
    
    int getRTypes() const { return numRTypes; }
    void setRTypes(int t) { numRTypes = t; }
    
    Time getStart() const { return start; }
    void setStart(Time t) { start = t; }
    
    Time getDuration() const { return duration; }
    void setDuration(Time t) { duration = t; }
    
    Date getDay() const { return day; }
    void setDay(Date d) { day = d; }
};

#endif
