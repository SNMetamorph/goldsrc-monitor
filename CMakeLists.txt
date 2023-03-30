cmake_minimum_required(VERSION 3.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
else()
    set(VCPKG_TARGET_TRIPLET "x86-windows-static")
endif()

set(VCPKG_OVERLAY_PORTS "${CMAKE_SOURCE_DIR}/vcpkg/overlay_ports")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(goldsrc-monitor)

# Build options
option(BUILD_LOADER "Build loader" ON)
option(BUILD_LIBRARY "Build library" ON)
option(BUILD_WRAPPER "Build library as wrapper for game library" OFF)
option(ENABLE_WINXP_SUPPORT "Enable Windows XP support" OFF)

# multithreaded build flag for MSVC
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>) 

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ENABLE_WINXP_SUPPORT FALSE)
    add_definitions(-DAPP_SUPPORT_64BIT)
endif()

if(NOT BUILD_WRAPPER AND BUILD_LOADER)
    add_subdirectory(cmake/loader)
endif()

if(BUILD_LIBRARY)
    add_subdirectory(cmake/library)
endif()

# set build output directory
set(DIR_COMMON_OUTPUT 
	${CMAKE_BINARY_DIR}/$<CONFIG>/bin
)

set_target_properties(loader library PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY ${DIR_COMMON_OUTPUT}
	LIBRARY_OUTPUT_DIRECTORY ${DIR_COMMON_OUTPUT}
	RUNTIME_OUTPUT_DIRECTORY ${DIR_COMMON_OUTPUT}
)

INSTALL(
    DIRECTORY ${CMAKE_SOURCE_DIR}/resources/program_dir/ 
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)
