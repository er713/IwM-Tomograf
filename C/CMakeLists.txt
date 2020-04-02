cmake_minimum_required(VERSION 3.15)
project(transforms C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -lcunit")

add_executable(transforms transform.c #[[munit/munit.h munit/munit.c]])
target_link_libraries(transforms m)
