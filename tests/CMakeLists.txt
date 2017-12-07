cmake_minimum_required(VERSION 3.5)
project(BasicThreadPoolTests)

add_subdirectory(googletest)

include_directories(../include)
include_directories(googletest/googletest/include)

#set(CMAKE_CXX_FLAGS -pg)

add_executable(BasicThreadPoolTests
        main.cpp
        TestThreadPool.cpp
        TestPerformance.cpp
        TestingExtend.hpp
)

target_link_libraries(BasicThreadPoolTests BasicThreadPool gtest)