cmake_minimum_required(VERSION 3.7)
project(NoisyTerrainGame)

# Executable Declaration.
add_executable(NoisyTerrain ${CMAKE_SOURCE_DIR}/source/Core/EntryPoint.cpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/source/*.cpp)
target_sources(NoisyTerrain PRIVATE ${SRC_FILES})

# Target Properties.
set_target_properties(
    NoisyTerrain
    PROPERTIES
    CXX_STANDARD 20
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/binaries/
)

# Includes.
target_include_directories(
    NoisyTerrain PRIVATE
    ${CMAKE_SOURCE_DIR}/JDEngine/source/
)

## Modules.
# Modules Folder Include.
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/modules)

# Custom modules.
add_subdirectory(modules/JLib)

# GLFW.
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_SOURCE_DIR}/modules/glfw/include)
target_link_directories(NoisyTerrain PRIVATE ${CMAKE_SOURCE_DIR}/modules/glfw/lib-mingw-w64)

# GLEW.
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_SOURCE_DIR}/modules/glew/include)
target_link_directories(NoisyTerrain PRIVATE ${CMAKE_SOURCE_DIR}/modules/glew/lib/Release/x64 ${CMAKE_SOURCE_DIR}/modules/glew/lib/Release/Win32)

# Library linking.
target_link_libraries(
	NoisyTerrain PRIVATE
	# Custom.
	JLib
	#Other
	-Wl,--export-all-symbols,-allow-multiple-definition
	-static glew32s glfw3 opengl32 gdi32
)