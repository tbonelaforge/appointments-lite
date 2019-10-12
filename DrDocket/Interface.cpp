#include <iostream>

#include "Interface.h"

using namespace std;

//gets single char input, ignores rest until newline
char Interface::inputChar(char c) {
    cin >> c;
    cin.ignore(999,'\n');
    c = toupper(c);
    return c;
}

void Interface::Menu(char selection) {
    bool valid = false;
    
    switch(selection) {
        
     //mode select, first menu
     case MODE:
        cout << "\t< -- Mode Select -- >\n"
            << "< S > - Secretary\t | Schedule Appointments\n"
            << "< D > - Doctor\t | Display Appointment Docket\n"
            << "< Q > - Quit\t | Leave DrDocket\n" << endl;
        while (!valid) {
            cout << "Enter Mode: >";
            selection = inputChar(selection);
            if (selection==SECRETARY || selection==DOCTOR || selection==QUIT) valid = true;
        }
        cout << endl;
        command = selection;
        break;
            
     case DOCTOR:
        switch (subMenu) {
         case 0:
            cout << "\t< -- Doctor Mode -- >\n"
                << "< P > - Print Week Schedule\t | Display Appointment Docket\n"
                << "< C > - Cancel Appt\t | Schedule Appointments\n"
                << "< B > - Back to Mode Select\t | Display Appointment Docket\n" << endl;
            while (!valid) {
               cout << "Enter Option: >";
                selection = inputChar(selection);
                if (selection=='P' || selection=='B') valid = true;
            }
            cout << endl;
            if (selection == 'B') command = MODE;
            else if (selection == 'P') subMenu = 1;
//            else if (selection == 'C') subMenu = 3;
            break;
         
         //Display Week Schedule  
         case 1:
            while (!valid) {
                cout << "Enter week number to see schedule: >";
                cin >> num1;
                cin.ignore(999,'\n');
                if (num1 < 53 && num1 > -1) valid = true;
            }
            cout << endl;
            fetch = ELEMENT;
            subMenu = 2;
            break;
                
         case 2:
            resrc->printAppts(num1);
            while (!valid) {
                cout << "Would you like to see another week? (y/n)?";
                selection = inputChar(selection);
                if (selection=='Y' || selection=='N') valid = true;
            }
            cout << endl;
            if (selection == 'N') subMenu = 0;
            else subMenu = 1;
            break;
        }
        break;
        
     case SECRETARY:
        switch (subMenu) {
         case 0:
            cout << "\t< -- Secretary Mode -- >\n"
                << "< S > - Scheduler\t | Make an Appointment\n"
                << "< C > - Cancel Appt\t | Schedule Appointments\n"
                << "< B > - Back to Mode Select\t | Display Appointment Docket\n" << endl;
            while (!valid) {
                cout << "Enter Option: >";
                selection = inputChar(selection);
                if (selection=='S' || selection=='B') valid = true;
            }
            cout << endl;
            if (selection == 'B') command = MODE;
            else if (selection == 'S') {
                subMenu = 1;
                tell = PRINT_PATS;
            }
            break;
         
         //Scheduler
         case 1:
            while (!valid) {
                cout << "Enter which patient number to schedule: >";
                cin >> numP;
                cin.ignore(999,'\n');
                if (numP < numPats && numP > -1) valid = true;
            }
            cout << endl;
            tell = PRINT_DOCS;
            subMenu = 2;
            break;
                
         case 2:
            while (!valid) {
                cout << "(cancel with -1)" << endl;
                cout << "With which Doctor: >";
                cin >> numD;
                cin.ignore(999,'\n');
                if (numD < numDocs) valid = true;
            }
            cout << endl;
            if (numD < 0) subMenu = 0;
            else {
                tell = PRINT_PROCED;
                fetch = DOC;
                subMenu = 3;
            }
            break;
                
         case 3:
            while (!valid) {
                cout << "(cancel with -1)" << endl;
                cout << "Which procedure: >";
                cin >> num1;
                cin.ignore(999,'\n');
                if (num1 < resrc->getNumProced()) valid = true;
            }
            cout << endl;
            if (num1 < 0) subMenu = 0;
            else {
                fetch = APPT;
                tell = PRINT_APPTS;
                subMenu = 4;
            }
            break;
                
         case 4:
            while (!valid) {
                cout << "(or browse for something else with -1 or -2...)" << endl;
                cout << "Which appointment: >";
                cin >> num2;
                cin.ignore(999,'\n');
                if (num2 < 3) valid = true;
            }
            cout << endl;
            if (num2 < 0) {
                num2 = abs(num2);
                fetch = BROWSE;
                tell = PRINT_APPTS;
            }
            else {
                tell = APPT;
            }
            break;
                
                
                
        }
    }
}
