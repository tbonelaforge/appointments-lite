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

Resource::Resource(const string t, string n) : type(t) {
    name = n;
    Time start, duration;
    Date date;    //default set to 1/1/2019
    if (type == "Patient") return;  //Patients don't use general availability
    
    int countHours = 0;
    bool toggle = false;
    int shift = date.getWeekday();  //temporarily shifts days based on set weekday, then shifts weeknum after new year
    int days = date.findWeekNum(date.getMonth(), date.getDay(), true);  //true = alt = findDayNum
    const int daysTot = (date.getYear() % 4) ? 365 : 366;
    const int startYear = date.getYear();
    
    for (int i = 0; i < 53; ++i) {  //for each week
        
        for(int j = 0 + shift; j < 7; ++j) {  //for each day
            date.setWeekday(static_cast<Weekday> (j));
            shift = 0;  //clear shift
            if (days++ > daysTot) { //increments days; increments year if days exceed daysTot
                date.addYear(1);
                days = 1;
            }
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
                        if (startYear < date.getYear()) shift = 52;
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
Resource::Resource(string t, string n, Availability a) {
    general = a;
    Resource(t, n);
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
bool Resource::matchTag(Resource* r, Requirement rq) {
    bool match1 = false, match2 = false;
    for (int i = 0; i < MAX_QUALT; ++i) {
        if (r->getQualTag(i) == rq) {
            match1 = true;
            break;
        }
    }
    for (int i = 0; i < MAX_QUALT; ++i) {
        if (this->getQualTag(i) == rq) {
            match2 = true;
            break;
        }
    }
    return (match2 && match1);
}

//bool= add which appt 1:open, 0:committed; default open
void Resource::addAppt(Appointment appt, bool open) {
    ApptNode* node = new ApptNode;
    node->appt = appt;
    int weekNum = appt.getDay().getWeekNum();
    ApptNode* current = oblig[weekNum][open];
    ApptNode* prev = nullptr;
    if (nodeInv[weekNum][open] == 0) {
        oblig[weekNum][open] = node;
    }
    else {
       
        //sorted list, ascending, then can find the next longest time slot for new appointment
        for (int i = 0; i < nodeInv[weekNum][open]; ++i) {
            if (current->appt.getDuration() < node->appt.getDuration()) {
                prev = current;
                current = current->next;
            } else break;
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
    if (appt.getDuration() > maxAvail[weekNum]) setMaxAvail(weekNum, appt.getDuration());
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
    
    else if (current->appt.getStart() == start && current->appt.getDay() == day) {  //head match
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
                if (open && i == nodeInv[weekNum][1] - 1) setMaxAvail(weekNum, prev->appt.getDuration());
                break;
            }
        }
        if (!found) throw "Appointment missing!";
    }
    --nodeInv[weekNum][open];
}
void Resource::printAppts(unsigned int weekNum) {
    if (weekNum > 53) throw "WeekNum too large!";
    
    ApptNode* current = oblig[weekNum][1];
    Date day;
    Time start;
    Time length;
    for (int i = 0; i < nodeInv[weekNum][1]; ++i) {
        day = current->appt.getDay();
        start = current->appt.getStart();
        length = current->appt.getDuration();
        cout << "Appt #" << i + 1 << " > ";
        day.outDate(cout, true);
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
