#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include "Resource.h"
#include "Spider.h"

const char MODE = 'M', DOCTOR = 'D', QUIT = 'Q', SECRETARY = 'S';  //user commands
enum Code { BLANK, PRINT_PATS, PRINT_DOCS, PAT, PRINT_P_APPTS,
    PRINT_AVAIL, BROWSE, PRINT_PROCED, DOC, APPT, AVAIL, PRINT_D_APPTS };  //internal codes

//displays menus and gets commands also treated as a record of store between user and backend classes
struct Interface {
    void Menu(char);
    char inputChar(char);
    
    int num1, num2;
    int numBrowse = 0;
    int numPats, numDocs;  //number of Patients, of Doctors
    int numD, numP;  //number to use when dealing with doctors; ... patients
    int subMenu = 0;
    char command = BLANK;
    Code fetch = BLANK;
    Code tell = BLANK;
    Date date;
    Time time;
    std::string str;
    
    Resource* resrc = nullptr;
    Patient* pat = nullptr;
    Opens opens;
    ApptNode* apptNode = nullptr;
    Availability avail;
    Requirement req;
};
#endif
