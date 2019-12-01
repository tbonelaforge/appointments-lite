#include <iostream>

#include "Interface.h"

using namespace std;
char enter;


//gets single char input, ignores rest until newline
char Interface::inputChar(char c) {
    cin >> c;
    cin.ignore(999,'\n');
    c = toupper(c);
    return c;
}

void Interface::Menu(char selection) {
    bool valid = false;
    static Date today = Date::getCurrentDate();
    
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
            tell = PRINT_DOCS;
            subMenu = 1;
            break;
                
         //doctor select
         case 1:
            while (!valid) {
                cout << "With which Doctor: >";
				cin >> enter;
				numD=enter-'0';
				enter = toupper(enter);
                cin.ignore(999,'\n');
                if (numD < numDocs && numD > -1) valid = true;
            }
            cout << endl;
            if (numD < 0) subMenu = 0;
            else {
                fetch = DOC;
                subMenu = 2;
            }
            break;
         
         //doctor mode select
         case 2:
            cout << "\t< -- Doctor Mode -- >\n"
                << "< P > - Print Week Schedule\t | Display Appointment Docket\n"
                << "< D > - Doctor Select\t | Change Doctor view\n"
//                << "< C > - Cancel Appt\t | Schedule Appointments\n"
                << "< B > - Back to Mode Select\t | Go back\n" << endl;
            while (!valid) {
               cout << "Enter Option: >";
                selection = inputChar(selection);
                if (selection=='P' || selection=='B' || selection=='D') valid = true;
            }
            cout << endl;
            if (selection == 'B') {
                command = MODE;
                subMenu = 0;
            }
            else if (selection == 'P') {
                num1 = today.getWeekNum();
                cout << "\tWeek #" << num1 << " >" << endl;
                subMenu = 3;
                tell = PRINT_D_APPTS;
            }
            else if (selection == 'D') subMenu = 0;
//            else if (selection == 'C') subMenu = 3;
            break;
        
         //schedule display
         case 3:
            while (!valid) {
                cout << "Would you like to see the next week? (y/n)?";
                selection = inputChar(selection);
                if (selection=='Y' || selection=='N') valid = true;
            }
            cout << endl;
            if (selection == 'N') {
                subMenu = 2;
                num1 = 0;
            }
            else {
                if (num1 < Date::MAX_WEEKS) ++num1;
                cout << "\tWeek #" << num1 << " >" << endl;
                tell = PRINT_D_APPTS;
            }
            break;
        }
        break;
        
     case SECRETARY:
        switch (subMenu) {
         case 0:
            cout << "\t< -- Secretary Mode -- >\n"
                << "< S > - Scheduler\t | Make an Appointment\n"
//                << "< C > - Cancel Appt\t | Schedule Appointments\n"
                << "< P > - Patient Appointments\t | Display Patient Appointments\n"
                << "< B > - Back to Mode Select\t | Go back to Main\n" << endl;
            while (!valid) {
                cout << "Enter Option: >";
                selection = inputChar(selection);
                if (selection=='S' || selection=='B' || selection=='P') valid = true;
            }
            cout << endl;
            if (selection == 'B') command = MODE;
            else if (selection == 'P') {
                subMenu = 5;
                tell = PRINT_PATS;
            }
            else if (selection == 'S') {
                subMenu = 1;
                tell = PRINT_PATS;
            }
            break;
         
         //Scheduler
         case 1:
            while (!valid) {
                cout << "Enter which patient number to schedule: >";
				cin >> enter;
				numP=enter-'0';
				enter = toupper(enter);
                cin.ignore(999,'\n');
                if (numP < numPats && numP > -1) valid = true;
            }
            cout << endl;
            tell = PRINT_DOCS;
            subMenu = 2;
            break;
                
         case 2:
            while (!valid) {
                cout << "(cancel with c)" << endl;
                cout << "With which Doctor: >";
				cin >> enter;
				numD = enter - '0';
				enter = toupper(enter);
                cin.ignore(999,'\n');
                if (numD < numDocs) valid = true;
				if (enter == 'C') valid = true;
            }
			if (enter == 'C') {
				subMenu = 0;
				break;
			}
			else {
				cout << endl;
				tell = PRINT_PROCED;
				fetch = DOC;
				subMenu = 3;
			}
            break;
                
         case 3:
            while (!valid) {
                cout << "(cancel with c)" << endl;
                cout << "Which procedure: >";
				cin >> enter;
				num1 = enter - '0';
				cin.ignore(999,'\n');
                if (num1 < resrc->getNumProced()) valid = true;
				enter = toupper(enter);
				if (enter == 'C') valid = true;
            }
			cout << endl;
			if (enter == 'C') {
				subMenu = 0;
				break;
			}
            else {
                fetch = AVAIL;
                tell = PRINT_AVAIL;
                subMenu = 4;
            }
            break;
                
         case 4:
            while (!valid) {
                cout << "(to browse for different times press n or to cancel press c)" << endl;
                cout << "Which appointment: >";
				cin >> enter;
				num2 = enter - '0';
				enter = toupper(enter);
                cin.ignore(999,'\n');
                if (num2 < 3) valid = true;
				if (enter == 'C' || enter == 'N') valid = true;
            }
            cout << endl;
			if (enter == 'C') {
				subMenu = 0;
				break;
			}
			else if(enter == 'N') {
                numBrowse = (numBrowse > 0) ? ++numBrowse : 1;
                fetch = BROWSE;
                tell = PRINT_AVAIL;
            }
            else {
                cout << "Appointment: " << num2 << " set!\n" << endl;
                numBrowse = 0;
                tell = APPT;
                subMenu = 0;
            }
            break;
                
         //patient appt view
         case 5:
            while (!valid) {
                cout << "Enter which patient number: >";
				cin >> enter;
				numP = enter - '0';
				enter = toupper(enter);
                cin.ignore(999,'\n');
                if (numP < numPats && numP > -1) valid = true;
            }
            cout << endl;
            fetch = PAT;
            tell = PRINT_P_APPTS;
            subMenu = 0;
            break;
                
                
        }
    }
}
