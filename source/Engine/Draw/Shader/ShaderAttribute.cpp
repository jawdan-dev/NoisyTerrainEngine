#include "ShaderAttribute.hpp"

ShaderAttribute::ShaderAttribute(
	const String& name,
	const GLint location, const GLenum glType,
	const size_t dataOffset, const bool isStatic
) :
	m_name(name),
	m_location(location), m_glType(glType),
	m_dataSize(glGetTypeByteSize(glType)),
	m_dataOffset(dataOffset),
	m_isStatic(isStatic) {}