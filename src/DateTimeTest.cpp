//
// (c)2019 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "hal-common/DateTime.hpp"

#include "gtest/gtest.h"

#include <ctime>
#include <iostream>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cert-msc32-c"


using lr::DateTime;


/// Test the state of a date/time object after construction.
///
TEST(DateTimeTest, Construction)
{
    const DateTime first;
    EXPECT_EQ(true, first.isFirst());
    EXPECT_EQ(2000, first.getYear());
    EXPECT_EQ(1, first.getMonth());
    EXPECT_EQ(1, first.getDay());
    EXPECT_EQ(0, first.getHour());
    EXPECT_EQ(0, first.getMinute());
    EXPECT_EQ(0, first.getSecond());

    const DateTime custom(2019, 4, 11, 12, 21, 13);
    EXPECT_EQ(false, custom.isFirst());
    EXPECT_EQ(2019, custom.getYear());
    EXPECT_EQ(4, custom.getMonth());
    EXPECT_EQ(11, custom.getDay());
    EXPECT_EQ(12, custom.getHour());
    EXPECT_EQ(21, custom.getMinute());
    EXPECT_EQ(13, custom.getSecond());
}


/// Test if the values are limited to valid valus.
///
TEST(DateTimeTest, ConstructionLimits)
{
    const DateTime low(0, 0, 0, 0, 0, 0);
    EXPECT_EQ(2000, low.getYear());
    EXPECT_EQ(1, low.getMonth());
    EXPECT_EQ(1, low.getDay());
    EXPECT_EQ(0, low.getHour());
    EXPECT_EQ(0, low.getMinute());
    EXPECT_EQ(0, low.getSecond());

    const DateTime high(0xffffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu);
    EXPECT_EQ(9999, high.getYear());
    EXPECT_EQ(12, high.getMonth());
    EXPECT_EQ(31, high.getDay());
    EXPECT_EQ(23, high.getHour());
    EXPECT_EQ(59, high.getMinute());
    EXPECT_EQ(59, high.getSecond());
}


TEST(DateTimeTest, Assignment)
{
    const DateTime custom1(2019, 4, 11, 12, 21, 13);
    const DateTime custom2(2017, 1, 20, 1, 3, 5);

    DateTime copy(custom1);
    EXPECT_EQ(2019, copy.getYear());
    EXPECT_EQ(4, copy.getMonth());
    EXPECT_EQ(11, copy.getDay());
    EXPECT_EQ(12, copy.getHour());
    EXPECT_EQ(21, copy.getMinute());
    EXPECT_EQ(13, copy.getSecond());
    copy = custom2;
    EXPECT_EQ(2017, copy.getYear());
    EXPECT_EQ(1, copy.getMonth());
    EXPECT_EQ(20, copy.getDay());
    EXPECT_EQ(1, copy.getHour());
    EXPECT_EQ(3, copy.getMinute());
    EXPECT_EQ(5, copy.getSecond());
    copy = custom1;
    EXPECT_EQ(2019, copy.getYear());
    EXPECT_EQ(4, copy.getMonth());
    EXPECT_EQ(11, copy.getDay());
    EXPECT_EQ(12, copy.getHour());
    EXPECT_EQ(21, copy.getMinute());
    EXPECT_EQ(13, copy.getSecond());
    const auto exprResult2 = copy.operator=(custom2);
    EXPECT_EQ(2017, copy.getYear());
    EXPECT_EQ(1, copy.getMonth());
    EXPECT_EQ(20, copy.getDay());
    EXPECT_EQ(1, copy.getHour());
    EXPECT_EQ(3, copy.getMinute());
    EXPECT_EQ(5, copy.getSecond());
    EXPECT_EQ(2017, exprResult2.getYear());
    EXPECT_EQ(1, exprResult2.getMonth());
    EXPECT_EQ(20, exprResult2.getDay());
    EXPECT_EQ(1, exprResult2.getHour());
    EXPECT_EQ(3, exprResult2.getMinute());
    EXPECT_EQ(5, exprResult2.getSecond());
    const auto exprResult1 = copy.operator=(custom1);
    EXPECT_EQ(2019, copy.getYear());
    EXPECT_EQ(4, copy.getMonth());
    EXPECT_EQ(11, copy.getDay());
    EXPECT_EQ(12, copy.getHour());
    EXPECT_EQ(21, copy.getMinute());
    EXPECT_EQ(13, copy.getSecond());
    EXPECT_EQ(2019, exprResult1.getYear());
    EXPECT_EQ(4, exprResult1.getMonth());
    EXPECT_EQ(11, exprResult1.getDay());
    EXPECT_EQ(12, exprResult1.getHour());
    EXPECT_EQ(21, exprResult1.getMinute());
    EXPECT_EQ(13, exprResult1.getSecond());
}


/// Test the comparison operators.
///
TEST(DateTimeTest, Comparison)
{
    const DateTime custom1(2019, 4, 11, 12, 21, 13);
    const DateTime custom2(2020, 4, 11, 12, 21, 13);
    const DateTime custom3(2019, 5, 11, 12, 21, 13);
    const DateTime custom4(2019, 4, 12, 12, 21, 13);
    const DateTime custom5(2019, 4, 11, 13, 21, 13);
    const DateTime custom6(2019, 4, 11, 12, 22, 13);
    const DateTime custom7(2019, 4, 11, 12, 21, 14);

    const DateTime copy1 = custom1;

    EXPECT_EQ(true, custom1 == custom1);
    EXPECT_EQ(true, custom2 == custom2);
    EXPECT_EQ(true, custom1 == copy1);
    EXPECT_EQ(false, custom1 == custom2);
    EXPECT_EQ(false, custom1 == custom3);
    EXPECT_EQ(false, custom1 == custom4);
    EXPECT_EQ(false, custom1 == custom5);
    EXPECT_EQ(false, custom1 == custom6);
    EXPECT_EQ(false, custom1 == custom7);

    EXPECT_EQ(false, custom1 != custom1);
    EXPECT_EQ(false, custom2 != custom2);
    EXPECT_EQ(false, custom1 != copy1);
    EXPECT_EQ(true, custom1 != custom2);
    EXPECT_EQ(true, custom1 != custom3);
    EXPECT_EQ(true, custom1 != custom5);
    EXPECT_EQ(true, custom1 != custom6);
    EXPECT_EQ(true, custom1 != custom7);
    EXPECT_EQ(true, custom1 != custom4);

    EXPECT_EQ(false, custom1 < custom1);
    EXPECT_EQ(false, custom2 < custom2);
    EXPECT_EQ(false, custom1 < copy1);
    EXPECT_EQ(true, custom1 < custom2);
    EXPECT_EQ(true, custom1 < custom3);
    EXPECT_EQ(true, custom1 < custom5);
    EXPECT_EQ(true, custom1 < custom6);
    EXPECT_EQ(true, custom1 < custom7);
    EXPECT_EQ(true, custom1 < custom4);

    EXPECT_EQ(true, custom1 <= custom1);
    EXPECT_EQ(true, custom2 <= custom2);
    EXPECT_EQ(true, custom1 <= copy1);
    EXPECT_EQ(true, custom1 <= custom2);
    EXPECT_EQ(true, custom1 <= custom3);
    EXPECT_EQ(true, custom1 <= custom5);
    EXPECT_EQ(true, custom1 <= custom6);
    EXPECT_EQ(true, custom1 <= custom7);
    EXPECT_EQ(true, custom1 <= custom4);

    EXPECT_EQ(false, custom1 > custom1);
    EXPECT_EQ(false, custom2 > custom2);
    EXPECT_EQ(false, custom1 > copy1);
    EXPECT_EQ(false, custom1 > custom2);
    EXPECT_EQ(false, custom1 > custom3);
    EXPECT_EQ(false, custom1 > custom5);
    EXPECT_EQ(false, custom1 > custom6);
    EXPECT_EQ(false, custom1 > custom7);
    EXPECT_EQ(false, custom1 > custom4);

    EXPECT_EQ(true, custom1 >= custom1);
    EXPECT_EQ(true, custom2 >= custom2);
    EXPECT_EQ(true, custom1 >= copy1);
    EXPECT_EQ(false, custom1 >= custom2);
    EXPECT_EQ(false, custom1 >= custom3);
    EXPECT_EQ(false, custom1 >= custom5);
    EXPECT_EQ(false, custom1 >= custom6);
    EXPECT_EQ(false, custom1 >= custom7);
    EXPECT_EQ(false, custom1 >= custom4);
}


/// Test if the setter work as expected.
///
TEST(DateTimeTest, Setter)
{
    DateTime custom(2019, 4, 11, 12, 21, 13);
    EXPECT_EQ(2019, custom.getYear());
    EXPECT_EQ(4, custom.getMonth());
    EXPECT_EQ(11, custom.getDay());
    EXPECT_EQ(12, custom.getHour());
    EXPECT_EQ(21, custom.getMinute());
    EXPECT_EQ(13, custom.getSecond());

    custom.setDate(2020, 1, 1);
    EXPECT_EQ(2020, custom.getYear());
    EXPECT_EQ(1, custom.getMonth());
    EXPECT_EQ(1, custom.getDay());
    EXPECT_EQ(12, custom.getHour());
    EXPECT_EQ(21, custom.getMinute());
    EXPECT_EQ(13, custom.getSecond());

    custom.setDate(0xffffu, 0xff, 0xff);
    EXPECT_EQ(9999, custom.getYear());
    EXPECT_EQ(12, custom.getMonth());
    EXPECT_EQ(31, custom.getDay());
    EXPECT_EQ(12, custom.getHour());
    EXPECT_EQ(21, custom.getMinute());
    EXPECT_EQ(13, custom.getSecond());

    custom.setDate(0, 0, 0);
    EXPECT_EQ(2000, custom.getYear());
    EXPECT_EQ(1, custom.getMonth());
    EXPECT_EQ(1, custom.getDay());
    EXPECT_EQ(12, custom.getHour());
    EXPECT_EQ(21, custom.getMinute());
    EXPECT_EQ(13, custom.getSecond());

    custom.setTime(21, 13, 14);
    EXPECT_EQ(2000, custom.getYear());
    EXPECT_EQ(1, custom.getMonth());
    EXPECT_EQ(1, custom.getDay());
    EXPECT_EQ(21, custom.getHour());
    EXPECT_EQ(13, custom.getMinute());
    EXPECT_EQ(14, custom.getSecond());

    custom.setTime(0xffu, 0xffu, 0xffu);
    EXPECT_EQ(2000, custom.getYear());
    EXPECT_EQ(1, custom.getMonth());
    EXPECT_EQ(1, custom.getDay());
    EXPECT_EQ(23, custom.getHour());
    EXPECT_EQ(59, custom.getMinute());
    EXPECT_EQ(59, custom.getSecond());

    custom.setTime(0, 0, 0);
    EXPECT_EQ(2000, custom.getYear());
    EXPECT_EQ(1, custom.getMonth());
    EXPECT_EQ(1, custom.getDay());
    EXPECT_EQ(0, custom.getHour());
    EXPECT_EQ(0, custom.getMinute());
    EXPECT_EQ(0, custom.getSecond());
}


/// Test if the day of week calculation is correct.
/// Test it against the C++ standard library.
/// @note I wish C++ had finally a set of date/time classes. :-(
///
TEST(DateTimeTest, DayOfWeek)
{
    static_assert(sizeof(std::time_t) >= 8, "This test requires 64bit time_t.");
    // Setting the timezone to UTC to make sure `mktime` will convert the `std::tm` as UTC.
    setenv("TZ", "UTC", 1);
    std::tm startDate = {
        .tm_year = (2000-1900), // Years are counted from 1900
        .tm_mon = 0, // Month 0-11
        .tm_mday = 1, // Day 1-31
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0
    };
    std::tm testEndDate = {
        .tm_year = 4000-1900,
        .tm_mon = 0,
        .tm_mday = 1,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0
    };
    std::time_t currentTime = std::mktime(&startDate);
    ASSERT_EQ((2000-1900), startDate.tm_year);
    ASSERT_EQ(0, startDate.tm_mon);
    ASSERT_EQ(1, startDate.tm_mday);
    ASSERT_EQ(0, startDate.tm_hour);
    ASSERT_EQ(0, startDate.tm_min);
    ASSERT_EQ(0, startDate.tm_sec);
    ASSERT_EQ(6, startDate.tm_wday);
    std::tm currentDate = {};
    // Sadly test the std implementation first.
    // If `mktime` uses the wrong (non UTC) timezone, the following
    // asserts will fail.
    gmtime_r(&currentTime, &currentDate);
    ASSERT_EQ((2000-1900), currentDate.tm_year);
    ASSERT_EQ(0, currentDate.tm_mon);
    ASSERT_EQ(1, currentDate.tm_mday);
    ASSERT_EQ(0, currentDate.tm_hour);
    ASSERT_EQ(0, currentDate.tm_min);
    ASSERT_EQ(0, currentDate.tm_sec);
    ASSERT_EQ(6, currentDate.tm_wday);
    const std::time_t one_day = 60*60*24;
    const std::time_t testEndTime = std::mktime(&testEndDate);
    while (currentTime < testEndTime) {
        gmtime_r(&currentTime, &currentDate);
        const uint16_t year = currentDate.tm_year + 1900;
        const uint8_t month = currentDate.tm_mon + 1;
        const uint8_t day = currentDate.tm_mday;

        const uint8_t expectedDayOfWeek = currentDate.tm_wday; // 0 = sunday

        DateTime dateTime(year, month, day);
        const uint8_t actualDayOfWeek = dateTime.getDayOfWeek();

        if (expectedDayOfWeek != actualDayOfWeek) {
            std::cout << "Failed Date: " << year << "-" << static_cast<int>(month) << "-" << static_cast<int>(day) << " Expected: " << static_cast<int>(expectedDayOfWeek)
                      << " Got: " << static_cast<int>(actualDayOfWeek) << std::endl;
        }
        ASSERT_EQ(static_cast<int>(expectedDayOfWeek), static_cast<int>(actualDayOfWeek));

        currentTime += one_day;
    }
}


/// Test counting seconds.
///
TEST(DateTimeTest, CountSeconds)
{
    // Setting the timezone to UTC to make sure `mktime` will convert the `std::tm` as UTC.
    setenv("TZ", "UTC", 1);
    std::tm startDate = {
        .tm_year = (2019-1900), // Years are counted from 1900
        .tm_mon = 8-1, // Month 0-11
        .tm_mday = 1, // Day 1-31
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0
    };
    std::tm testEndDate = {
        .tm_year = (2020-1900),
        .tm_mon = 8-1,
        .tm_mday = 1,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0
    };
    std::time_t currentTime = std::mktime(&startDate);
    std::tm currentDate = {};
    gmtime_r(&currentTime, &currentDate);
    const std::time_t testEndTime = std::mktime(&testEndDate);
    DateTime dateTime(2019, 8, 1, 0, 0, 0);
    while (currentTime < testEndTime) {
        gmtime_r(&currentTime, &currentDate);
        const uint16_t year = currentDate.tm_year + 1900;
        const uint8_t month = currentDate.tm_mon + 1;
        const uint8_t day = currentDate.tm_mday;
        const uint8_t hour = currentDate.tm_hour;
        const uint8_t minute = currentDate.tm_min;
        const uint8_t second = currentDate.tm_sec;
        ASSERT_EQ(year, dateTime.getYear());
        ASSERT_EQ(month, dateTime.getMonth());
        ASSERT_EQ(day, dateTime.getDay());
        ASSERT_EQ(hour, dateTime.getHour());
        ASSERT_EQ(minute, dateTime.getMinute());
        ASSERT_EQ(second, dateTime.getSecond());
        const int stepSize = 27;
        currentTime += stepSize;
        for (int i = 0; i < stepSize; ++i) {
            dateTime.addOneSecond();
        }
    }
}


/// Test string conversions.
///
TEST(DateTimeTest, StringConversion)
{
    struct TestValue {
        DateTime::Format format;
        const char* text;
    };
    TestValue testValues[] = {
        TestValue({DateTime::Format::ISO, "2019-04-11T12:21:13"}),
        TestValue({DateTime::Format::Long, "2019-04-11 12:21:13"}),
        TestValue({DateTime::Format::ISODate, "2019-04-11"}),
        TestValue({DateTime::Format::ISOBasicDate, "20190411"}),
        TestValue({DateTime::Format::ISOTime, "12:21:13"}),
        TestValue({DateTime::Format::ISOBasicTime, "122113"}),
        TestValue({DateTime::Format::ShortDate, "11.04."}),
        TestValue({DateTime::Format::ShortTime, "12:21"}),
    };

    const DateTime custom(2019, 4, 11, 12, 21, 13);
    for (const auto &testValue : testValues) {
        auto text = custom.toString(testValue.format);
        const auto result = std::strcmp(text.getData(), testValue.text);
        ASSERT_EQ(0, result);
    }
}


#pragma clang diagnostic pop
