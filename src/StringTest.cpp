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
#include "hal-common/String.hpp"

#include "gtest/gtest.h"

#include <random>
#include <sstream>
#include <csignal>


using namespace lr;


TEST(StringTest, Create)
{
    String empty;
    EXPECT_EQ(empty.isEmpty(), true);
    EXPECT_EQ(empty.getLength(), 0);
    const auto strLiteral = "string literal x";
    String fromLiteral(strLiteral);
    EXPECT_EQ(fromLiteral.isEmpty(), false);
    EXPECT_EQ(fromLiteral.getLength(), 16);
    EXPECT_NE(fromLiteral.getData(), strLiteral); // test if a copy was made.
    String partialCopy(strLiteral, 4);
    EXPECT_EQ(partialCopy.isEmpty(), false);
    EXPECT_EQ(partialCopy.getLength(), 4);
    String specialEmpty("");
    EXPECT_EQ(specialEmpty.isEmpty(), true);
    EXPECT_EQ(specialEmpty.getLength(), 0);
}


TEST(StringTest, CopyAndAssign)
{
    String a("hello");
    String b(a);
    EXPECT_EQ(a, b);
    String c;
    EXPECT_EQ(c.isEmpty(), true);
    EXPECT_NE(c, a);
    c = a;
    EXPECT_EQ(c.isEmpty(), false);
    EXPECT_EQ(c, a);
}


TEST(StringTest, Compare)
{
    String a("first");
    String b("second");
    EXPECT_EQ((a == b), false);
    EXPECT_EQ((a != b), true);
    EXPECT_EQ((a < b), true);
    EXPECT_EQ((a <= b), true);
    EXPECT_EQ((a > b), false);
    EXPECT_EQ((a >= b), false);
    // same objects
    EXPECT_EQ((a == a), true);
    EXPECT_EQ((a != a), false);
    EXPECT_EQ((a < a), false);
    EXPECT_EQ((a <= a), true);
    EXPECT_EQ((a > a), false);
    EXPECT_EQ((a >= a), true);
    // With literals
    EXPECT_EQ((a == "hello"), false);
    EXPECT_EQ((a == "first"), true);
    EXPECT_EQ((a != "hello"), true);
    EXPECT_EQ((a < "second"), true);
    EXPECT_EQ((a <= "second"), true);
    EXPECT_EQ((a > "second"), false);
    EXPECT_EQ((a >= "second"), false);
}


TEST(StringTest, Append)
{
    String a;
    a.append("Text");
    EXPECT_EQ(a, "Text");
    String b("Run");
    a.append(b);
    EXPECT_EQ(a, "TextRun");
    a.append(':');
    EXPECT_EQ(a, "TextRun:");
}

// TODO: Test append and various other methods.

template<typename IntType>
void stringToNumberTests(StatusResult<IntType>(String::*memberFn)() const, const std::string &type)
{
    SCOPED_TRACE(std::string("String to number tests: ") + type);
    String a;
    a = "";
    StatusResult<IntType> result = (a.*memberFn)();
    EXPECT_EQ(result.hasError(), true);
    a = "test";
    result = (a.*memberFn)();
    EXPECT_EQ(result.hasError(), true);
    a = " 12";
    result = (a.*memberFn)();
    EXPECT_EQ(result.hasError(), true);
    a = "x12";
    result = (a.*memberFn)();
    EXPECT_EQ(result.hasError(), true);
    a = "  ";
    result = (a.*memberFn)();
    EXPECT_EQ(result.hasError(), true);
    a = "0";
    result = (a.*memberFn)();
    EXPECT_EQ(result.isSuccess(), true);
    EXPECT_EQ(result.getValue(), 0);
    a = "12";
    result = (a.*memberFn)();
    EXPECT_EQ(result.isSuccess(), true);
    EXPECT_EQ(result.getValue(), 12);
    a = String::number(std::numeric_limits<IntType>::max());
    result = (a.*memberFn)();
    EXPECT_EQ(result.isSuccess(), true);
    EXPECT_EQ(result.getValue(), std::numeric_limits<IntType>::max());
    a = String::number(std::numeric_limits<IntType>::min());
    result = (a.*memberFn)();
    EXPECT_EQ(result.isSuccess(), true);
    EXPECT_EQ(result.getValue(), std::numeric_limits<IntType>::min());

    // Test some random numbers.
    std::ranlux24_base engine(0x9a); // Fixed value for the tests.
    std::uniform_int_distribution<IntType> distribution(std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max());
    for (int i = 0; i < 1000; ++i) {
        const IntType value = distribution(engine);
        const String valueStr = String::number(value);
        std::ostringstream s;
        s << "Value " << static_cast<int64_t>(value) << " Text '" << valueStr.getData() << "'";
        SCOPED_TRACE(s.str());
        result = (valueStr.*memberFn)();
        EXPECT_EQ(result.isSuccess(), true);
        EXPECT_EQ(result.getValue(), value);
    }
}


TEST(StringTest, StringToNumber)
{
    stringToNumberTests<int8_t>(&String::toInt8, "int8_t");
    stringToNumberTests<uint8_t>(&String::toUInt8, "uint8_t");
    stringToNumberTests<int16_t>(&String::toInt16, "int16_t");
    stringToNumberTests<uint16_t>(&String::toUInt16, "uint16_t");
    stringToNumberTests<int32_t>(&String::toInt32, "int32_t");
    stringToNumberTests<uint32_t>(&String::toUInt32, "uint32_t");
}