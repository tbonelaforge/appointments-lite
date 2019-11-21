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
            current->appt.prettyPrint(cout, count);
            count += 1;
            current = current->next;
        }
    }
    cout << endl;
}
