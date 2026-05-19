#ifndef DATE_H
#define DATE_H

#include <string>

class Date {
public:
    Date();
    Date(int day, int month, int year);

    int day() const;
    int month() const;
    int year() const;

    std::string toString() const;

    static bool isLeapYear(int year);
    static bool isValid(int day, int month, int year);

private:
    int day_;
    int month_;
    int year_;
};

#endif
