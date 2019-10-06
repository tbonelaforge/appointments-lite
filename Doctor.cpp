
#include <iostream>

#include "Doctor.h"

void Doctor::print(std::ostream& out) {
    out << "Doctor: " << getFullName() << " (id = " << id << ")";
}
