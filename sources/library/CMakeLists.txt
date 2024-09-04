cmake_minimum_required(VERSION 3.19)

project(library CXX)
include(CompilerRuntime)
include(SupportWinXP)

list(APPEND MAIN_SOURCES
	"../exception.cpp"
	"../string_stack.cpp"
	"../sys_utils.cpp"
	"application.cpp"
	"bounding_box.cpp"
	"bvh_tree.cpp"
	"bvh_tree_node.cpp"
	"local_player.cpp"
	"client_module.cpp"
	"server_module.cpp"
	"engine_module.cpp"
	"utils.cpp"
	"cvars.cpp"
	"build_info.cpp"
	"build_info_impl.cpp"
	"build_info_entry.cpp"
	"memory_pattern.cpp"
	"display_mode.h"
	"displaymode_angletracking.cpp"
	"displaymode_entityreport.cpp"
	"displaymode_facereport.cpp"
	"displaymode_full.cpp"
	"displaymode_measurement.cpp"
	"displaymode_speedometer.cpp"
	"opengl_primitives_renderer.cpp"
	"entity_dictionary.cpp"
	"entity_description.cpp"
	"dll_main.cpp"
	"hooks.cpp"
	"hooks_impl.cpp"
	"hooks_logger.cpp"
	"stdafx.cpp"
)

if(WIN32)
	list(APPEND MAIN_SOURCES
		"library.rc"
	)
endif()

add_library(${PROJECT_NAME} SHARED ${MAIN_SOURCES})
find_path(HLSDK_DIRECTORY "cl_dll/GameStudioModelRenderer.h" PATH_SUFFIXES "hlsdk")
target_include_directories(${PROJECT_NAME} PRIVATE
	"${HLSDK_DIRECTORY}/common"
	"${HLSDK_DIRECTORY}/pm_shared"
	"${HLSDK_DIRECTORY}/engine"
	"${HLSDK_DIRECTORY}/game_shared"
	"${HLSDK_DIRECTORY}/external"
	"${HLSDK_DIRECTORY}/utils/vgui/include"
	"${HLSDK_DIRECTORY}/public"
	"${HLSDK_DIRECTORY}/cl_dll"
	"."
	"../"
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	OUTPUT_NAME "gsm-library"
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

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
		"opengl32"
		"psapi"
	)
else()
endif()

find_package(PolyHook_2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE PolyHook_2::PolyHook_2)

find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE rapidjson)

# copy compiled binaries to install directory
install( TARGETS ${PROJECT_NAME}
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
	    GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE 
)
