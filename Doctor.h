#ifndef _DOCTOR_H_
#define _DOCTOR_H_

#include "Person.h"

#include <string>
#include <iostream>

using namespace std;

class Doctor : public Person {
 public:
 Doctor() : Person(), id(0) {};
 Doctor(int id, string firstName, string lastName): Person(firstName, lastName), id(id) {};
    void print(std::ostream& out);
 private:
    int id;
};


#endif
