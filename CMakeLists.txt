cmake_minimum_required(VERSION 3.5)
project(BasicThreadPool)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_FLAGS -pg)

option(BASICTHREADPOOL_BUILD_TESTS "Build tests" OFF)

add_subdirectory(libraries/STLRoundBuffer)

include_directories(include)
include_directories(libraries/STLRoundBuffer/include)

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

if (${BASICTHREADPOOL_BUILD_TESTS})
    add_subdirectory(tests)
endif()

add_library(BasicThreadPool
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)
target_link_libraries(BasicThreadPool -pthread)