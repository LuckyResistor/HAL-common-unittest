//
// BCD Tests
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
#include "hal-common/BCD.hpp"

#include "gtest/gtest.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cert-msc32-c"


using namespace lr::BCD;


namespace {


struct Value
{
    uint8_t bcd;
    uint8_t bin;
};


Value testVector[] = {Value({0x00, 0}), Value({0x01, 1}), Value({0x02, 2}), Value({0x03, 3}), Value({0x08, 8}),
                      Value({0x09, 9}), Value({0x10, 10}), Value({0x11, 11}), Value({0x23, 23}), Value({0x28, 28}),
                      Value({0x39, 39}), Value({0x45, 45}), Value({0x80, 80}), Value({0x92, 92}), Value({0x97, 97}),
                      Value({0x98, 98}), Value({0x99, 99}),};


}


/// Test conversion from BCD to binary.
///
TEST(BCDTest, ConvertBcdToBin)
{
    for (const auto v : testVector) {
        const uint8_t result = convertBcdToBin(v.bcd);
        EXPECT_EQ(v.bin, result);
    }

    for (uint8_t i = 0; i < 100; ++i) {
        const uint8_t bin = i;
        const uint8_t bcd = (i%10u) | ((i/10u)<<4u);
        const uint8_t result = convertBcdToBin(bcd);
        EXPECT_EQ(bin, result);
    }
}


/// Test conversion from binary to BCD.
///
TEST(BCDTest, ConvertBinToBcd)
{
    for (const auto v : testVector) {
        const uint8_t result = convertBinToBcd(v.bin);
        EXPECT_EQ(v.bcd, result);
    }
    for (uint8_t i = 0; i < 100; ++i) {
        const uint8_t bin = i;
        const uint8_t bcd = (i%10u) | ((i/10u)<<4u);
        const uint8_t result = convertBinToBcd(bin);
        EXPECT_EQ(bcd, result);
    }
}


/// Test is the function behaves well for unexpected values.
///
TEST(BCDTest, CrashTest)
{
    for (int i = 0; i < 0x100; ++i) {
        convertBcdToBin(static_cast<uint8_t>(i));
        convertBinToBcd(static_cast<uint8_t>(i));
    }
}


#pragma clang diagnostic pop
