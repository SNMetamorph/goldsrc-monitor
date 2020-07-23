cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 17)
include("../cotire.cmake")


if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
cmake_policy(SET CMP0063 NEW)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(goldsrc-monitor-library CXX)

add_library(${PROJECT_NAME} SHARED
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

add_subdirectory(../../external/polyhook ../../bin)

set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "../../sources/library/stdafx.h")
cotire(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE PolyHook_2 opengl32)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4005")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gsm-library")
target_include_directories(${PROJECT_NAME} PRIVATE
	../../external/polyhook
	../../external/polyhook/asmjit
	../../external/polyhook/zydis/msvc
	../../external/polyhook/zydis/include
	../../external/polyhook/zydis/dependencies/zycore/include
	../../external/polyhook
	../../external/hlsdk/common
	../../external/hlsdk/pm_shared
	../../external/hlsdk/engine
	../../external/hlsdk/game_shared
	../../external/hlsdk/external
	../../external/hlsdk/utils/vgui/include
	../../external/hlsdk/public
	../../external/hlsdk/cl_dll	
	../../sources
	../../sources
	../../sources/library
)