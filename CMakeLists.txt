cmake_minimum_required(VERSION 3.10)

if (UNIX)
	set(CMAKE_C_COMPILER "gcc-8")
	set(CMAKE_CXX_COMPILER "g++-8")
elseif (WIN32)
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
else ()
	message(FATAL_ERROR "Platform is not supported!")
endif ()

project(fps-game)

set(FPS_GAME_VERSION_MAJOR 0)
set(FPS_GAME_VERSION_MINOR 1)

configure_file(
	"${PROJECT_SOURCE_DIR}/fps-game-config.h.in"
	"${PROJECT_BINARY_DIR}/fps-game-config.h"
)

set(SOURCES
	game/sources/main.cc
	game/sources/camera.cc
	game/sources/glad.c
	game/sources/mesh.cc
	game/sources/model.cc
	game/sources/shader.cc
)
add_executable(thegame ${SOURCES})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)

target_compile_options(thegame PUBLIC -Wall -O2)
target_compile_features(thegame PUBLIC cxx_std_17)

target_include_directories(thegame PUBLIC include lib/glfw/include game/headers .)

# target_link_directories(thegame lib)
if (UNIX)
	# Linux specific libraries
	set(PLAT_SPEC_LIBS "dl")
elseif (WIN32)
	# Windows specific libraries
	set(PLAT_SPEC_LIBS "")
else ()
	message(FATAL_ERROR "Platform is not supported!")
endif ()

target_link_libraries(thegame stdc++fs assimp glfw ${PLAT_SPEC_LIBS})
