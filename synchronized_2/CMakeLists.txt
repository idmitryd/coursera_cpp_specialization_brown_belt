cmake_minimum_required(VERSION 3.8) 

set(PROJECT synchronized_2)
project(${PROJECT})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")


include_directories(./include ~/coursera/brown_belt/include)
set (SOURCES ./src/main.cpp)

find_library(PTHREAD_LIBRARY pthread REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT} ${SOURCES})
target_link_libraries(${PROJECT} PRIVATE ${PTHREAD_LIBRARY})