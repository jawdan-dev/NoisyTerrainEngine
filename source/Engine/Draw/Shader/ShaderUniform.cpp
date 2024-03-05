#include "ShaderUniform.hpp"

ShaderUniform::ShaderUniform(
	const String& name,
	const GLint location, const GLenum glType
) :
	m_name(name),
	m_location(location), m_glType(glType) {}