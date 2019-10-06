
#include <iostream>

#include "Procedure.h"

string Procedure::getName() {
    return name;
}

void Procedure::print(std::ostream& out) {
    out << "Procedure: " 
        << name << ", Length: " 
        << length << " min, "
        << "(id = " << id << ")";
}
