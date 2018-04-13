# THIS IS JUST A TEMPLATE CMAKELISTS FILE! MAKE YOUR OWN IN YOUR OWN MICROSERVICE DIRECTORY!

# CMAKE verion requirement
cmake_minimum_required(VERSION 3.5)

# Project name
project(CarServices)

# Flags and standards
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# Path variables
set(TEST_DIR tests)
set(LIB_DIR libs)

# Included packages
find_package(libcluon REQUIRED)
include_directories(SYSTEM ${CLUON_INCLUDE_DIRS})
include_directories(${LIB_DIR})

# Compile messages into c++
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/messages.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND cluon-msc --cpp-sources --cpp-add-include-file=messages.hpp --out=${CMAKE_BINARY_DIR}/messages.cpp ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd
        COMMAND cluon-msc --cpp-headers --out=${CMAKE_BINARY_DIR}/messages.hpp ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd)
include_directories(SYSTEM ${CMAKE_BINARY_DIR})

# Executables -- ADD YOUR SERVICES COMPILATION COMMANDS BELOW


# Testing
#enable_testing()
#add_executable(${PROJECT_NAME}-test ${TEST_DIR}/UnitTests.cpp <File to test>)
#add_test(NAME ${PROJECT_NAME}-test COMMAND ${PROJECT_NAME}-test)
