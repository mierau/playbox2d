cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME Playbox2d)
set(PLAYDATE_GAME_DEVICE Playbox2d_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB IMAGES
	"Source/images/*"
)
set(C_SOURCES
extension/main.c
playbox2d/arbiter.c
playbox2d/arbiter.h
playbox2d/array.c
playbox2d/array.h
playbox2d/body.c
playbox2d/body.h
playbox2d/collide.c
playbox2d/joint.c
playbox2d/joint.h
playbox2d/maths.c
playbox2d/maths.h
playbox2d/platform.h
playbox2d/playbox.c
playbox2d/playbox.h
playbox2d/world.c
playbox2d/world.h
)
if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} ${SDK}/C_API/buildsupport/setup.c ${C_SOURCES})
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED ${C_SOURCES} ${IMAGES})
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)
