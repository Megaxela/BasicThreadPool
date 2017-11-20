cmake_minimum_required(VERSION 3.9)
project(ThreadWorkersPool)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(INCLUDE_FILES
        include/ThreadPool.hpp
        include/JobResult.hpp
        include/Job.hpp
)

set(SOURCE_FILES
        src/ThreadPool.cpp
        src/JobResult.cpp
        src/Job.cpp
)

add_library(ThreadWorkersPool
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)
target_link_libraries(ThreadWorkersPool -pthread)