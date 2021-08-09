cmake_minimum_required(VERSION 3.10)

if (UNIX)
	set(CMAKE_C_COMPILER "gcc-8")
	set(CMAKE_CXX_COMPILER "g++-8")
elseif (WIN32)
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
else ()
	message(FATAL_ERROR "The platform is not supported")
endif ()

project(fps-game)

set(FPS_GAME_VERSION_MAJOR 0)
set(FPS_GAME_VERSION_MINOR 1)

configure_file(
	"${PROJECT_SOURCE_DIR}/fps-game-config.h.in"
	"${PROJECT_BINARY_DIR}/fps-game-config.h"
)

set(HEADERS
	fps-game-config.h
)

set(SOURCES
	game/sources/main.cc
	game/sources/service-locator.cc

	game/sources/utility/console-logger.cc

	game/sources/model/mesh.cc
	game/sources/model/model.cc
	game/sources/model/terrain.cc
	game/sources/model/assimp/assimp-terrain-loader.cc

	game/sources/collision/collision-solver.cc

	game/sources/gui/scene.cc
	game/sources/gui/text-area.cc
	game/sources/gui/bitmap-font.cc
	game/sources/gui/bitmap-font-renderer.cc

	game/sources/renderer/camera.cc
	game/sources/renderer/opengl/shader.cc
	game/sources/renderer/opengl/opengl-drawable-mesh.cc
	game/sources/renderer/opengl/opengl-drawable-model.cc
	game/sources/renderer/opengl/opengl-drawable-terrain.cc
	game/sources/renderer/opengl/opengl-terrain-renderer.cc

	game/sources/input/keyboard-handler.cc
	game/sources/input/mouse-handler.cc

	external/glad/glad.c
)
add_executable(thegame ${HEADERS} ${SOURCES})
set_target_properties(thegame PROPERTIES
	CXX_STANDARD 17
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)
add_subdirectory(external/assimp)

target_compile_options(thegame PUBLIC -Wall)
target_compile_features(thegame PUBLIC cxx_std_17)

target_include_directories(thegame PUBLIC game/headers external external/stb external/glm external/glfw/include .)

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
