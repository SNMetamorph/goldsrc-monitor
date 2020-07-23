cmake_minimum_required(VERSION 3.3)

if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
cmake_policy(SET CMP0063 NEW)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(loader CXX)

get_filename_component(PARENT_DIR ${MYPROJECT_DIR} DIRECTORY)

add_executable(loader 
../../sources/exception.cpp
../../sources/loader/main.cpp
../../sources/loader/util.cpp)

target_include_directories(loader PRIVATE
	../../sources
	../../sources/library
)