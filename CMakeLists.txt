cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD "20")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	# causes things to fall through each other :)
	#add_compile_options("-ffast-math")
endif()

project(racingGame)

file(
	GLOB_RECURSE SOURCES
	LIST_DIRECTORIES true
	CONFIGURE_DEPENDS
	"source/*.cpp" "source/*.c"
)
add_executable(racingGame ${SOURCES})

target_include_directories(
	racingGame
	PRIVATE "source"
	PRIVATE "include"
)

target_link_libraries(
	racingGame
	"SDL2main"
	"SDL2"
	"opengl32"
)
