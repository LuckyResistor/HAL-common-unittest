# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# Set the project name
project(HAL-common-unittest)

# Make sure we use the C++17 compiler standard
set(CMAKE_CXX_STANDARD 17)

# Set the executable name
add_executable(HAL-common-unittest src/RingBufferTest.cpp src/BCDTest.cpp src/DateTimeTest.cpp src/TimestampTest.cpp)

# Add the google test as subproject
add_subdirectory(googletest)

# Add the HAL common files.
add_subdirectory(src/hal-common)

# Add the include directories from google test.
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Link the google unittest library to our target.
target_link_libraries(HAL-common-unittest gtest gtest_main HAL-common)

