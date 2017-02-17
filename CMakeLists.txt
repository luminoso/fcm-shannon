cmake_minimum_required(VERSION 3.5)
project(cav_gmz)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 ")

SET(BOOST_ROOT "/usr/lib/")
SET(BOOST_INCLUDEDIR "/usr/include")

find_package(Boost 1.55 REQUIRED system serialization)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

set(SOURCE_FILES main.cpp fcm.cpp fcm.h)
add_executable(fcm ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(fcm ${Boost_LIBRARIES})