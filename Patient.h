#ifndef _PATIENT_H_
#define _PATIENT_H_

#include "Person.h"

#include <string>
#include <iostream>

using namespace std;

class Patient : public Person {
 public:
    Patient() : Person(), id(0) {};
    Patient(int id, string firstName, string lastName): Person(firstName, lastName), id(id) {};
    void print(std::ostream& out);
 private:
    int id;
};

#endif
