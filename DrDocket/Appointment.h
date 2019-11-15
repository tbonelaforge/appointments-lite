#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <iostream>
using std::string;

#include "Time.h"
#include "Date.h"

enum Requirement { NIL, EXAM, BLOOD, XRAY, THEREPY};

using std::ostream;
using std::endl;

class Appointment {
 public: static const int MAX_REQS = 5;
    
 private:
    string type;
    Requirement reqs[MAX_REQS];    //various requirements
    string rList;    //list of necessary resources (other than doctor)
    int numRTypes = 0;    //number of types of resources
    Time start;
    Time duration;
    Date day;

 public:
    Appointment() {};
    //commitment with doctor
    Appointment(string doc, int numR, Time dur, Requirement rq)
        { type = doc; numRTypes = numR; duration = dur; reqs[0] = rq; }
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

    Time getEnd() const { return getStart() + getDuration(); }
    
    Date getDay() const { return day; }
    void setDay(Date d) { day = d; }

    void prettyPrint(ostream& out, int appointmentNumber) {
        Date day;
        Time start;
        Time length;
        day = getDay();
        start = getStart();
        length = getDuration();
        out << "Appt #" << appointmentNumber << " > ";
        day.outDate(out);
        out << " |-type: " << getType();
        out << " |-start: ";
        start.timeOut(out);
        out << " |-length: ";
        length.timeOut(out);
    }

    void debugPrint(ostream& out) {
        Date day;
        Time start;
        Time length;
        day = getDay();
        start = getStart();
        length = getDuration();
        //Time end;
        //end = start + length;
        out << "Appt: " << getRList() << endl;
        out << " |-type: " << getType();
        out << " |-start: ";
        start.timeOut(out);
        out << " on ";
        day.outDate(out, true);
        out << " |-end: ";
        getEnd().timeOut(out);
        out << " on ";
        day.outDate(out, true);
        out << endl;
        out << "Procedure: " << reqs[0];
        switch (reqs[0]) {
            case EXAM:
                out << "Exam";
                break;
            case BLOOD:
                out << "Blood";
                break;
            case XRAY:
                out << "X-Ray";
                break;
            case THEREPY:
                out << "Therapy";
                break;
        }
    }
};

#endif
