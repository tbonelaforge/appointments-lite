#ifndef DATE_H
#define DATE_H

enum Weekday { MON, TUE, WED, THU, FRI, SAT, SUN };
enum Month { JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};

class Date {

 private:
    Month month = JAN;
    Weekday wDay = TUE;
    int day = 1;    //day of the month
    int year = 2019;
    int weekNum = 1;    //which week in the year is it? max: 52
    
 public:
    Month getMonth() const { return month; }
    void setMonth(Month m) { month = m; }
    
    Weekday getWDay() const { return wDay; }
    void setWDay(Weekday wd) { wDay = wd; }
    
    int getDay() const { return day; }
    void setDay(unsigned int d) { if (d < 32) day = d; }
    
    int getWeekNum() const { return weekNum; }
    void setWeekNum(unsigned int wn) {if (wn < 53) weekNum = wn; }
    
    int getYear() const { return year; }
    void setYear(unsigned int y) { if (y < 2100) year = y; }
    
    //helper functions
    int findWeekNum(Month, unsigned int, bool alt = false);    //finds weekNum for a given month and day (depends on year)
    Weekday jan1stWeekDay(unsigned int);    //returns Jan 1 weekday for a given year
    Weekday findWeekDay(Month, unsigned int);    //finds weekday (depends on year)
    void findMonthNDay(unsigned int, Weekday, Month &, int &);    //finds month & day (put in last two param) given WeekNum & Weekday
    
    bool operator= (const Date &rhs);
    bool operator== (const Date &rhs); //only compares year, weekNum, and wDay
    bool operator> (const Date &rhs);
    bool operator< (const Date &rhs);
    Date operator+ (const unsigned int &rhs);    //increment Date by days
};

#endif