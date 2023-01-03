cmake_minimum_required(VERSION 3.24)

project(fps-game LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -O2 -g")

set(HEADERS
	fps-game-config.h
)

set(SOURCES
	game/sources/main.cc
	game/sources/service-locator.cc
	game/sources/window-manager.cc

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
	CXX_STANDARD 20
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW off CACHE BOOL "" FORCE)
add_subdirectory(external/assimp)

target_compile_options(thegame PUBLIC -Wall)
target_compile_features(thegame PUBLIC cxx_std_20)

target_include_directories(thegame PUBLIC game/headers external external/stb external/glm external/glfw/include .)

if (UNIX)
	# Linux specific libraries
	set(PLAT_SPEC_LIBS "dl")
endif ()
target_link_libraries(thegame stdc++fs assimp glfw ${PLAT_SPEC_LIBS})
