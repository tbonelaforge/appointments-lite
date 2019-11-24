/*
----------------------------
Date
----------------------------
- day :  int
- month : Month
- weekday : Weekday
- year :  int
- weekNum :  int
----------------------------
+ Date()
+ Date(Month, unsigned int, unsigned int)
+ Date(unsigned int, Weekday, unsigned int)

+ getMonth() : Month
+ setMonth(Month) : void

+ getWeekday() : Weekday
+ setWeekday(Weekday) : void

+ getYear() : int
+ addYear(int) : void

+ getWeekNum() : int
+ setWeekNum(unsigned int) : void

+ getDay() : int
+ setDay(unsigned int) : void

+ findWeekNum(Month, unsigned int, bool = false) : int
+ jan1stWeekDay(unsigned int) : Weekday
+ findWeekDay(Month, unsigned int) : Weekday
+ findMonthNDay(unsigned int, Weekday, Month &, int &) : void
+ outDate(std::ostream &, bool = false) : void
+ daysMonth(Month, int = 0) : int

+ operator= (Date &) : Date
+ operator== (Date &) : bool
+ operator!= (Date &) : bool
+ operator> (Date &) : bool
+ operator< (Date &) : bool
+ operator+ (int &) : Date
----------------------------
*/

#include <iostream>
#include "Date.h"

Month Months[13] = {JAN, JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};

void Date::initialize(Month m, unsigned int d, unsigned int y) {
    month = m;
    day = (d < 32) ? d : 1;
    year = (y < 2100) ? y : 2019;
    weekNum = findWeekNum(m, day);
    weekday = findWeekDay(m, day);
}

Date::Date(Month m, unsigned int d, unsigned int y) {
    this->initialize(m, d, y);
}

Date::Date(unsigned int wn, Weekday wd, unsigned int y) {
    weekNum = (wn < 53) ? wn : 0;
    weekday = wd;
    year = (y < 2100) ? y : 2019;
    Month m;
    int d;
    findMonthNDay(weekNum, wd, m, d);
    month = m;
    day = d;
}
//returns how many days in a given month / year (default year as set)
int Date::daysMonth(Month m, int y) {
    if (!y) y = year;
    switch (m) {
        case DEC: return 31;
        case NOV: return 30;
        case OCT: return 31;
        case SEP: return 30;
        case AUG: return 31;
        case JUL: return 31;
        case JUN: return 30;
        case MAY: return 31;
        case APR: return 30;
        case MAR: return 31;
        case FEB: return 28 + (y % 4) ? 0 : 1;
        case JAN: return 31;
    }
}
Date Date::operator= (const Date &rhs) {
    month = rhs.month;
    weekday = rhs.weekday;
    year = rhs.year;
    setDay(rhs.day);
    setWeekNum(rhs.weekNum);
    return *this;
}
//increment Date by days
Date Date::operator+ (const unsigned int &rhs) {
    if (rhs > 366) throw "Added Date increment to large!";
    
    int diffWeekday = rhs % 7;
    int diffWeeknum = rhs / 7;
    if (diffWeeknum + weekNum > 52) {
        ++year;
        weekNum = (diffWeeknum + weekNum) - 52;
    }
    else weekNum += diffWeeknum;
    weekday = static_cast<Weekday> (diffWeekday);
    findMonthNDay(weekNum, weekday, month, day);
    return *this;
}
bool Date::operator> (const Date &rhs) {
    if (year > rhs.year) return true;
    if (year == rhs.year && weekNum > rhs.weekNum) return true;
    if (year == rhs.year && weekNum == rhs.weekNum && weekday > rhs.weekday) return true;
    return false;
}
bool Date::operator< (const Date &rhs) {
    if (year < rhs.year) return true;
    if (year == rhs.year && weekNum < rhs.weekNum) return true;
    if (year == rhs.year && weekNum == rhs.weekNum && weekday < rhs.weekday) return true;
    return false;
}
bool  Date::operator<= (const Date &rhs) {
    if (year < rhs.year) return true;
    if (year == rhs.year && weekNum < rhs.weekNum) return true;
    if (year == rhs.year && weekNum == rhs.weekNum)
        if (weekday < rhs.weekday || weekday == rhs.weekday) return true;
    return false;
}
//y:true output year; pass std::cout as first param
void Date::outDate(std::ostream &out, bool y) {
    out << month + 1 << "/" << day;
    if (y) out << "/" << year;
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
    
    int days = (wNum * 7) + wd + 1 - jan1stWeekDay(year);    //weekday indexed at 0 and adjust for year start weekday
    if (days < 1) throw "Weekday wrong!";
    
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
    
    int leap = (year % 4) ? 0 : 1;    //true = 1+, false = 0
    if (days > 334 + leap) {           //365 (or 366) - 31 ..
        d = days - 334 + leap;
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


Month Date::parseMonth(int month) {
    return Months[month];
}