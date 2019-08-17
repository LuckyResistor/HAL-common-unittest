//
// (c)2018 by Lucky Resistor. See LICENSE for details.
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
#include "hal-common/RingBuffer.hpp"

#include "gtest/gtest.h"

#include <random>
#include <vector>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cert-msc32-c"


using lr::RingBuffer;


/// Construct a number of different ring buffers.
/// This just tests if the buffer can compile using different types.
///
TEST(RingBufferTest, ConstructBuffers)
{
    RingBuffer<uint8_t, uint8_t> byteRingBuffer(0x10);
    RingBuffer<uint16_t, char> charRingBuffer(0x80);
    RingBuffer<uint32_t, int> intRingBuffer(0x100);
    RingBuffer<uint64_t, uint64_t> int64RingBuffer(0x200);
}


/// Test the state of a disabled buffer after construction.
///
TEST(RingBufferTest, DisabledBuffer)
{
    RingBuffer<uint8_t, uint8_t> disabledBuffer(0);
    EXPECT_EQ(true, disabledBuffer.isDisabled());
    EXPECT_EQ(false, disabledBuffer.isEnabled());
    EXPECT_EQ(0, disabledBuffer.getSize());
    EXPECT_EQ(0, disabledBuffer.getCount());
    EXPECT_EQ(true, disabledBuffer.isEmpty());
    // Try some operations.
    uint8_t dummyByte = 0;
    disabledBuffer.write(&dummyByte, 1); // no crash...
    auto readBytes = disabledBuffer.read(&dummyByte, 1); // no crash...
    EXPECT_EQ(0, readBytes);
    readBytes = disabledBuffer.readToEnd(&dummyByte, 1, 0); // no crash...
    EXPECT_EQ(0, readBytes);
    // This should not change the state.
    EXPECT_EQ(true, disabledBuffer.isDisabled());
    EXPECT_EQ(false, disabledBuffer.isEnabled());
    EXPECT_EQ(0, disabledBuffer.getSize());
    EXPECT_EQ(0, disabledBuffer.getCount());
    EXPECT_EQ(true, disabledBuffer.isEmpty());
    // Also test reset
    disabledBuffer.reset(); // no crash...
}


/// Test the state of a buffer after construction.
///
TEST(RingBufferTest, BufferAfterConstruction)
{
    const uint8_t bufferSize = 0x62;
    RingBuffer<uint8_t, uint8_t> ringBuffer(bufferSize);
    EXPECT_EQ(false, ringBuffer.isDisabled());
    EXPECT_EQ(true, ringBuffer.isEnabled());
    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(0, ringBuffer.getCount());
    EXPECT_EQ(true, ringBuffer.isEmpty());
    // Write a byte.
    uint8_t dummyByte = 0xaa;
    ringBuffer.write(&dummyByte, 1);
    EXPECT_EQ(0xaa, dummyByte);
    EXPECT_EQ(false, ringBuffer.isDisabled());
    EXPECT_EQ(true, ringBuffer.isEnabled());
    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(false, ringBuffer.isEmpty());
    EXPECT_EQ(1, ringBuffer.getCount());
    // Read a byte.
    dummyByte = 0;
    auto readBytes = ringBuffer.read(&dummyByte, 1);
    EXPECT_EQ(1, readBytes);
    EXPECT_EQ(0xaa, dummyByte);
    EXPECT_EQ(false, ringBuffer.isDisabled());
    EXPECT_EQ(true, ringBuffer.isEnabled());
    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(true, ringBuffer.isEmpty());
    EXPECT_EQ(0, ringBuffer.getCount());
    // Read another byte.
    dummyByte = 0xee;
    readBytes = ringBuffer.read(&dummyByte, 1);
    EXPECT_EQ(0, readBytes);
    EXPECT_EQ(0xee, dummyByte);
    EXPECT_EQ(false, ringBuffer.isDisabled());
    EXPECT_EQ(true, ringBuffer.isEnabled());
    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(true, ringBuffer.isEmpty());
    EXPECT_EQ(0, ringBuffer.getCount());
}


/// Test the `getCount()` and `isEmpty()` function.
///
TEST(RingBufferTest, TestGetCount)
{
    const uint16_t testDataSize = 0x100;
    uint8_t testData[testDataSize];
    std::memset(testData, 0xaa, testDataSize);
    uint8_t readBuffer[testDataSize];
    std::memset(readBuffer, 0x00, testDataSize);
    std::ranlux24_base engine(0x56); // Fixed value for the tests.
    std::uniform_int_distribution<uint16_t> distribution(0, testDataSize);
    auto testCount = [&](uint32_t bufferSize) {
        RingBuffer<uint16_t, uint8_t> ringBuffer(static_cast<uint16_t>(bufferSize));
        // Using as larger type to handle overflows easier than the buffer has to.
        uint32_t expectedCount = 0;
        uint32_t overflowCount = 0;
        // Count up
        for (int i = 0; i < 0x1000; ++i) {
            uint16_t writeCount = distribution(engine);
            expectedCount += writeCount;
            if (expectedCount > bufferSize) {
                expectedCount = bufferSize;
            }
            ringBuffer.write(testData, writeCount);
            ASSERT_EQ(static_cast<uint16_t>(expectedCount), ringBuffer.getCount());
            ASSERT_EQ(ringBuffer.getCount() == 0, ringBuffer.isEmpty());
            if (expectedCount == bufferSize) {
                if (++overflowCount >= 5) {
                    break;
                }
            }
        }
        // Count down.
        expectedCount = ringBuffer.getCount();
        overflowCount = 0;
        for (int i = 0; i < 0x1000; ++i) {
            uint16_t readCount = distribution(engine);
            uint16_t expectedReadCount = readCount;
            if (expectedCount > readCount) {
                expectedCount -= readCount;
            } else {
                expectedReadCount = expectedCount;
                expectedCount = 0;
            }
            const auto actualReadCount = ringBuffer.read(readBuffer, readCount);
            ASSERT_EQ(static_cast<uint16_t>(expectedCount), ringBuffer.getCount());
            ASSERT_EQ(expectedReadCount, actualReadCount);
            ASSERT_EQ(ringBuffer.getCount() == 0, ringBuffer.isEmpty());
            if (expectedCount == 0) {
                if (++overflowCount >= 5) {
                    break;
                }
            }
        }
    };
    // Test various sizes.
    const int bufferSizes[] = {
            0x08, 0x0f, 0x10, 0x20, 0x21, 0x1f, 0x40, 0x48, 0x6e, 0x7f, 0x80, 0xc8, 0xfe, 0xff,
            0x100, 0x123, 0x200, 0x400, 0x1023, 0x2934, 0xfffe, 0xffff};
    for (const auto bufferSize : bufferSizes) {
        testCount(bufferSize);
    }
}


/// Helper method for the `SmallAmountWrites` test.
///
template<typename Size, typename Element>
void testSmallAmountWrites(Size bufferSize)
{
    RingBuffer<Size, Element> byteBuffer(bufferSize);
    for (uint64_t i = 0; i < 0x2000; ++i) {
        auto we = static_cast<Element>(i);
        byteBuffer.write(&we, 1);
        Element re = 0;
        auto elementCount = byteBuffer.read(&re, 1);
        EXPECT_EQ(1, elementCount);
        EXPECT_EQ(we, re);
    }
}


/// This tests basically makes sure the ring buffer compiles with different types
/// and can be created in various sizes. The test will write and read single
/// elements from the buffer.
///
TEST(RingBufferTest, SmallAmountWrites)
{
    testSmallAmountWrites<uint8_t, uint8_t>(0x10);
    testSmallAmountWrites<uint8_t, uint8_t>(0x20);
    testSmallAmountWrites<uint8_t, uint8_t>(0x40);
    testSmallAmountWrites<uint8_t, uint8_t>(0x81);
    testSmallAmountWrites<uint8_t, uint8_t>(0xff);
    testSmallAmountWrites<uint16_t, uint16_t>(0x0fe);
    testSmallAmountWrites<uint16_t, uint16_t>(0x0ff);
    testSmallAmountWrites<uint16_t, uint16_t>(0x100);
    testSmallAmountWrites<uint16_t, uint16_t>(0x101);
    testSmallAmountWrites<uint32_t, uint32_t>(0x193);
    testSmallAmountWrites<uint64_t, uint64_t>(0x239);
}


/// Helper class for the `WritesAndRead` test.
///
template<typename Size, typename Element>
class SmallWritesDelayedRead
{
public:
    SmallWritesDelayedRead()
    {
        // Prepare the test data.
        std::ranlux24_base engine(0x56); // Fixed value for the tests.
        std::uniform_int_distribution<Element> distribution;
        for (int i = 0; i < _dataSize; ++i) {
            _data[i] = distribution(engine);
        }
    }

    void testDelay(Size bufferSize, int delay)
    {
        // Create the buffer to test.
        RingBuffer<Size, Element> ringBuffer(bufferSize);

        // Create our read and write pointers.
        int readIndex = 0;
        int writeIndex = 0;

        // First write some data without reading it.
        for (int i = 0; i < delay; ++i) {
            ringBuffer.write(&_data[writeIndex], 1);
            writeIndex += 1;
            if (writeIndex >= _dataSize) {
                writeIndex = 0;
            }
        }

        // Now write and read, checking the returned data.
        for (int i = 0; i < 0x400; ++i) {
            ringBuffer.write(&_data[writeIndex], 1);
            writeIndex += 1;
            if (writeIndex >= _dataSize) {
                writeIndex = 0;
            }
            Element readElement = 0;
            const auto readBytes = ringBuffer.read(&readElement, 1);
            ASSERT_EQ(1, readBytes);
            ASSERT_EQ(_data[readIndex], readElement);
            readIndex += 1;
            if (readIndex >= _dataSize) {
                readIndex = 0;
            }
        }
    }

    void testIncreasingBlocks(Size bufferSize)
    {
        // Create the buffer to test.
        RingBuffer<Size, Element> ringBuffer(bufferSize);

        Element readBuffer[bufferSize];

        // Write and read in different block sizes.
        for (int j = 0; j < 5; ++j) {
            for (Size i = 1; i < bufferSize; i++) {
                ringBuffer.write(_data, i);
                std::memset(readBuffer, 0, sizeof(Element)*i);
                const auto readBytes = ringBuffer.read(readBuffer, i);
                ASSERT_EQ(i, readBytes);
                const auto cmpResult = std::memcmp(readBuffer, _data, sizeof(Element)*i);
                ASSERT_EQ(0, cmpResult);
            }
        }
    }

private:
    static const int _dataSize = 0x400;
    Element _data[_dataSize];
};


/// In this test we actually test the ring buffer functionality by writing
/// an number of elements into the buffer and after some write operations
/// start to read them from the buffer again.
///
TEST(RingBufferTest, WritesAndRead)
{
    SmallWritesDelayedRead<uint8_t, uint8_t> test8_8;
    SmallWritesDelayedRead<uint16_t, uint16_t> test16_16;
    SmallWritesDelayedRead<uint32_t, uint32_t> test32_32;
    SmallWritesDelayedRead<uint64_t, uint64_t> test64_64;

    test8_8.testDelay(0x10, 0x03);
    test8_8.testDelay(0x20, 0x11);
    test8_8.testDelay(0x40, 0x21);
    test8_8.testDelay(0x81, 0x22);
    test8_8.testDelay(0xff, 0x31);
    test16_16.testDelay(0x0fe, 0x07);
    test16_16.testDelay(0x0ff, 0x21);
    test16_16.testDelay(0x100, 0x34);
    test16_16.testDelay(0x101, 0x19);
    test32_32.testDelay(0x193, 0x0a);
    test64_64.testDelay(0x239, 0x1e);
}


/// In these tests we write increasingly block sizes from one element
/// up to [buffer size] elements and read them back.
///
TEST(RingBufferTest, ReadAndWriteSizes)
{
    SmallWritesDelayedRead<uint8_t, uint8_t> test8_8;

    uint8_t testSizes[] = {0x08, 0x10, 0x11, 0x20, 0x40, 0x81, 0xc2, 0xfe, 0xff};
    for (auto testSize : testSizes) {
        test8_8.testIncreasingBlocks(testSize);
    }
}


/// This test will read and write over the boundary of the ring buffer.
/// To test this, data in the size of the buffer is written and read back.
/// After each write/read operation, an additional element is added to the
/// buffer to shift the block one element further.
///
TEST(RingBufferTest, BoundaryWrap)
{
    // Create a test data vector.
    std::ranlux24_base engine(0x56); // Fixed value for the tests.
    std::uniform_int_distribution<uint8_t> distribution;
    uint8_t testData[0x200];
    for (unsigned char &i : testData) {
        i = distribution(engine);
    }
    auto test = [&](int bufferSize) {
        RingBuffer<uint8_t, uint8_t> ringBuffer(bufferSize);
        // Test
        uint8_t readBuffer[bufferSize];
        for (int i = 0; i < 0x205; ++i) {
            ringBuffer.write(testData, bufferSize);
            std::memset(readBuffer, 0, bufferSize);
            const auto readSize = ringBuffer.read(readBuffer, bufferSize);
            ASSERT_EQ(bufferSize, readSize);
            const auto cmpResult = std::memcmp(testData, readBuffer, bufferSize);
            ASSERT_EQ(0, cmpResult);
            // Add extra element
            ringBuffer.write(testData, 1);
        }
    };
    // Test various sizes.
    const int bufferSizes[] = {
            0x08, 0x0f, 0x10, 0x20, 0x21, 0x1f, 0x40, 0x48, 0x6e, 0x7f, 0x80, 0xc8, 0xfe, 0xff};
    for (const auto bufferSize : bufferSizes) {
        test(bufferSize);
    }
}

/// Test what happens if too much data is written to the buffer.
/// The ring buffer should drop exceeding data and return the last
/// block in the size of the ring buffer.
///
TEST(RingBufferTest, DroppedData)
{
    // Create a test data vector.
    const int testDataSize = 0x388;
    std::ranlux24_base engine(0x56); // Fixed value for the tests.
    std::uniform_int_distribution<uint8_t> distribution;
    uint8_t testData[testDataSize];
    for (unsigned char &i : testData) {
        i = distribution(engine);
    }
    auto test = [&](int bufferSize) {
        RingBuffer<uint16_t, uint8_t> ringBuffer(bufferSize);
        // Write one whole block
        ringBuffer.write(testData, testDataSize);
        uint8_t readBuffer[bufferSize];
        std::memset(readBuffer, 0, bufferSize);
        auto readSize = ringBuffer.read(readBuffer, bufferSize);
        ASSERT_EQ(bufferSize, readSize);
        // Expect the last portion of the test data.
        auto cmpResult = std::memcmp(testData + (testDataSize-bufferSize), readBuffer, bufferSize);
        ASSERT_EQ(0, cmpResult);
        // Repeat, but this time write single bytes.
        for (int i = 0; i < testDataSize; ++i) {
            ringBuffer.write(testData + i, 1);
        }
        // Same result?
        std::memset(readBuffer, 0, bufferSize);
        readSize = ringBuffer.read(readBuffer, bufferSize);
        ASSERT_EQ(bufferSize, readSize);
        // Expect the last portion of the test data.
        cmpResult = std::memcmp(testData + (testDataSize-bufferSize), readBuffer, bufferSize);
        ASSERT_EQ(0, cmpResult);
    };
    // Test various sizes.
    const int bufferSizes[] = {
            0x08, 0x0f, 0x10, 0x20, 0x21, 0x1f, 0x40, 0x48, 0x6e, 0x7f, 0x80, 0xc8, 0xfe, 0xff};
    for (const auto bufferSize : bufferSizes) {
        test(bufferSize);
    }
}


TEST(RingBufferTest, ReadToEnd)
{
    // Generate test data.
    const int testDataSize = 0x200;
    std::ranlux24_base engine(0x56); // Fixed value for the tests.
    std::uniform_int_distribution<uint8_t> distribution(0x40, 0x7f); // Limited range.
    uint8_t testData[testDataSize];
    for (unsigned char &i : testData) {
        i = distribution(engine);
    }
    const uint8_t endMark = 0xff; // The used end mark for the tests.
    const int bufferSize = 0xc0;
    uint8_t readBuffer[bufferSize];
    // Test
    RingBuffer<uint16_t, uint8_t> ringBuffer(bufferSize);
    auto writeBlock = [&](int size) {
        if (size > 0) {
            ringBuffer.write(testData, size);
        }
        ringBuffer.write(&endMark, 1);
    };
    auto readBlock = [&](int expectedSize, int testOffset = 0) {
        std::memset(readBuffer, 0, bufferSize);
        const auto readCount = ringBuffer.readToEnd(readBuffer, bufferSize, endMark);
        ASSERT_EQ(expectedSize + 1, readCount);
        ASSERT_EQ(endMark, readBuffer[readCount-1]);
        if (expectedSize > 0) {
            const auto cmpResult = std::memcmp(readBuffer, testData + testOffset, expectedSize);
            ASSERT_EQ(0, cmpResult);
        }
    };
    auto readSmallBlock = [&](int smallSize, int testOffset) {
        std::memset(readBuffer, 0, smallSize);
        const auto readCount = ringBuffer.readToEnd(readBuffer, smallSize, endMark);
        ASSERT_EQ(smallSize, readCount);
        ASSERT_NE(endMark, readBuffer[readCount-1]);
        const auto cmpResult = std::memcmp(readBuffer, testData + testOffset, smallSize);
        ASSERT_EQ(0, cmpResult);
    };
    auto testSequence = [&](const std::vector<int> &blockSizes) {
        for (const auto blockSize : blockSizes) {
            writeBlock(blockSize);
        }
        for (const auto blockSize : blockSizes) {
            readBlock(blockSize);
        }
    };
    // Test sequences
    testSequence({0});
    testSequence({1});
    testSequence({10});
    testSequence({0, 0, 0});
    testSequence({0, 1, 0});
    testSequence({10, 0, 20});
    testSequence({1, 2, 50});
    testSequence({0x1e, 0x1c, 0x1b, 0x1a});
    testSequence({20, 0, 19, 1, 20, 3, 5, 12});
    // Test no end mark.
    writeBlock(0x80);
    readSmallBlock(0x10, 0);
    readSmallBlock(0x20, 0x10);
    readSmallBlock(0x30, 0x30);
    readSmallBlock(0x10, 0x60);
    readBlock(0x10, 0x70);
}


/// Test the reset function briefly.
///
TEST(RingBufferTest, ResetTest)
{
    const int testDataSize = 0x60;
    uint8_t testData[testDataSize];
    std::memset(testData, 0xae, testDataSize);

    const int bufferSize = 0xb1;
    RingBuffer<uint16_t, uint8_t> ringBuffer(bufferSize);

    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(0, ringBuffer.getCount());
    EXPECT_EQ(true, ringBuffer.isEmpty());

    ringBuffer.write(testData, testDataSize);

    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(testDataSize, ringBuffer.getCount());
    EXPECT_EQ(false, ringBuffer.isEmpty());

    ringBuffer.reset();

    EXPECT_EQ(bufferSize, ringBuffer.getSize());
    EXPECT_EQ(0, ringBuffer.getCount());
    EXPECT_EQ(true, ringBuffer.isEmpty());
}


#pragma clang diagnostic pop
