cmake_minimum_required(VERSION 3.3)

project(library CXX)
include(CompilerRuntime)
include(SupportWinXP)

add_library(${PROJECT_NAME} SHARED
	"../../sources/exception.cpp"
	"../../sources/string_stack.cpp"
	"../../sources/module_info.cpp"
	"../../sources/library/application.cpp"
	"../../sources/library/bounding_box.cpp"
	"../../sources/library/bvh_tree.cpp"
	"../../sources/library/bvh_tree_node.cpp"
	"../../sources/library/local_player.cpp"
	"../../sources/library/client_module.cpp"
	"../../sources/library/server_module.cpp"
	"../../sources/library/engine_module.cpp"
	"../../sources/library/utils.cpp"
	"../../sources/library/cvars.cpp"
	"../../sources/library/build_info.cpp"
	"../../sources/library/build_info_impl.cpp"
	"../../sources/library/build_info_entry.cpp"
	"../../sources/library/memory_pattern.cpp"
	"../../sources/library/display_mode.h"
	"../../sources/library/displaymode_angletracking.cpp"
	"../../sources/library/displaymode_entityreport.cpp"
	"../../sources/library/displaymode_facereport.cpp"
	"../../sources/library/displaymode_full.cpp"
	"../../sources/library/displaymode_measurement.cpp"
	"../../sources/library/displaymode_speedometer.cpp"
	"../../sources/library/entity_dictionary.cpp"
	"../../sources/library/entity_description.cpp"
	"../../sources/library/dll_main.cpp"
	"../../sources/library/hooks.cpp"
	"../../sources/library/hooks_impl.cpp"
	"../../sources/library/stdafx.cpp"
	"../../sources/library/library.rc"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"../../external/polyhook"
	"../../external/polyhook/asmjit"
	"../../external/polyhook/zydis/msvc"
	"../../external/polyhook/zydis/include"
	"../../external/polyhook/zydis/dependencies/zycore/include"
	"../../external/polyhook"
	"../../external/rapidjson/include"
	"../../external/hlsdk/common"
	"../../external/hlsdk/pm_shared"
	"../../external/hlsdk/engine"
	"../../external/hlsdk/game_shared"
	"../../external/hlsdk/external"
	"../../external/hlsdk/utils/vgui/include"
	"../../external/hlsdk/public"
	"../../external/hlsdk/cl_dll"
	"../../sources"
	"../../sources/library"
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	OUTPUT_NAME "gsm-library"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

# configure & add PolyHook library target
set (POLYHOOK_DISASM_CAPSTONE OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_PE OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_VIRTUALS OFF CACHE BOOL "")
set (POLYHOOK_FEATURE_EXCEPTION OFF CACHE BOOL "")

# configure dependency libraries to use static runtime
set (POLYHOOK_BUILD_STATIC_RUNTIME ON CACHE BOOL "")
set (ASMJIT_STATIC ON CACHE BOOL "")

add_subdirectory("../../external/polyhook" polyhook EXCLUDE_FROM_ALL)

if (MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /wd4005) # macro redefinition warning disable
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		_CRT_SECURE_NO_WARNINGS=1 # disable CRT warnings
	)
	
	if(ENABLE_WINXP_SUPPORT)
		target_compile_definitions(${PROJECT_NAME} PRIVATE PSAPI_VERSION=1)
		target_compile_options(${PROJECT_NAME} PRIVATE /Zc:threadSafeInit-) # for WinXP compatibility (to avoid LoadLibrary error #998)
		enable_winxp_support(${PROJECT_NAME})
	endif()

	# debug mode compiler flags
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/Od>) # disable optimizing at all
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>) # enable "Edit and Continue" MSVC feature
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:
		/INCREMENTAL 
		/SAFESEH:NO
	>)

	# release mode compiler flags
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/Ob1>) # disable force function inlining, to prevent segfault in release mode
	target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/O1>) # enable optimizing to minimize size
else()
endif()

# configure to use static runtime
set_compiler_runtime(${PROJECT_NAME} STATIC)

# linking libraries
if(MSVC)
	target_link_libraries(${PROJECT_NAME} PRIVATE 
		"PolyHook_2"
		"opengl32"
		"psapi"
	)
else()
endif()

# copy compiled binaries to install directory
install( TARGETS ${PROJECT_NAME}
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
	    GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE 
)
