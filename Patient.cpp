
#include <iostream>

#include "Patient.h"


void Patient::print(std::ostream& out) {
    out << "Patient: " << getFullName() << " (id = " << id << ")";
}
