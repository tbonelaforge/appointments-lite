
#include <iostream>

#include "Person.h"

string Person::getFullName() {
    return lastName + ", " + firstName;
}
