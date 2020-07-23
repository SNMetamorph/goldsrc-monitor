cmake_minimum_required(VERSION 3.3)

if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
cmake_policy(SET CMP0063 NEW)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(library CXX)

get_filename_component(PARENT_DIR ${MYPROJECT_DIR} DIRECTORY)

add_library(library SHARED
../../sources/exception.cpp
../../sources/string_stack.cpp
../../sources/library/util.cpp
../../sources/library/buildinfo.cpp
../../sources/library/core.cpp
../../sources/library/display_mode.h
../../sources/library/displaymode_angletracking.cpp
../../sources/library/displaymode_entityreport.cpp
../../sources/library/displaymode_full.cpp
../../sources/library/displaymode_measurement.cpp
../../sources/library/displaymode_speedometer.cpp
../../sources/library/dll_main.cpp
../../sources/library/globals.cpp
../../sources/library/hooks.cpp
../../sources/library/stdafx.cpp
../../sources/library/util.cpp
)

target_include_directories(library PRIVATE
	../../external/polyhook
	../../external/polyhook/asmjit
	../../external/polyhook/zydis/msvc
	../../external/polyhook/zydis/include
	../../external/polyhook/zydis/dependencies/zycore/include
	../../sources
	../../sources
	../../sources/library
)