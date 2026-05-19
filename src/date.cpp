#include "date.h"

#include <sstream>

Date::Date() : day_(1), month_(1), year_(2000) {}

Date::Date(int day, int month, int year) : day_(day), month_(month), year_(year) {}

int Date::day() const {
    return day_;
}

int Date::month() const {
    return month_;
}

int Date::year() const {
    return year_;
}

std::string Date::toString() const {
    std::ostringstream stream;
    stream << day_ << "/" << month_ << "/" << year_;
    return stream.str();
}

bool Date::isLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

bool Date::isValid(int day, int month, int year) {
    const int maxValidYear = 9999;
    const int minValidYear = 1800;

    if (year < minValidYear || year > maxValidYear) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1 || day > 31) {
        return false;
    }

    if (month == 2) {
        return isLeapYear(year) ? day <= 29 : day <= 28;
    }

    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return day <= 30;
    }

    return true;
}
