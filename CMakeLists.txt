cmake_minimum_required(VERSION 3.7)
project(NoisyTerrain)

# Executable Declaration.
add_executable(NoisyTerrain ${CMAKE_CURRENT_SOURCE_DIR}/source/Engine/Core/EntryPoint.cpp)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
target_sources(NoisyTerrain PRIVATE ${SRC_FILES})

# Set output directory.
set(TARGET_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/binaries/desktop)

# Target Properties.
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
# Submodule validate function.
function(check_and_add_subdirectory dir)
    if (NOT EXISTS ${dir}/CMakeLists.txt)
        message(FATAL_ERROR "Fatal: Submodule " ${dir} " not found\n")
    endif ()
    add_subdirectory(${dir})
endfunction()

# Custom modules.
check_and_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/JLib)
target_link_libraries(NoisyTerrain PRIVATE JLib)

# GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
check_and_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/GLFW)
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/GLFW/include)
target_link_libraries(NoisyTerrain PRIVATE glfw)

# GLEW.
set(glew-cmake_BUILD_SHARED FALSE)
set(glew-cmake_BUILD_STATIC ON)
set(ONLY_LIBS ON)
check_and_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/GLEW)
target_include_directories(NoisyTerrain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modules/GLEW/include)
target_link_libraries(NoisyTerrain PRIVATE libglew_static)

# OpenGL.
find_package(OpenGL REQUIRED)
target_link_libraries(NoisyTerrain PRIVATE OpenGL::GL)

# Strip.
target_link_libraries(NoisyTerrain PRIVATE -s)