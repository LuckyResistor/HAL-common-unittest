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
#include "hal-common/IntegerMath.hpp"

#include "gtest/gtest.h"

#include <random>


using namespace lr;


template<typename IntType>
void ringIncrementTest(const IntType size)
{
    const IntType last = size - 1;
    const IntType overflow = size + 1;
    IntType value = 0u;
    IntegerMath::ringIncrement<IntType>(value, 0, size);
    EXPECT_EQ(value, 0);
    IntegerMath::ringIncrement<IntType>(value, last, size);
    EXPECT_EQ(value, last);
    IntegerMath::ringIncrement<IntType>(value, 1, size);
    EXPECT_EQ(value, 0);
    IntegerMath::ringIncrement<IntType>(value, overflow, size);
    EXPECT_EQ(value, 1);
}


TEST(IntegerMathTest, RingIncrement)
{
    ringIncrementTest<uint8_t>(10u);
    ringIncrementTest<uint8_t>(0x7fu);
    ringIncrementTest<uint8_t>(0xfeu);
    ringIncrementTest<uint16_t>(10u);
    ringIncrementTest<uint16_t>(0x7ffeu);
    ringIncrementTest<uint16_t>(0xfffeu);
    ringIncrementTest<uint32_t>(10u);
    ringIncrementTest<uint32_t>(0x7ffffffeu);
    ringIncrementTest<uint32_t>(0xfffffffeu);
    ringIncrementTest<uint64_t>(10u);
}


TEST(IntegerMathTest, MinMax)
{
    EXPECT_EQ(IntegerMath::min(40, 20), 20);
    EXPECT_EQ(IntegerMath::min(20, 20), 20);
    EXPECT_EQ(IntegerMath::min(20, 40), 20);
    EXPECT_EQ(IntegerMath::min(0, 40), 0);
    EXPECT_EQ(IntegerMath::max(40, 20), 40);
    EXPECT_EQ(IntegerMath::max(20, 20), 20);
    EXPECT_EQ(IntegerMath::max(20, 40), 40);
    EXPECT_EQ(IntegerMath::max(0, 40), 40);
}


template<typename IntType>
void addWithOverflowTest(const std::string &type)
{
    SCOPED_TRACE(std::string("Overflow test: ") + type);
    IntType result;
    bool overflow;
    overflow = IntegerMath::addCheckOverflow<IntType>(0, 0, &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, 0);
    overflow = IntegerMath::addCheckOverflow<IntType>(0, 1, &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, 1);
    overflow = IntegerMath::addCheckOverflow<IntType>(1, 0, &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, 1);
    overflow = IntegerMath::addCheckOverflow<IntType>(0, std::numeric_limits<IntType>::max(), &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, std::numeric_limits<IntType>::max());
    overflow = IntegerMath::addCheckOverflow<IntType>(0, std::numeric_limits<IntType>::min(), &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, std::numeric_limits<IntType>::min());
    overflow = IntegerMath::addCheckOverflow<IntType>(std::numeric_limits<IntType>::max(), 0, &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, std::numeric_limits<IntType>::max());
    overflow = IntegerMath::addCheckOverflow<IntType>(1, std::numeric_limits<IntType>::max(), &result);
    EXPECT_EQ(overflow, true);
    overflow = IntegerMath::addCheckOverflow<IntType>(std::numeric_limits<IntType>::max(), std::numeric_limits<IntType>::max(), &result);
    EXPECT_EQ(overflow, true);
    if (std::numeric_limits<IntType>::is_signed) {
        overflow = IntegerMath::addCheckOverflow<IntType>(0, -1, &result);
        EXPECT_EQ(overflow, false);
        EXPECT_EQ(result, -1);
        overflow = IntegerMath::addCheckOverflow<IntType>(-1, 0, &result);
        EXPECT_EQ(overflow, false);
        EXPECT_EQ(result, -1);
    }
}

template<typename IntType>
void addWithOverflowRandomTest(const std::string &type)
{
    SCOPED_TRACE(std::string("Random test: ") + type);
    std::ranlux24_base engine(0x77); // Fixed value for the tests.
    std::uniform_int_distribution<IntType> distribution(std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max());
    for (int i = 0; i < 1000; ++i) {
        const int64_t a = distribution(engine);
        const int64_t b = distribution(engine);
        const int64_t c = a + b;
        const bool expectOverflow = static_cast<int64_t>(static_cast<IntType>(c)) != c;
        IntType result;
        const bool overflow = IntegerMath::addCheckOverflow<IntType>(static_cast<IntType>(a), static_cast<IntType>(b), &result);
        ASSERT_EQ(expectOverflow, overflow);
        ASSERT_EQ(static_cast<IntType>(c), result);
    }
}


TEST(IntegerMathTest, AddWithOverflow)
{
    addWithOverflowTest<int8_t>("int8_t");
    addWithOverflowTest<uint8_t>("uint8_t");
    addWithOverflowTest<int16_t>("int16_t");
    addWithOverflowTest<uint16_t>("uint16_t");
    addWithOverflowTest<int32_t>("int32_t");
    addWithOverflowTest<uint32_t>("uint32_t");
    addWithOverflowTest<int64_t>("int64_t");
    addWithOverflowTest<uint64_t>("uint64_t");

    addWithOverflowRandomTest<int8_t>("int8_t");
    addWithOverflowRandomTest<uint8_t>("uint8_t");
    addWithOverflowRandomTest<int16_t>("int16_t");
    addWithOverflowRandomTest<uint16_t>("uint16_t");
    addWithOverflowRandomTest<int32_t>("int32_t");
    addWithOverflowRandomTest<uint32_t>("uint32_t");
}


TEST(IntegerMathTest, MultiplyWithOverflow)
{
    int8_t result;
    bool overflow = IntegerMath::multiplyCheckOverflow(static_cast<int8_t>(-4), static_cast<int8_t>(10), &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, -40);
    overflow = IntegerMath::multiplyCheckOverflow(static_cast<int8_t>(-80), static_cast<int8_t>(10), &result);
    EXPECT_EQ(overflow, true);
    overflow = IntegerMath::multiplyCheckOverflow(static_cast<int8_t>(10), static_cast<int8_t>(-4), &result);
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(result, -40);
    overflow = IntegerMath::multiplyCheckOverflow(static_cast<int8_t>(10), static_cast<int8_t>(-80), &result);
    EXPECT_EQ(overflow, true);
}
