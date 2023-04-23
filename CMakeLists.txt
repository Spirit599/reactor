cmake_minimum_required(VERSION 2.6)

project("Tarder")

set(CMAKE_CXX_STANDARD 17)

set(CXX_FLAGS
 -g
 -Wall
 -Wextra
 -Werror
 -std=c++17
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


find_package(Boost REQUIRED)


#动态链接LOG库
add_definitions(-DBOOST_LOG_DYN_LINK)


include_directories(${PROJECT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})


add_subdirectory(base)
add_subdirectory(net)

