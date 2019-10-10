/*
----------------------------
Time
----------------------------
- hr : int
- mn : int
----------------------------
+ Time(unsigned int = 0, unsigned int = 0)

+ getHr() : int
+ setHr(unsigned int) : void

+ getMn() : int
+ setMn(unsigned int) : void

+ operator= (Time &) : Time
+ operator+ (Time &) : Time
+ operator- (Time &) : Time
+ operator> (Time &) : bool
+ operator< (Time &) : bool
+ operator== (Time &) : bool
+ operator!= (Time &) : bool

+ timeOut(ofstream &) : void
+ timeIn(ifstream &) : void
----------------------------
*/

#include "Time.h"

void Time::setHr(unsigned int h) {
    if (h >= 24) hr = h % 24;
    else hr = h;
}
void Time::setMn(unsigned int m) {
    if (m >= 60) mn = m % 60;
    else mn = m;
}
//expected input: num:num
void Time::timeIn(std::istream &in) {
    int h, m;
    char c;
    in >> h >> c >> m;
    setHr(h);
    setMn(m);
}
bool Time::operator> (const Time &rhs) {
    if (hr > rhs.hr) return true;
    else if (hr == rhs.hr && mn > rhs.mn) return true;
    return false;
}
bool Time::operator>= (const Time &rhs) {
    if (hr > rhs.hr) return true;
    else if (hr == rhs.hr) {
        if (mn > rhs.mn || mn == rhs.mn) return true;
    }
    return false;
}
bool Time::operator< (const Time &rhs) {
    if (hr < rhs.hr) return true;
    if (hr == rhs.hr && mn < rhs.mn) return true;
    return false;
}
Time Time::operator= (const Time &rhs) {
    setHr(rhs.hr);
    setMn(rhs.mn);
    return *this;
}
Time Time::operator+ (const Time &rhs) {
    Time temp;
    unsigned int m;
    m = mn + rhs.mn;
    while (m >= 60) {
        m -= 60;
        ++hr;
    }
    temp.setMn(m);
    temp.setHr(hr + rhs.hr);
    return temp;
}
Time Time::operator- (const Time &rhs) {
    Time temp;
    int m;
    m = mn - rhs.mn;
    while (m < 0) {
        m += 60;
        --hr;
    }
    temp.setMn(m);
    if (hr - rhs.hr < 0) temp.hr = 0;
    else temp.hr = hr = rhs.hr;
    return temp;
}
