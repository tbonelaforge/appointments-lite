#include <fstream>
#include <iostream>
#include <string>

#include "Interface.h"
#include "Resource.h"
#include "Spider.h"
#include "Patient.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    cout << "Welcome to DrDocket!" << endl;
    
    Spider spdr;
    string docs[4] {"Johnson", "Reyes", "Swinburne", "Bradshaw"};
    for (int i = 0; i < 4; ++i) {
        Resource* d = new Resource("Dr", docs[i]);
        d->setQualTag(0, EXAM);
        spdr.addResrc(d);
    }
    spdr.setResrc(0)->setQualTag(1, XRAY);
    spdr.setResrc(1)->setQualTag(1, XRAY);
    spdr.setResrc(2)->setQualTag(1, BLOOD);
    spdr.setResrc(3)->setQualTag(1, THEREPY);

    Availability nonLaborAvail;
    nonLaborAvail.setAvail(true, 1, 22);
    nonLaborAvail.setAvail(true, 0, 7, 'D');
    nonLaborAvail.setAvail(true, 0, 12, 'M');
    string rooms[5] {"Exam A", "Exam B", "Lab", "X-ray", "Physiotherapy"};
    for (int i = 0; i < 5; ++i) {
        Resource* r = new Resource("Room", rooms[i], nonLaborAvail);
        spdr.addResrc(r);
    }
    spdr.setResrc(4)->setQualTag(0, EXAM);
    spdr.setResrc(5)->setQualTag(0, EXAM);
    spdr.setResrc(6)->setQualTag(0, BLOOD);
    spdr.setResrc(7)->setQualTag(0, THEREPY);
    spdr.setResrc(8)->setQualTag(0, XRAY);

    string intake[3] {"Fay Zhong", "Shane Hightower", "Brad Bradford"};
    for (int i = 0; i < 3; ++i) {
        Patient* p = new Patient(intake[i]);
        spdr.addPat(p);
    }
    
    Interface handler;
    handler.command = MODE;    //command for operating the interface. starts at 'M' for Mode select menu
    
    //menu gets input from user, then loops to fetch access from spider or tell spider to perfom something
    //think of main as back end, and interface.cpp as front end;
    while (handler.command != QUIT) {
        handler.numPats = spdr.getNumPats();
        handler.Menu(handler.command);
        
        //fetches access
        switch (handler.fetch) {
                
         case ELEMENT: handler.resrc = spdr.setResrc(0); break;
        
         case DOC:
            for (int i = 0; i < spdr.getNumResrcs(); ++i) {
                if (spdr.setResrc(i)->getType() == "Dr")
                    if ( i == handler.numD) handler.resrc = spdr.setResrc(i);
            }
            break;
                
         case AVAIL:
                
            //fetches requirement
            for (int i = 0; i < handler.resrc->MAX_QUALT; ++i) {
                if (spdr.setResrc(handler.numD)->getQualTag(i) != NIL)
                    if ( i == handler.num1) handler.req = spdr.setResrc(handler.numD)->getQualTag(i);
            }
                
            //fetches openings
            handler.opens = spdr.findAppts(handler.resrc, spdr.setResrc(handler.numP), handler.req);
            break;
         
         case BROWSE:
    
            //fetches different openings
            handler.opens = spdr.findAppts(handler.resrc, spdr.setResrc(handler.numP), handler.req, handler.num2);
            break;
                
         default:;
        }
        handler.fetch = BLANK;  //clear request
        
        //tells spider what to do
        switch (handler.tell) {
                
         case PRINT_PATS: spdr.printPats(); break;
         case PRINT_DOCS: handler.numDocs = spdr.printDocs(); break;
         case PRINT_PROCED: spdr.printProced(handler.resrc); break;
         case PRINT_AVAIL: spdr.printAvails(handler.opens); break;
         case APPT: spdr.convertToCommit(handler.resrc, spdr.setResrc(handler.numP), handler.opens, handler.num2); break;
                
         default:;
        }
        handler.tell = BLANK;  //clear request
    }
    
    cout << "Bye!" << endl;
    
    return 0;
}
