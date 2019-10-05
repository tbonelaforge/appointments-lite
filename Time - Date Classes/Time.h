#ifndef TIME_H
#define TIME_H

#include <fstream>

class Time {
    
 private:
    int hr;
    int mn;

 public:
    Time(unsigned int h = 0, unsigned int m = 0) { hr = h; mn = m; }
    
    int getHr() const { return hr; }
    void setHr(unsigned int h);    //checks range
    
    int getMn() const { return mn; }
    void setMn(unsigned int m);    //checks range
    
    Time operator= (const Time &rhs);    //rhs: right hand side; ex: var = rhs
    Time operator+ (const Time &rhs);    //overflow hours are reset
    Time operator- (const Time &rhs);    //underlow hours are set to 0
    bool operator== (const Time &rhs) { return (hr == rhs.hr && mn == rhs.mn); }
    bool operator> (const Time &rhs);
    bool operator< (const Time &rhs);
    
    void timeOut(std::ofstream &out) { out << hr << ":" << mn; }
    void timeIn(std::ifstream &in);
};

#endif