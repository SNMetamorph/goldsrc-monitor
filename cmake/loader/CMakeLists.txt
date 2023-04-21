cmake_minimum_required(VERSION 3.3)

project(loader CXX)
include(CompilerRuntime)
include(SupportWinXP)

add_executable(${PROJECT_NAME} 
	"../../sources/sys_utils.cpp"
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

# compiler flags
if(MSVC)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		UNICODE=1
		_UNICODE=1
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
		"psapi" 
		"shlwapi"
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
