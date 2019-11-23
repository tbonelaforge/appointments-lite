/*
----------------------------
Availability : struct
----------------------------
+ hours :  bool []
+ weekDays :  bool []
+ months : bool []

+ setAvail(bool, int, int, char = 'H')
----------------------------

----------------------------
Resource
----------------------------
- oblig :  ApptNode* [] [] 
- nodeInv : int [] []
- type : string
- name :  string
- general :  Availability
- qualTags :  Type []

- setMaxAvail(int) : void
----------------------------
+ Resource(string, string)
+ ~Resource()

+ addAppt(Appointment, bool = true) : void
+ removeAppt(Time, Date, bool) : void

+ getNodeInv(unsigned int, unsigned int) : int
+ printAppts(unsigned int) : void

+ getType() : string

+ getName() : string
+ setName(string) : void

+ getGenAvail() : Availability
+ setGenAvail(Availability a) : void

+ getQualTag(unsigned int i) : Type
+ setQualTag(unsigned int i, Type t) : void
----------------------------
*/

#include <iostream>
#include "Resource.h"

using std::cout;
using std::endl;

//which 1:open 0:cancel, what: H hrs, D days, M months; duration = hours
void Availability::setAvail(bool open, int start, int duration, char what) {
        if (what == 'H')
            for (int i = start; i < start + duration; ++i) hours[i] = open;
        else if (what == 'D')
            for (int i = start; i < start + duration; ++i) weekDays[i] = open;
        else if (what == 'M')
            for (int i = start; i < start + duration; ++i) months[i] = open;
}

Resource::Resource(string t, string n) : type(t) {
    name = n;
    id = 0;
    if (type == "Patient") return;  //Patients don't use general availability
    spoolAvail();
}
Resource::Resource(string t, string n, Availability a) : type(t) {
    general = a;
    name = n;
    id = 0;
    spoolAvail();
}
void Resource::spoolAvail() {
    Time start, duration;
    Date date;    //default set to 1/1/2019
    int countHours = 0;
    bool toggle = false;
    int shift = date.getWeekday();  //temporarily shifts days based on set weekday, {then shifts weeknum after new year}cancelled for now
    int days = date.findWeekNum(date.getMonth(), date.getDay(), true);  //true = alt = findDayNum
    const int daysTot = (date.getYear() % 4) ? 365 : 366;
//    const int startYear = date.getYear();  //may use later if +year
    
    for (int i = 0; i < 53; ++i) {  //for each week
        
        for(int j = 0 + shift; j < 7; ++j) {  //for each day
            if (days++ > daysTot) break;  //stop when exceed one year
            date.setWeekday(static_cast<Weekday> (j));
            shift = 0;  //clear shift
            if (general.weekDays[j]) {
                
                for (int k = 0; k < 24; ++k) {  //for each hour of general avail
                
                    //count each whole block of hours then set appt
                    if (toggle) {
                        if (general.hours[k]) ++countHours;
                        else toggle = false;
                    }
                    else {
                        if (general.hours[k]) {
                            toggle = true;
                            countHours = 1;
                            start.setHr(k);
                        }
                    }
                    if (countHours > 0 && !toggle) {
                        Month m;
                        int day;
//                        if (startYear < date.getYear()) shift = 52;  //may incorporate +year later
                        date.setWeekNum(i - shift);
                        date.findMonthNDay(i - shift, static_cast<Weekday> (j), m, day);
                        date.setMonth(m);
                        date.setDay(day);
                        duration.setHr(countHours);
                        Appointment appt("Available", start, duration, date);
                        addAppt(appt);
                        countHours = 0;
                    }
                }
            }
        }
    }
}
Resource::~Resource() {
    ApptNode* current;
    ApptNode* prev;
    for (int i = 0; i < 53; ++i) {
        current = oblig[i][0];
        for (int j = 0; j < nodeInv[i][0]; ++j) {
            prev = current;
            current = current->next;
            delete prev;
        }
        current = oblig[i][1];
        for (int j = 0; j < nodeInv[i][1]; ++j) {
            prev = current;
            current = current->next;
            delete prev;
        }
    }
}
void Resource::setQualTag(unsigned int i, Requirement t) {
    if (i < MAX_QUALT) qualTags[i] = t;
    if (t != NIL) ++numProced;
    if (t == NIL) --numProced;
}
bool Resource::matchTag(Requirement rq) {
    for (int i = 0; i < MAX_QUALT; ++i) {
        if (qualTags[i] == rq) return true;
    }
    return false;
}

//bool= add which appt 1:open, 0:commit; default open
void Resource::addAppt(Appointment appt, bool open) {
    ApptNode* node = new ApptNode;
    node->appt = appt;
    int weekNum = appt.getDay().getWeekNum();
    ApptNode* current = oblig[weekNum][open];
    ApptNode* prev = nullptr;
    
    //place head
    if (nodeInv[weekNum][open] == 0) {
        oblig[weekNum][open] = node;
    }
    else {
        if (open) {
            
            //opens: sorted list, ascending by duration, then can find the next longest time slot for new appointment
            for (int i = 0; i < nodeInv[weekNum][1]; ++i) {
                if (current->appt.getDuration() < node->appt.getDuration()) {
                    prev = current;
                    current = current->next;
                } else break;
            }
        }
        else {
            
            //commits: sorted list, ascending by date/time
            for (int i = 0; i < nodeInv[weekNum][0]; ++i) {
                if (current->appt.getDay() <= node->appt.getDay()) {
                    if (current->appt.getStart() < node->appt.getStart()) {
                        prev = current;
                        current = current->next;
                    }
                } else break;
            }
        }
       
        //swap head
        if (prev == nullptr) {
            node->next = oblig[weekNum][open];
            oblig[weekNum][open] = node;
        }
        
        //add in middle or end
        else {
            node->next = current;
            prev->next = node; 
        }
    }
    ++nodeInv[weekNum][open];
    if (open && appt.getDuration() > maxAvail[weekNum]) setMaxAvail(weekNum, appt.getDuration());
}
void Resource::removeAppt(Time start, Date day, bool open) {
    int weekNum = day.getWeekNum();
    if (nodeInv[weekNum][open] == 0) throw "List Empty!";
    
    ApptNode* current = oblig[weekNum][open];
    ApptNode* prev = nullptr;
    
    //one node + match
    if (nodeInv[weekNum][open] == 1 && current->appt.getStart() == start && current->appt.getDay() == day) {
        delete current;
        oblig[weekNum][open] = nullptr;
        if (open) setMaxAvail(weekNum, Time(0));
    }
    
    //head match and >1 node
    else if (current->appt.getStart() == start && current->appt.getDay() == day) {
        oblig[weekNum][open] = current->next;
        delete current;
    }
    else {
        bool found = false;
        prev = current;
        current = current->next;
            
        for (int i = 1; i < nodeInv[weekNum][open]; ++i) {
            if (current->appt.getStart() != start || current->appt.getDay() != day) {
                prev = current;
                current = current->next;
            }
            else {
                prev->next = current->next;
                delete current;
                found = true;
                //if i is last and open, then set max avail to last node
                if (open && i == nodeInv[weekNum][1] - 1) setMaxAvail(weekNum, prev->appt.getDuration());
                break;
            }
        }
        if (!found) throw "Appointment missing!";
    }
    --nodeInv[weekNum][open];
}
void Resource::printAppts(int weekNum) {
    if (weekNum > 53) throw "WeekNum too large!";
    
    ApptNode* current = oblig[weekNum][0];
    Date day;
    Time start;
    Time length;
    for (int i = 0; i < nodeInv[weekNum][0]; ++i) {
        day = current->appt.getDay();
        start = current->appt.getStart();
        length = current->appt.getDuration();
        cout << "Appt #" << i + 1 << " > ";
        day.outDate(cout);
        cout << " |-type: " << current->appt.getType();
        cout << " |-start: ";
        start.timeOut(cout);
        cout << " |-length: ";
        length.timeOut(cout);
        cout << endl;
        current = current->next;
    }
    cout << endl;
}

void Resource::convertAvailabilityToAppointment(Appointment availability, Appointment appointment) {

    // Remove an availability, add an appointment
    removeAppt(availability.getStart(), availability.getDay());  //remove open slot
    addAppt(appointment, 0);  //commit to appt
    if (type == "Dr") { // commit to paperwork
        Appointment admin = Appointment(
                "Admin",
                appointment.getStart() + appointment.getDuration(),
                Time(0, 5),
                availability.getDay()
        );  //paperwork for doc
        addAppt(admin, 0);
    }

    //make up to two new available appt refunded from the open slot used
    Appointment newAvailability = availability; //same day, may be same start time
    Time RefundPrior = appointment.getStart() - availability.getStart();
    Time RefundAfter = availability.getDuration() - RefundPrior - appointment.getDuration();
    if (type == "Dr") {
        RefundAfter = RefundAfter - Time(0, 5);
    }
    if (RefundPrior > Time(0)) {
        newAvailability.setDuration(RefundPrior);
        addAppt(newAvailability);
    }
    if (RefundAfter > Time(0)) {
        Time afterAppointment = appointment.getStart() + appointment.getDuration();
        if (type == "Dr") {
            afterAppointment = afterAppointment + Time(0,5);
        }
        newAvailability.setStart(afterAppointment);
        newAvailability.setDuration(RefundAfter);
        addAppt(newAvailability);
    }
}