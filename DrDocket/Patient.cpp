#include "Patient.h"
#include <iostream>

using namespace std;

void Patient::printAppts() {
	ApptNode* current = nullptr;
    Date day;
    Time start;
    Time length;
    int count = 1;
    for (int i = 0; i < 53; ++i) {
        current = oblig[i][0];
        
        for (int j = 0; j < nodeInv[i][0]; ++j) {
            day = current->appt.getDay();
            start = current->appt.getStart();
            length = current->appt.getDuration();
            cout << "Appt #" << count++ << " > ";
            day.outDate(cout);
            cout << " |-type: " << current->appt.getType();
            cout << " |-start: ";
            start.timeOut(cout);
            cout << " |-length: ";
            length.timeOut(cout);
            cout << endl;
            current = current->next;
        }
    }
    cout << endl;
}
