cmake_minimum_required(VERSION 3.3)

project(goldsrc-monitor-loader CXX)
include("../compiler_runtime.cmake")

if (POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

cmake_policy(SET CMP0063 NEW)
if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

add_executable(${PROJECT_NAME} 
	"../../sources/exception.cpp"
	"../../sources/loader/main.cpp"
	"../../sources/loader/utils.cpp"
	"../../sources/loader/application.cpp"
	"../../sources/loader/loader.rc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"../../sources"
	"../../sources/loader"
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	OUTPUT_NAME "gsm-loader"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

if (MSVC)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		UNICODE=1
		_UNICODE=1
		_CRT_SECURE_NO_WARNINGS=1 # disable CRT warnings
		PSAPI_VERSION=1 # for WinXP compatibility
	)

	if (CMAKE_CONFIGURATION_TYPES MATCHES Debug)
		target_compile_options(${PROJECT_NAME} PRIVATE /Od) # disable optimizing at all
		target_compile_options(${PROJECT_NAME} PRIVATE /ZI) # enable "Edit and Continue" MSVC feature
		target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL /SAFESEH:NO)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE /Ob1) # disable force function inlining, to prevent segfault in release mode
		target_compile_options(${PROJECT_NAME} PRIVATE /O1) # enable optimizing to minimize size
	endif()

	set_compiler_runtime(${PROJECT_NAME} STATIC)
	target_compile_options(${PROJECT_NAME} PRIVATE /Zc:threadSafeInit-) # for WinXP compatibility (to avoid LoadLibrary error #998)
	target_link_libraries(${PROJECT_NAME} PRIVATE 
		psapi 
		shlwapi
	)
endif()
