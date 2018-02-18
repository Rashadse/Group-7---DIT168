# CMAKE verion requirement
cmake_minimum_required(VERSION 3.5)

# Project name
project(examples)

# Flags and standards
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# Path variables
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Included packages
find_package(libcluon REQUIRED)
include_directories(SYSTEM ${CLUON_INCLUDE_DIRS})
include_directories(${LIB_DIR})
include_directories(${SOURCE_DIR})

# Executables
add_executable(${PROJECT_NAME}-receiver ${SOURCE_DIR}/receiver.cpp)
target_link_libraries(${PROJECT_NAME}-receiver ${CLUON_LIBRARIES})

add_executable(${PROJECT_NAME}-sender ${SOURCE_DIR}/sender.cpp)
target_link_libraries(${PROJECT_NAME}-sender ${CLUON_LIBRARIES})

# Testing
enable_testing()
add_executable(${PROJECT_NAME}-Runner ${TEST_DIR}/TestDummyClass.cpp ${SOURCE_DIR}/DummyTestClass.cpp)
add_test(NAME ${PROJECT_NAME}-Runner COMMAND ${PROJECT_NAME}-Runner)
