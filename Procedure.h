#ifndef _PROCEDURE_H_
#define _PROCEDURE_H_

#include <iostream>

using namespace std;

class Procedure {
 public:
 Procedure() : id(0), name(""), length(0) {};
 Procedure(int id, string name, int length) : id(id), name(name), length(length) {};
    string getName();
    void print(std::ostream& out);
 private:
    int id;
    string name;
    int length;
};

#endif
