#pragma once
#include <Engine/Core/Core.hpp>

class ShaderAttribute {
public:
	// TODO: Storing name as a char* would be nicer.
	String m_name;
	GLint m_location;
	GLenum m_glType;
	size_t m_dataSize, m_dataOffset;
	bool m_isStatic;

public:
	ShaderAttribute(
		const String& name,
		const GLint location, const GLenum glType,
		const size_t dataOffset, const bool isStatic
	);
};