cmake_minimum_required(VERSION 3.3)

project(goldsrc-monitor)

if (CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-bit
    set(APPLY_WINXP_SUPPORT TRUE)
else() # 64-bit
    set(APPLY_WINXP_SUPPORT FALSE)
    add_definitions(-DAPP_SUPPORT_64BIT)
endif()

if (APPLY_WINXP_SUPPORT EQUAL TRUE)
include("cmake/msvc_winxp_support.cmake")
endif()

if (POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

cmake_policy(SET CMP0063 NEW)
if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

if (NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(cmake/loader)
add_subdirectory(cmake/library)

if (APPLY_WINXP_SUPPORT EQUAL TRUE)
    enable_winxp_support()
endif()

set_target_properties(goldsrc-monitor-loader goldsrc-monitor-library PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY ../../bin
	LIBRARY_OUTPUT_DIRECTORY ../../bin
	RUNTIME_OUTPUT_DIRECTORY ../../bin
)
