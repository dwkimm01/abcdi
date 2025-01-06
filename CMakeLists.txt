cmake_minimum_required(VERSION 3.22)
project(abcdi)

option(RUN_TESTS "Build the tests" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fstack-protector-all -pedantic -pedantic-errors")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(abcdi_srcs
    include/abcdi/abcdi.h
    include/abcdi/abcdi_always_create.h
    include/abcdi/abcdi_util.h
)

if(RUN_TESTS)
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLE_LIBS)
    add_subdirectory(src)
endif()
