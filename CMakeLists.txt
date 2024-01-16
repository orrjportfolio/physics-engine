cmake_minimum_required(VERSION 3.25)

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
