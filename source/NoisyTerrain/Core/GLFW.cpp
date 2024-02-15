#include "GLFW.hpp"

#include <JLib/JLib.hpp>
using namespace JLib;

const size_t glGetTypeBase(const GLenum glType) {
	switch (glType) {
		// Booleans.
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
		case GL_BOOL: return GL_BOOL;

		// Ints.
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
		case GL_INT: return GL_INT;

		// Unsigned Ints.
		case GL_UNSIGNED_INT_VEC2:
		case GL_UNSIGNED_INT_VEC3:
		case GL_UNSIGNED_INT_VEC4:
		case GL_UNSIGNED_INT: return GL_UNSIGNED_INT;

		// Floats.
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_FLOAT: return GL_FLOAT;

		// Doubles.
		case GL_DOUBLE_VEC2:
		case GL_DOUBLE_VEC3:
		case GL_DOUBLE_VEC4:
		case GL_DOUBLE_MAT2:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT2x4:
		case GL_DOUBLE_MAT3:
		case GL_DOUBLE_MAT3x2:
		case GL_DOUBLE_MAT3x4:
		case GL_DOUBLE_MAT4:
		case GL_DOUBLE_MAT4x2:
		case GL_DOUBLE_MAT4x3:
		case GL_DOUBLE: return GL_DOUBLE;

		// Textures / Samplers
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE: return GL_SAMPLER;
	}
	J_WARNING("GLFW.cpp: Unknown GL Type [%x]\n", glType);
	return 0;
}
const size_t glGetTypeBaseByteSize(const GLenum glType) {
	// Primitives only. :)
	switch (glGetTypeBase(glType)) {
		case GL_BOOL: return sizeof(bool);
		case GL_INT: return sizeof(int);
		case GL_UNSIGNED_INT: return sizeof(unsigned int);
		case GL_FLOAT: return sizeof(float);
		case GL_DOUBLE: return sizeof(double);
		//case GL_SAMPLER: return sizeof(...);
	}
	J_WARNING("GLFW.cpp: Unknown GL Type [%x]\n", glType);
	return 0;
}
const size_t glGetTypeElementCount(const GLenum glType) {
	switch (glType) {
		// 1
		case GL_BOOL:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
		case GL_DOUBLE: return 1;

		// 2
		case GL_BOOL_VEC2:
		case GL_INT_VEC2:
		case GL_UNSIGNED_INT_VEC2:
		case GL_FLOAT_VEC2:
		case GL_DOUBLE_VEC2: return 2;

		// 3
		case GL_BOOL_VEC3:
		case GL_INT_VEC3:
		case GL_UNSIGNED_INT_VEC3:
		case GL_FLOAT_VEC3:
		case GL_DOUBLE_VEC3: return 3;

		// 4
		case GL_BOOL_VEC4:
		case GL_INT_VEC4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_FLOAT_VEC4:
		case GL_DOUBLE_VEC4:

		case GL_FLOAT_MAT2:
		case GL_DOUBLE_MAT2: return 4;

		// 6
		case GL_FLOAT_MAT2x3:
		case GL_DOUBLE_MAT2x3:
		case GL_FLOAT_MAT3x2:
		case GL_DOUBLE_MAT3x2: return 6;

		// 8
		case GL_FLOAT_MAT2x4:
		case GL_DOUBLE_MAT2x4:
		case GL_FLOAT_MAT4x2:
		case GL_DOUBLE_MAT4x2: return 8;

		// 9
		case GL_FLOAT_MAT3:
		case GL_DOUBLE_MAT3: return 9;

		// 12
		case GL_FLOAT_MAT3x4:
		case GL_DOUBLE_MAT3x4:
		case GL_FLOAT_MAT4x3:
		case GL_DOUBLE_MAT4x3: return 12;

		// 16
		case GL_FLOAT_MAT4:
		case GL_DOUBLE_MAT4: return 16;

		// Textures / Samplers
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE: return 1;
	}
	J_WARNING("GLFW.cpp: Unknown GL Type [%x]\n", glType);
	return 0;
}
const size_t glGetTypeVertexSplitCount(const GLenum glType) {
	// TODO: Im not sure whether the first or last is the determining value.
	switch (glType) {
		// 2
		case GL_FLOAT_MAT2:
		case GL_DOUBLE_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_DOUBLE_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_DOUBLE_MAT2x4: return 2;

			// 3
		case GL_FLOAT_MAT3x2:
		case GL_DOUBLE_MAT3x2:
		case GL_FLOAT_MAT3:
		case GL_DOUBLE_MAT3:
		case GL_FLOAT_MAT3x4:
		case GL_DOUBLE_MAT3x4: return 3;

			// 4
		case GL_FLOAT_MAT4x2:
		case GL_DOUBLE_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_DOUBLE_MAT4x3:
		case GL_FLOAT_MAT4:
		case GL_DOUBLE_MAT4: return 4;
	}
	return 1;
}
const size_t glGetTypeByteSize(const GLenum glType) {
	return glGetTypeBaseByteSize(glType) * glGetTypeElementCount(glType);
}