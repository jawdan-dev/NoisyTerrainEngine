#include "Texture.hpp"

Texture::Texture(const char* file) :
	m_file(file), m_loaded(false),
	m_texture(0), m_width(0), m_height(0) {}
Texture::~Texture() {
	if (m_texture) glDeleteTextures(1, &m_texture);
}

const uint32_t Texture::getWidth() {
	load();
	// Return width.
	return m_width;
}
const uint32_t Texture::getHeight() {
	load();
	// Return height.
	return m_height;
}
const GLuint Texture::getTexture() {
	load();
	// Return texture.
	return m_texture;
}

void Texture::setActiveSlot(const GLuint activeSlot) {
	load();

	// Set active slot.
	glActiveTexture(GL_TEXTURE0 + activeSlot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::load() {
	if (m_loaded) return;

	// Macro for switching a 4 character file extension.
#	define EXTENSION(str) ( \
		(uint64_t)str[0] << 0 | (uint64_t)str[1] << 16 |  \
		(uint64_t)str[2] << 32 | (uint64_t)str[3] << 48)

	// Get file extension.
	if (strlen(m_file) <= 4) J_ERROR_EXIT("Texture.cpp: File path is too short.\n");
	const String filePath = String("Assets/") + m_file;
	const String extensionString = filePath.substr(filePath.size() - 4);
	const uint64_t extension = EXTENSION(extensionString.c_str());

	// Open file.
	FILE* const file = fopen(filePath.c_str(), "rb");
	if (file == nullptr) J_ERROR_EXIT("Texture.cpp: Failed to open %s\n", m_file);

	// Load.
	switch (extension) {
		default: J_WARNING("Texture.cpp: File extension '%s' not supported\n", extensionString.c_str()); break;
		case EXTENSION(".bmp"): loadBMP(file); break;
	}

	// Close file.
	fclose(file);
#	undef EXTENSION;

	// Error check.
	if (m_texture == 0) J_ERROR_EXIT("Texture.cpp: Failed to load texture %s.\n", m_file);
	if (m_width == 0) J_ERROR_EXIT("Texture.cpp: Failed to load width of %s.\n", m_file);
	if (m_height == 0) J_ERROR_EXIT("Texture.cpp: Failed to load height of %s.\n", m_file);

	// Update details.
	m_loaded = true;
}
