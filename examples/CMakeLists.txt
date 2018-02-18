# CMAKE verion requirement
cmake_minimum_required(VERSION 3.5)

# Project name
project(examples)

# Flags and standards
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# Included packages
find_package(libcluon REQUIRED)
include_directories(SYSTEM ${CLUON_INCLUDE_DIRS})

# Executables
add_executable(${PROJECT_NAME}-receiver ${CMAKE_CURRENT_SOURCE_DIR}/receiver.cpp)
target_link_libraries(${PROJECT_NAME}-receiver ${CLUON_LIBRARIES})

add_executable(${PROJECT_NAME}-sender ${CMAKE_CURRENT_SOURCE_DIR}/sender.cpp)
target_link_libraries(${PROJECT_NAME}-sender ${CLUON_LIBRARIES})

# Testing
#enable_testing()
#add_executable(${PROJECT_NAME}-Runner TestSender.cpp ${CMAKE_CURRENT_SOURCE_DIR}/sender.cpp)
#target_link_libraries(${PROJECT_NAME}-Runner ${CLUON_LIBRARIES})
#add_test(NAME ${PROJECT_NAME}-Runner COMMAND ${PROJECT_NAME}-Runner)
