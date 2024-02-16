#include "Shader.hpp"

Shader::Shader(const char* file) :
	m_file(file),
	m_loaded(false),
	m_shaderProgram(0),
	m_attributes(),
	m_instanceTotalSize(0), m_staticTotalSize(0) {}
Shader::~Shader() {
	// Cleanup shader.
	if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
}

const GLuint compileShader(const char* source, const GLenum shaderType) {
	// Create shader.
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) return 0;

	// Attach source.
	glShaderSource(shader, 1, &source, NULL);

	// Compile.
	glCompileShader(shader);

	// Get compilation status.
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		// Output error.
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		switch (shaderType) {
			case GL_VERTEX_SHADER: J_WARNING("Shader.cpp: Vertex shader compilation failed:\n%s\n", infoLog); break;
			case GL_GEOMETRY_SHADER: J_WARNING("Shader.cpp: Geometry shader compilation failed:\n%s\n", infoLog); break;
			case GL_FRAGMENT_SHADER: J_WARNING("Shader.cpp: Fragment shader compilation failed:\n%s\n", infoLog); break;
		}

		// Cleanup.
		glDeleteShader(shader);
		return 0;
	};

	return shader;
}

const size_t Shader::getTotalInstanceSize() {
	load();
	return m_instanceTotalSize;
}
const size_t Shader::getTotalStaticSize() {
	load();
	return m_staticTotalSize;
}
const GLuint Shader::getProgram() {
	load();
	return m_shaderProgram;
}
const ShaderAttribute* const Shader::getAttribute(const char* attributeName) {
	load();

	// Find attribute.
	auto it = m_attributes.find(attributeName);
	if (it == m_attributes.end()) return nullptr;

	// Return attribute.
	return &it->second;
}
const List<ShaderAttribute> Shader::getAttributes() {
	load();

	// Get attributes.
	List<ShaderAttribute> attributes;
	for (auto it = m_attributes.begin(); it != m_attributes.end(); it++) {
	// Add attributes to list.
		attributes.emplace_back(it->second);
	}
	return attributes;
}

void readShaderSource(const char* sourcePath, const char* defaultVersion, String& vertexSource, String& fragmentSource) {
	// Interpretation values.
	String targetVersion = String(defaultVersion) + "\n";
	enum class ShaderType {
		None,
		Vertex,
		Geometry,
		Fragment,
	} activeSource = ShaderType::None;

	// Load shader from file.
	const String filePath = String("Assets/") + sourcePath;
	FILE* file = fopen(filePath.c_str(), "rb");
	if (file == nullptr) J_ERROR_EXIT("Shader.cpp: Failed to open shader %s.", filePath.c_str());

	// Interpret file.
	char c = '\0';
	String line;
	while (!feof(file)) {
		// Read line.
		line = "";
		while (!feof(file) && c != '\n') {
			const size_t r = fread(&c, sizeof(c), 1, file);
			// Add to line.
			if (r > 0) line += c;
			else break;
		}
		c = '\0';

		// Check line.
		if (line == "#pragma vertex\n") {
			activeSource = ShaderType::Vertex;
			line = "";
		} else if (line == "#pragma geometry\n") {
			activeSource = ShaderType::Geometry;
			line = "";
		} else if (line == "#pragma fragment\n") {
			activeSource = ShaderType::Fragment;
			line = "";
		} else if (line.substr(0, 8) == "#version\n") {
			targetVersion = line;
			line = "";
		}

		// Add to shader source.
		switch (activeSource) {
			case ShaderType::Vertex: vertexSource += line; break;
			case ShaderType::Fragment: fragmentSource += line; break;
		}
	}

	// Add version to source.
	vertexSource.insert(0, targetVersion.c_str());
	fragmentSource.insert(0, targetVersion.c_str());
}
const GLuint compileProgram(const char* vertexSource, const char* fragmentSource) {
	// Compile vertex shader.
	GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
	if (vertexShader == 0) J_ERROR_EXIT("Shader.cpp: Failed to compile vertex shader.");

	// Compile fragment shader.
	GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader);
		J_ERROR_EXIT("Shader.cpp: Failed to compile fragment shader.");
	}

	// Compile program.
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Check compile status.
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		// Output error.
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		J_WARNING("Shader.cpp: Program link failed:\n%s\n", infoLog);

		// Cleanup.
		glDeleteProgram(program);
		program = 0;
	}

	// Cleanup.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
void loadShaderAttributes(
	const GLuint shaderProgram,
	Map<String, ShaderAttribute>& attributes,
	size_t& instanceTotalSize, size_t& staticTotalSize
) {
	// Clear attributes.
	attributes.clear();
	instanceTotalSize = 0;
	staticTotalSize = 0;

	// Get attributes information.
	GLint attributeCount, maxNameSize;
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	if (attributeCount <= 0) return;
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameSize);
	maxNameSize += 1;

	// Allocating the space needed for reading the attributes.
	char* nameBuffer = new char[maxNameSize];
	GLint nameLength, attributeSize, attributeLocation;
	GLenum attributeType;

	// Loading time.
	;
	for (size_t i = 0; i < attributeCount; i++) {
		// Get attribute information.
		glGetActiveAttrib(
			shaderProgram, i,
			maxNameSize, &nameLength,
			&attributeSize, &attributeType, nameBuffer
		);
		attributeLocation = glGetAttribLocation(shaderProgram, nameBuffer);

		// Check if attribute is internal.
		bool staticAttribute =
			nameLength >= 2 &&
			nameBuffer[0] == 'v' &&
			nameBuffer[1] == '_';

		// Add to attribute list.
		attributes.emplace(
			String(nameBuffer),
			ShaderAttribute(
				nameBuffer,
				attributeLocation, attributeType,
				staticAttribute ? staticTotalSize : instanceTotalSize,
				staticAttribute
			)
		);

		// Update details.
		const size_t dataSize = glGetTypeByteSize(attributeType);
		if (staticAttribute) staticTotalSize += dataSize;
		else instanceTotalSize += dataSize;
	}

	// Cleanup.
	delete[] nameBuffer;
}

void Shader::load() {
	// Ignore if already loaded.
	if (m_loaded) return;

	// Load shader from file.
	String vertexSource = "", fragmentSource = "";
	readShaderSource(m_file, "#version 460 core", vertexSource, fragmentSource);

	// Compile shader.
	m_shaderProgram = compileProgram(vertexSource.c_str(), fragmentSource.c_str());
	if (m_shaderProgram == 0) J_ERROR_EXIT("Shader.cpp: Failed to compile shader %s.", m_file);

	// Load attributes.
	loadShaderAttributes(
		m_shaderProgram, m_attributes,
		m_instanceTotalSize, m_staticTotalSize
	);

	// Update details.
	m_loaded = true;
}
