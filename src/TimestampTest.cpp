//
// Timestamp Tests
// ---------------------------------------------------------------------------
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
#include "hal-common/Timestamp.hpp"

#include "gtest/gtest.h"

#include <ctime>
#include <iostream>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cert-msc32-c"


using lr::DateTime;
using lr::Timestamp32;
using lr::Timestamp64;


/// Test adding seconds using the 32bit Timestamp.
///
TEST(TimestampTest, AddSeconds32)
{
    // Setting the timezone to UTC to make sure `mktime` will convert the `std::tm` as UTC.
    setenv("TZ", "UTC", 1);
    std::tm startDate = {
        .tm_year = (2019 - 1900), // Years are counted from 1900
        .tm_mon = 8 - 1, // Month 0-11
        .tm_mday = 1, // Day 1-31
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0};
    std::tm testEndDate = {
        .tm_year = (2130 - 1900),
        .tm_mon = 8 - 1,
        .tm_mday = 1,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0};
    std::time_t currentTime = std::mktime(&startDate);
    std::tm currentDate = {};
    gmtime_r(&currentTime, &currentDate);
    const std::time_t testEndTime = std::mktime(&testEndDate);
    DateTime dateTime(2019, 8, 1, 0, 0, 0);
    uint32_t stepSize = 1;
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
        auto timestamp = Timestamp32(dateTime);
        ASSERT_EQ(currentTime, (timestamp.getValue() + 0x386D4380u));
        ASSERT_EQ(currentTime, timestamp.toUnixTimestamp());
        timestamp.addSeconds(stepSize);
        dateTime = timestamp.toDateTime();
        currentTime += stepSize;
        stepSize *= 10;
    }
}


/// Test adding seconds using the 64bit Timestamp.
///
TEST(TimestampTest, AddSeconds64)
{
    // Setting the timezone to UTC to make sure `mktime` will convert the `std::tm` as UTC.
    setenv("TZ", "UTC", 1);
    std::tm startDate = {
        .tm_year = (2019 - 1900), // Years are counted from 1900
        .tm_mon = 8 - 1, // Month 0-11
        .tm_mday = 1, // Day 1-31
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0};
    std::tm testEndDate = {
        .tm_year = (3200 - 1900),
        .tm_mon = 8 - 1,
        .tm_mday = 1,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0};
    std::time_t currentTime = std::mktime(&startDate);
    std::tm currentDate = {};
    gmtime_r(&currentTime, &currentDate);
    const std::time_t testEndTime = std::mktime(&testEndDate);
    DateTime dateTime(2019, 8, 1, 0, 0, 0);
    uint32_t stepSize = 1;
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
        auto timestamp = Timestamp64(dateTime);
        ASSERT_EQ(currentTime, (timestamp.getValue() + 0x386D4380ull));
        ASSERT_EQ(currentTime, timestamp.toUnixTimestamp());
        timestamp.addSeconds(stepSize);
        dateTime = timestamp.toDateTime();
        currentTime += stepSize;
        stepSize *= 10;
    }
}

TEST(TimestampTest, AddDays)
{
    DateTime first;
    EXPECT_EQ(true, first.isFirst());
    EXPECT_EQ(2000, first.getYear());
    EXPECT_EQ(1, first.getMonth());
    EXPECT_EQ(1, first.getDay());
    EXPECT_EQ(0, first.getHour());
    EXPECT_EQ(0, first.getMinute());
    EXPECT_EQ(0, first.getSecond());

    Timestamp32 ts32(first);
    ts32.addDays(1);
    DateTime next = ts32.toDateTime();
    EXPECT_EQ(false, next.isFirst());
    EXPECT_EQ(2000, next.getYear());
    EXPECT_EQ(1, next.getMonth());
    EXPECT_EQ(2, next.getDay());
    EXPECT_EQ(0, next.getHour());
    EXPECT_EQ(0, next.getMinute());
    EXPECT_EQ(0, next.getSecond());

    Timestamp64 ts64(first);
    ts64.addDays(1);
    next = ts64.toDateTime();
    EXPECT_EQ(false, next.isFirst());
    EXPECT_EQ(2000, next.getYear());
    EXPECT_EQ(1, next.getMonth());
    EXPECT_EQ(2, next.getDay());
    EXPECT_EQ(0, next.getHour());
    EXPECT_EQ(0, next.getMinute());
    EXPECT_EQ(0, next.getSecond());
}


/// Create a time delta.
///
TEST(TimestampTest, GetSecondDelta)
{
    const DateTime first;
    EXPECT_EQ(true, first.isFirst());
    EXPECT_EQ(2000, first.getYear());
    EXPECT_EQ(1, first.getMonth());
    EXPECT_EQ(1, first.getDay());
    EXPECT_EQ(0, first.getHour());
    EXPECT_EQ(0, first.getMinute());
    EXPECT_EQ(0, first.getSecond());

    const DateTime last(2019, 4, 11, 12, 21, 13);
    const auto delta32 = Timestamp32(first).secondsTo(Timestamp32(last));
    EXPECT_EQ(608300473, delta32);
    const auto delta64 = Timestamp64(first).secondsTo(Timestamp64(last));
    EXPECT_EQ(608300473, delta64);
}


#pragma clang diagnostic pop

