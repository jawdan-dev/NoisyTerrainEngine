#pragma once
#include <NoisyTerrain/Core/Core.hpp>

class ShaderUniform {
public:
	// TODO: Storing name as a char* would be nicer.
	String m_name;
	GLint m_location;
	GLenum m_glType;

public:
	ShaderUniform(
		const String& name,
		const GLint location, const GLenum glType
	);
};