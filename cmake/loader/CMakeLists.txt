cmake_minimum_required(VERSION 3.3)
if(MSVC)
	add_definitions(-DUNICODE -D_UNICODE)
endif()
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
cmake_policy(SET CMP0063 NEW)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(goldsrc-monitor-loader CXX)

add_executable(${PROJECT_NAME} 
../../sources/exception.cpp
../../sources/loader/main.cpp
../../sources/loader/util.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4005")
target_link_libraries(${PROJECT_NAME} PRIVATE psapi shlwapi)
target_include_directories(${PROJECT_NAME} PRIVATE
	../../sources
	../../sources/library
)