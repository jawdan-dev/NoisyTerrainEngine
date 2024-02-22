#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLFW stuff.
const size_t glGetTypeBase(const GLenum glType);
const size_t glGetTypeBaseByteSize(const GLenum glType);
const size_t glGetTypeElementCount(const GLenum glType);
const size_t glGetTypeVertexSplitCount(const GLenum glType);
const size_t glGetTypeByteSize(const GLenum glType);