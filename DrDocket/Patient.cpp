#include "Patient.h"
#include <iostream>

using namespace std;

void Patient::printAppts() {
	ApptNode* current = nullptr;
    Date today = Date::getCurrentDate();
    int count = 1;
    for (int i = today.getWeekNum(); i < Date::MAX_WEEKS; ++i) {
        current = oblig[i][0];
        
        for (int j = 0; j < nodeInv[i][0]; ++j) {
            current->appt.prettyPrint(cout, count);
            count += 1;
            current = current->next;
            cout << endl;
        }
    }
    cout << endl;
}
