
#include <iostream>

#include "Procedure.h"

string Procedure::getName() {
    return name;
}

int Procedure::getId() {
    return id;
}

void Procedure::print(std::ostream& out) {
    out << "Procedure: " 
        << name << ", Length: " 
        << length << " min, "
        << "(id = " << id << ")";
}
