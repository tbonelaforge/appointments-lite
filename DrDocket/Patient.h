#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include "Resource.h"

class Patient : public Resource {
 
 private:
    string assign;
    
 public:
    Patient(string n, string a = "") : Resource("Patient", n) { assign = a; }
    
    string getAssign() const { return assign; }
    void setAssign(string a) { assign = a; }
    
    void printAppts();
};

#endif
