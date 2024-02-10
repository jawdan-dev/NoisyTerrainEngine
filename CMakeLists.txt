cmake_minimum_required(VERSION 3.7)
project(NoisyTerrainGame)

# Executable Declaration.
add_executable(NoisyTerrain ${CMAKE_CURRENT_SOURCE_DIR}/source/NoisyTerrain/Core/EntryPoint.cpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/NoisyTerrain/*.cpp)
target_sources(NoisyTerrain PRIVATE ${SRC_FILES})

# Target Properties.
set(TARGET_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set_target_properties(
    NoisyTerrain
    PROPERTIES
    CXX_STANDARD 20
    RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIRECTORY}
)

# Includes.
target_include_directories(
    NoisyTerrain PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/
)

# Copy Assets.
file(GLOB_RECURSE ASSET_FILES_RAW ${CMAKE_CURRENT_SOURCE_DIR}/source/Assets/**)
foreach(ASSET_FILE IN LISTS ASSET_FILES_RAW)
	FILE(RELATIVE_PATH ASSET_FILE_RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/Assets/ ${ASSET_FILE})
	add_custom_command(TARGET NoisyTerrain POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSET_FILE} ${TARGET_OUTPUT_DIRECTORY}/Assets/${ASSET_FILE_RELATIVE})
endforeach()

## Modules.
# Modules Folder Include.
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Custom modules.
add_subdirectory(modules/JLib)

# GLFW.
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/glfw/include)
target_link_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/glfw/lib-mingw-w64)

# GLEW.
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/glew/include)
target_link_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/glew/lib/Release/x64 ${CMAKE_CURRENT_SOURCE_DIR}/modules/glew/lib/Release/Win32)

# Library linking.
target_link_libraries(
	NoisyTerrain PRIVATE
	# Custom.
	JLib
	#Other
	-Wl,--export-all-symbols,-allow-multiple-definition
	-static glew32s glfw3 opengl32 gdi32
)