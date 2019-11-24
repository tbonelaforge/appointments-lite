#ifndef DATE_H
#define DATE_H

#include <iostream>

enum Weekday { MON, TUE, WED, THU, FRI, SAT, SUN };
enum Month { JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};


class Date {

 private:
    Month month = JAN;
    Weekday weekday = TUE;
    int day = 1;    //day of the month
    int year = 2019;
    int weekNum = 0;    //which week in the year is it? max: 52 (counting from 0)
    
 public:
    Date() {}
    void initialize(Month m, unsigned int day, unsigned int year);
    Date(Month, unsigned int, unsigned int);  //construct date from month, day, year
    Date(unsigned int, Weekday, unsigned int);  //construct date from weekNum, weekday, year
    
    Month getMonth() const { return month; }
    void setMonth(Month m) { month = m; }
    
    Weekday getWeekday() const { return weekday; }
    void setWeekday(Weekday wd) { weekday = wd; }
    
    int getDay() const { return day; }
    void setDay(unsigned int d) { if (d < 32) day = d; }
    
    int getWeekNum() const { return weekNum; }
    void setWeekNum(unsigned int wn) {if (wn < 53) weekNum = wn; }
    
    int getYear() const { return year; }
    void setYear(int y) { year = y; }

    void addYear(int plus) { year += plus; }
    
    //helper functions
    //finds weekNum for a given month and day (depends on year); alt: dayNum
    int findWeekNum(Month, unsigned int, bool alt = false);
    Weekday jan1stWeekDay(unsigned int);    //returns Jan 1 weekday for a given year
    Weekday findWeekDay(Month, unsigned int);    //finds weekday (depends on year)
    //finds month & day (put in last two param) given WeekNum & Weekday
    void findMonthNDay(unsigned int, Weekday, Month &, int &);
    void outDate(std::ostream &, bool y = false);    // M/D formate; pass std::cout; y = true outputs year
    int daysMonth(Month, int y = 0);  //returns how many days in a given month / year (default year as set)
    
    //only compares year, weekNum, and weekday
    bool operator== (const Date &rhs) { return (year==rhs.year && weekNum==rhs.weekNum && weekday==rhs.weekday); }
    bool operator!= (const Date &rhs) { return (year!=rhs.year || weekNum!=rhs.weekNum || weekday!=rhs.weekday); }
        
    Date operator= (const Date &rhs);
    bool operator> (const Date &rhs);
    bool operator< (const Date &rhs);
    bool operator<= (const Date &rhs);
    Date operator+ (const unsigned int &rhs);    //increment Date by days
    static Month parseMonth(int month);
};

#endif
