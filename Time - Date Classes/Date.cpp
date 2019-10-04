/*
----------------------------
Date
----------------------------
- day :  int
- month : Month
- wDay : Weekday
- year :  int
- weekNum :  int
----------------------------
+ getMonth() : Month
+ getMonth() : Month

+ getWDay() : Weekday
+ setWDay(Weekday) : void

+ getYear() : int
+ setYear(unsigned int) : void

+ getWeekNum() : int
+ setWeekNum(unsigned int) : void

+ getDay() : int
+ setDay(unsigned int) : void

+ findWeekNum(Month, unsigned int, bool = false) : int
+ jan1stWeekDay(unsigned int) : Weekday
+ findWeekDay(Month, unsigned int) : Weekday
+ findMonthNDay(unsigned int, Weekday, Month &, int &) : void

+ operator= (Date &) : bool
+ operator== (Date &) : bool
+ operator> (Date &) : bool
+ operator< (Date &) : bool
----------------------------
*/
#include <iostream>
#include "Date.h"

bool Date::operator> (const Date &rhs) {
    if (year > rhs.year) return true;
    if (year == rhs.year && weekNum > rhs.weekNum) return true;
    if (year == rhs.year && weekNum == rhs.weekNum && wDay > rhs.wDay) return true;
    return false;
}
bool Date::operator< (const Date &rhs) {
    if (year < rhs.year) return true;
    if (year == rhs.year && weekNum < rhs.weekNum) return true;
    if (year == rhs.year && weekNum == rhs.weekNum && wDay < rhs.wDay) return true;
    return false;
}
//only compares year, weekNum, and wDay
bool Date::operator== (const Date &rhs) {
    if (year == rhs.year && weekNum == rhs.weekNum && wDay == rhs.wDay) return true;
    return false;
}
//returns Jan 1 weekday for a given year (excluding < 2018)
Weekday Date::jan1stWeekDay(unsigned int y) {
    if (y < 2018) throw "Year parameter too small!";
    int diff = y - 2018;    //most recent year where MON was Jan 1st
    int leap = (diff + 2) / 4;    //2016 was leap, shift of 2 from 2018
    if (!(y % 4)) --leap;    //most recent leap shift only applies after current leap year (jan < feb)
    diff += leap;
    diff = diff % 7;
    return static_cast<Weekday> (diff);
}
//finds weekNum for a given month and day (depends on year)
//alt: returns accumulated days instead 
int Date::findWeekNum(Month m, unsigned int d, bool alt) {
    if (d > 31) throw "Day parameter too large!";
    int dayAcc = 0;    //day accumulator
    
    //add up all months before given
    if (m > JAN) {
        switch (m - 1) {
            case DEC: dayAcc += 31;
            case NOV: dayAcc += 30;
            case OCT: dayAcc += 31;
            case SEP: dayAcc += 30;
            case AUG: dayAcc += 31;
            case JUL: dayAcc += 31;
            case JUN: dayAcc += 30;
            case MAY: dayAcc += 31;
            case APR: dayAcc += 30;
            case MAR: dayAcc += 31;
            case FEB: dayAcc += 28;
            case JAN: dayAcc += 31;
        }
    }
    
    dayAcc += d;    //add given days
    if (!(year % 4) && m > FEB) ++dayAcc;    //adjust for leap year

    if (alt) return dayAcc;
    return (dayAcc + jan1stWeekDay(year) - 1) / 7;    //adjust for year in order to keep MON as start of week
}
//finds weekday (depends on year); uses alt findWeekNum for days accumulated
Weekday Date::findWeekDay(Month m, unsigned int d) {
    int wd = findWeekNum(m, d, true) + jan1stWeekDay(year) - 1;    //days accum + jan1st adjust - 1 for index 0
    wd %= 7;
    return static_cast<Weekday> (wd);
}
//finds month & day (puts in last two param) given WeekNum & Weekday
void Date::findMonthNDay(unsigned int wNum, Weekday wd, Month &m, int &d) {
    if (wNum > 53) throw "Week number too large!";
    int days = (wNum * 7) + wd + 1;    //weekday indexed at 0
    
    //jan-feb optimization
    if (days < 60) {
        if (days > 31) {
            d = days - 31;
            m = FEB;
        } else {
            m = JAN;
            d = days;
        }
        return;
    }
    
    bool leap = (!(year % 4)) ? true : false;    //true = 1
    if (days > 334 + leap) {    //365 (or 366) - 31 ..
        d = days - 334;
        m = DEC;
    } else if (days > 304 + leap) {    //.. - 30
        d = days - 304 + leap;
        m = NOV;
    } else if (days > 273 + leap) {    //.. - 31
        d = days - 273 + leap;
        m = OCT;
    } else if (days > 243 + leap) {    //.. - 30
        d = days - 243 + leap;
        m = SEP;
    } else if (days > 212 + leap) {    //.. - 31
        d = days - 212 + leap;
        m = AUG;
    } else if (days > 181 + leap) {    //.. - 31
        d = days - 181 + leap;
        m = JUL;
    } else if (days > 151 + leap) {    //.. - 30
        d = days - 151 + leap;
        m = JUN;
    } else if (days > 120 + leap) {    //.. - 31
        d = days - 120 + leap;
        m = MAY;
    } else if (days > 90 + leap) {    //.. - 30
        d = days - 90 + leap;
        m = APR;
    } else if (days > 59 + leap) {    //.. - 31
        d = days - 59 + leap;
        m = MAR;
    } else {                          //must be FEB 29
        d = 29;
        m = FEB;
    }
}
