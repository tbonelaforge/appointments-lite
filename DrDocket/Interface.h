#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include "Resource.h"
#include "Spider.h"

const char MODE = 'M', DOCTOR = 'D', QUIT = 'Q', SECRETARY = 'S';  //user commands
enum Code { BLANK, ELEMENT, PRINT_PATS, PRINT_DOCS, APPT, PRINT_APPTS, BROWSE, PRINT_PROCED, DOC };  //internal codes

//displays menus and gets commands also treated as a record of store between user and backend classes
struct Interface {
    void Menu(char);
    char inputChar(char);
    
    int num1, num2;
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
    Opens opens;
    ApptNode* apptNode = nullptr;
    Availability avail;
    Requirement req;
};
#endif
