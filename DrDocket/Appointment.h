#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
using std::string;


#include "Time.h"
#include "Date.h"

enum Requirement { NIL, EXAM, BLOOD, XRAY, THEREPY};


using namespace std;

class Appointment {
 public:
    static const int MAX_REQS = 5;
    static string formatDatetime(Date date, Time time) {
        char buf[100];

        // Use format: 2016-01-01 10:20
        snprintf(
           buf,
           sizeof(buf),
           "%04d-%02d-%02d %02d:%02d",
           date.getYear(),
           date.getMonth() + 1,
           date.getDay(),
           time.getHr(),
           time.getMn()
        );
        string result = buf;
        return result;
    }

    static void parseDatetime(string datetime, Date& date, Time& time) {

        // Use format: 2016-01-01 10:20
        replace(datetime.begin(), datetime.end(), '-', ' ');
        std::replace(datetime.begin(), datetime.end(), ':', ' ');
        stringstream data(datetime);
        int year, month, day, hour, minute;
        data >> year >> month >> day >> hour >> minute;
        date.initialize(Date::parseMonth(month), day, year);
        time.setHr(hour);
        time.setMn(minute);
    }

    static void parseRequirement(Requirement req, Time& dur, string& type) {
        switch (req) {
            case EXAM:
                dur.setMn(45);
                type = "Exam";
                break;

            case BLOOD:
                dur.setMn(15);
                type = "Lab";
                break;

            case THEREPY:
                dur.setHr(1);
                type = "Therepy";
                break;

            case XRAY:
                dur.setMn(20);
                type = "X-Ray";
                break;

            case NIL: throw "Invalid Requirement for Appointment";
        }
    }

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

    string formatStartDatetime() {
        Date startDate = getDay();
        Time startTime = getStart();
        return Appointment::formatDatetime(startDate, startTime);
    }

    string formatEndDatetime() {
        Date endDate = getDay();
        Time endTime = getEnd();
        return Appointment::formatDatetime(endDate, endTime);
    }

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
        out << "Appt: " << getRList() << endl;
        out << " |-week:" << getDay().getWeekNum() << endl;
        out << " |-day:" << getDay().getWeekday() << endl;
        out << " |-type: " << getType() << endl;
        out << " |-start: " << formatStartDatetime() << endl;
        out << " |-end: " << formatEndDatetime() << endl;
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
