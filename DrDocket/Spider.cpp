#include "Spider.h"
#include <iostream>

using namespace std;

void Spider::removePat(const string nm) {
    int loc = 0;
    bool found = false;
    for (int i = 0; i < pats.size(); ++i ) {
        if (pats.at(i)->getName() == nm) {
            loc = i;
            found = true;
            break;
        }
    }
    if (found) pats.erase(pats.begin() + loc);
}

//find a matchup a time interval of a room resource for a given doctor
bool Spider::findMatchTime(Resource* rm, Opens &opens) {
    int index = 0;
    if (!opens.isGood[0]) index = 0;
    else if (!opens.isGood[1]) index = 1;
    else if (!opens.isGood[2]) index = 2;
    else throw "Bad Opens Record!";
    
    Date d = opens.appt [index] .getDay();
    int weeknum = d.getWeekNum();
    Time dur = opens.appt [index] .getDuration();
    Time start = opens.appt [index] .getStart();
    Time early, earlyDur, late;  //early / late start of avail time; duration of early window
    ApptNode* current;
    bool found = false;
    
    if (rm->maxAvail[weeknum] >= dur) {  //if even enough time in a week
        current = rm->oblig[weeknum][1];
        
        for (int i = 0; i < rm->nodeInv[weeknum][1]; ++i) {  //find day / time / node
            if (current->appt.getDay() == d) {  //day
                if (current->appt.getDuration() >= dur) {  //found enough time
                    if (start < current->appt.getStart()) {  //which window is first
                        early = start;
                        earlyDur = opens.dur;
                        late = current->appt.getStart();
                    } else {
                        early = current->appt.getStart();
                        earlyDur = current->appt.getDuration();
                        late = start;
                    }
                    if (late + dur < early + earlyDur) {  //margins in sync!!
                        opens.isGood[index] = true;
                        opens.nodeNum[1][index] = i;  //save node for if patient agrees; prevent re-searching
                        opens.appt [index] .setStart(late);
                        opens.strands[index] = rm;
                        found = true;
                        ++index;
                        if (index > 2) break;  //if >2 then done finding openings
                        
                        //find more slots if time permits
                        Time startNew = late + dur;
                        Time durNew = earlyDur - (late - early) - dur;
                        if (durNew > dur) { //doctor requires more time
                            if (index == 1) {
                                opens.isGood[1] = true;
                                opens.nodeNum[1][1] = i;
                                opens.appt [1] .setStart( startNew );
                                opens.appt [1] .setDay(d);
                                opens.strands[1] = rm;
                                ++index;
                                startNew = startNew + dur;
                                durNew = durNew - dur;
                                if ( !(durNew > dur) ) continue;  //no more time
                            }
                            if (index == 2) {
                                opens.isGood[2] = true;
                                opens.nodeNum[1][2] = i;
                                opens.appt [2] .setStart( startNew );
                                opens.appt [2] .setDay(d);
                                opens.strands[2] = rm;
                                break;  //done
                            }
                        }
                    }
                }
            }
            current = current->next;
        }
    }
    return found;
}
Opens Spider::findAppts(Resource* doc, Resource* pat, Requirement req, int browse) {
    Opens opens;
    Time dur;
    switch (req) {
        case EXAM: dur.setMn(45); break;
        case BLOOD: dur.setMn(15); break;
        case THEREPY: dur.setHr(1); break;
        case XRAY: dur.setMn(20); break;
        case NIL: throw "Invalid Requirement for Appointment";
    }
    opens.appt[0] = Appointment(doc->getName(), 1, dur, req);
    opens.appt[1] = Appointment(doc->getName(), 1, dur, req);
    opens.appt[2] = Appointment(doc->getName(), 1, dur, req);
    
    //finds 3 opening weeks, minus browse to browse ahead for next openings
    int count = 0 - browse;  //number of found opens
    ApptNode* current = nullptr;
    
    for (int i = 0; i < 53; ++i) {  //first find week
        if (doc->maxAvail[i] > dur) {  //make sure enough time somewhere in week
            current = doc->oblig[i][1];
            
            for (int j = 0; j < doc->nodeInv[i][1]; ++j) {
                if (current->appt.getDuration() > dur) {  //find slot with more time, always add admin for paperwork with Dr's
                   if (count > -1) {  //check for browsing; if so push to different time
                       
                        for (int k = 0; k < resrcs.size(); ++k) {
                            if (resrcs.at(k)->getType() == "Room" && resrcs.at(k) -> matchTag(doc, req)) {
                                opens.appt [count] .setDay( current->appt.getDay() );
                                opens.appt [count] .setStart( current->appt.getStart() );
                                opens.dur = current->appt.getDuration();
                                if (findMatchTime(resrcs.at(k), opens)) {
                                    if (opens.isGood[2]) count = 3;
                                    else if (opens.isGood[1]) count = 2;
                                    else if (opens.isGood[0]) count = 1;
                                    if (count > 2) return opens;
                                }
                            }
                        }
                   }
                   else ++count;
                }
                current = current->next;
                if (count > 2) return opens;
            }
            if (count > 2) return opens;
        }
    }
    return opens;
}
void Spider::printPats() {
    for (int i = 0; i < pats.size(); ++i) {
        cout << " < " << i << " > " << pats.at(i) -> getName() << endl;
    }
    cout << endl;
}
int Spider::printDocs() {
    int count = 0;
    for (int i = 0; i < resrcs.size(); ++i) {
        if (resrcs.at(i)->getType() == "Dr")
            cout << " < " << count++ << " > " << resrcs.at(i) -> getName() << endl;
    }
    cout << endl;
    return count;
}
void Spider::printAppts(Opens opens) {
    for (int i = 0; i < 3; ++i) {
        if (opens.isGood[i]) {
            cout << " < " << i << " > ";
            opens.appt [i] .getDay() .outDate(cout);
            cout << " @ ";
            opens.appt [i] .getStart() .timeOut(cout);
            cout << endl;
        }
    }
    cout << endl;
}
void Spider::printProced(Resource* doc) {
    Requirement rq;
    int count = 0;
    for (int i = 0; i < doc->MAX_QUALT; ++i) {
        rq = doc->getQualTag(i);
        if (rq != NIL) {
            switch (rq) {
                    
             case EXAM:
                cout << " < " << count++ << " > " << "Exam" << endl;
                break;
                    
             case THEREPY:
                cout << " < " << count++ << " > " << "Physiotherepy" << endl;
                break;
                    
             case XRAY:
                cout << " < " << count++ << " > " << "X-Ray" << endl;
                break;
                    
             case BLOOD:
                cout << " < " << count++ << " > " << "Bloodwork" << endl;
                break;
                    
             default:;
            }
        }
    }
    cout << endl;
}
