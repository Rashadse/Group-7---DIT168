# CMAKE version requirement
cmake_minimum_required(VERSION 3.5)

# Project name
project(CarServices)

# Flags and standards
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# Path variables

# Included packages
find_package(libcluon REQUIRED)
include_directories(SYSTEM ${CLUON_INCLUDE_DIRS})

# Compile messages into c++
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/messages.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND cluon-msc --cpp-sources --cpp-add-include-file=messages.hpp --out=${CMAKE_BINARY_DIR}/messages.cpp ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd
        COMMAND cluon-msc --cpp-headers --out=${CMAKE_BINARY_DIR}/messages.hpp ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/messages.odvd)
include_directories(SYSTEM ${CMAKE_BINARY_DIR})

# Executables -- ADD YOUR SERVICES COMPILATION COMMANDS BELOW
add_executable(${PROJECT_NAME}-V2VService ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_BINARY_DIR}/messages.cpp ${CMAKE_CURRENT_SOURCE_DIR}/v2v/v2v.cpp)
target_link_libraries(${PROJECT_NAME}-V2VService ${CLUON_LIBRARIES})

add_executable(${PROJECT_NAME}-RC_SIMULATOR ${CMAKE_CURRENT_SOURCE_DIR}/rc_sim.cpp ${CMAKE_BINARY_DIR}/messages.cpp)
target_link_libraries(${PROJECT_NAME}-RC_SIMULATOR ${CLUON_LIBRARIES})

add_executable(${PROJECT_NAME}-FL_SIMULATOR ${CMAKE_CURRENT_SOURCE_DIR}/fl_sim.cpp ${CMAKE_BINARY_DIR}/messages.cpp)
target_link_libraries(${PROJECT_NAME}-FL_SIMULATOR ${CLUON_LIBRARIES})

add_executable(${PROJECT_NAME}-TIME_CONVERSION ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

