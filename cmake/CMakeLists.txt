cmake_minimum_required(VERSION 3.3)

if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
cmake_policy(SET CMP0063 NEW)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(goldsrc-monitor  CXX)

if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(i386|i486|i586|i686|x86_64|AMD64)$")
    # OK
else()
    message(FATAL_ERROR "This project can be compiled only in x86 mode on x86/x64 processors (got ${CMAKE_SIZEOF_VOID_P} and ${CMAKE_SYSTEM_PROCESSOR})")
endif()

if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(loader)
add_subdirectory(library)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "../bin")

foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config)
    set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${config} ""../bin")
endforeach()

target_link_libraries(${PROJECT_NAME} PRIVATE Opus SILK Speex)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()