#pragma once
#include <Engine/Core/Core.hpp>

#include <Engine/Draw/ShaderAttribute.hpp>
#include <Engine/Draw/ShaderUniform.hpp>

class Shader {
private:
	const char* m_file;
	bool m_loaded;
	GLuint m_shaderProgram;
	Map<String, ShaderAttribute> m_attributes;
	Map<String, ShaderUniform> m_uniforms;
	size_t m_instanceTotalSize, m_staticTotalSize;

public:
	Shader(const char* file);
	Shader(const Shader& other) = delete;
	~Shader();

public:
	const size_t getTotalInstanceSize();
	const size_t getTotalStaticSize();
	const GLuint getProgram();
	const ShaderAttribute* const getAttribute(const char* attributeName);
	const List<ShaderAttribute> getAttributes();
	const ShaderUniform* const getUniform(const char* uniformName);
	const List<ShaderUniform> getUniforms();

private:
	void load();
};