cmake_minimum_required(VERSION 3.3)

project(goldsrc-monitor-library CXX)
include("../cotire.cmake")
include("../compiler_runtime.cmake")

if (POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

cmake_policy(SET CMP0063 NEW)
if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

add_library(${PROJECT_NAME} SHARED
	../../sources/exception.cpp
	../../sources/string_stack.cpp
	../../sources/library/application.cpp
	../../sources/library/local_player.cpp
	../../sources/library/client_module.cpp
	../../sources/library/server_module.cpp
	../../sources/library/engine_module.cpp
	../../sources/library/utils.cpp
	../../sources/library/cvars.cpp
	../../sources/library/buildinfo.cpp
	../../sources/library/display_mode.h
	../../sources/library/displaymode_angletracking.cpp
	../../sources/library/displaymode_entityreport.cpp
	../../sources/library/displaymode_full.cpp
	../../sources/library/displaymode_measurement.cpp
	../../sources/library/displaymode_speedometer.cpp
	../../sources/library/entity_dictionary.cpp
	../../sources/library/entity_description.cpp
	../../sources/library/dll_main.cpp
	../../sources/library/hooks.cpp
	../../sources/library/stdafx.cpp
	../../sources/library/library.rc
)

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
	../../sources/library
)

# Configure & add PolyHook library target
set (POLYHOOK_BUILD_STATIC_RUNTIME ON CACHE BOOL "")
set (POLYHOOK_DISASM_CAPSTONE OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_PE OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_VIRTUALS OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_EXCEPTION OFF CACHE BOOL "")

add_subdirectory(../../external/polyhook polyhook)
set_compiler_runtime(PolyHook_2 STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	OUTPUT_NAME "gsm-library"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "../../sources/library/stdafx.h")
cotire(${PROJECT_NAME})

if (MSVC)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		_CRT_SECURE_NO_WARNINGS=1 # disable CRT warnings
		PSAPI_VERSION=1 # for WinXP compatibility
	)
	
	if (CMAKE_CONFIGURATION_TYPES MATCHES Debug)
		target_compile_options(${PROJECT_NAME} PRIVATE /Od) # disable optimizing at all
		# enable "Edit and Continue" MSVC feature
		target_compile_options(${PROJECT_NAME} PRIVATE /ZI) 
		target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL /SAFESEH:NO)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE /Ob1) # disable force function inlining, to prevent segfault in release mode
	endif()

	set_compiler_runtime(${PROJECT_NAME} STATIC)
	target_compile_options(${PROJECT_NAME} PRIVATE /wd4005) # macro redefinition warning disable
	target_compile_options(${PROJECT_NAME} PRIVATE /Zc:threadSafeInit-) # for WinXP compatibility (to avoid LoadLibrary error #998)
	target_link_libraries(${PROJECT_NAME} PRIVATE 
		PolyHook_2
		opengl32
		psapi
		libvcruntime
	)
endif()
