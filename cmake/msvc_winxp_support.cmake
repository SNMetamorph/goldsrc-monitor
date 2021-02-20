if (WINXPSUPPORT_INCLUDED)
  return ()
endif ()
set (WINXPSUPPORT_INCLUDED true)

#!	Applies some variables and compiler flags, 
#	which provides legacy Windows XP support.
#	Affects targets from current and below directories.

macro (enable_winxp_support)
	if (WIN32)
		if (MSVC)
			if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio ([0-9]+)")
				add_definitions (/D_USING_V110_SDK71_)

				if (${CMAKE_MATCH_1} LESS 11)
					# Nothing. Older VS does support XP by default.
				elseif (${CMAKE_MATCH_1} EQUAL 11)
					# Visual Studio 11 2012
					set (CMAKE_GENERATOR_TOOLSET "v110_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
					set (CMAKE_VS_PLATFORM_TOOLSET "v110_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
				elseif (${CMAKE_MATCH_1} EQUAL 12)
					# Visual Studio 12 2013
					set (CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
					set (CMAKE_VS_PLATFORM_TOOLSET "v120_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
				elseif (${CMAKE_MATCH_1} EQUAL 14)
					# Visual Studio 14 2015
					set (CMAKE_GENERATOR_TOOLSET "v140_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
					set (CMAKE_VS_PLATFORM_TOOLSET "v140_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
				elseif (${CMAKE_MATCH_1} EQUAL 15)
					# Visual Studio 15 2017
					set (CMAKE_GENERATOR_TOOLSET "v141_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
					set (CMAKE_VS_PLATFORM_TOOLSET "v141_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
				elseif (${CMAKE_MATCH_1} EQUAL 16)
					# Visual Studio 16 2019
					set (CMAKE_GENERATOR_TOOLSET "v141_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
					set (CMAKE_VS_PLATFORM_TOOLSET "v141_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
				else()
					message (WARNING "enable_winxp_support(): unknown toolset number, skipping")
				endif()
			else()
				message (FATAL_ERROR "enable_winxp_support(): CMake generator doesn't match Visual Studio")
			endif()	
		else()
			message (WARNING "enable_winxp_support(): compiler is not MSVC, skipping")
		endif()
	else()
		message (WARNING "enable_winxp_support(): target OS is not Windows, skipping")
	endif()
endmacro()
