#ifndef _PERSON_H_
#define _PERSON_H_

#include <string>
#include <iostream>

using namespace std;

class Person {
 public:
    Person(): firstName(""), lastName("") {};
    Person(string firstName, string lastName): firstName(firstName), lastName(lastName) {};
    string getFullName();
 private:
    string firstName;
    string lastName;
};

#endif
