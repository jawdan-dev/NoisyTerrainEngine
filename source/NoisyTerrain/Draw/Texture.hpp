#pragma once
#include <NoisyTerrain/Core/Core.hpp>

class Texture {
private:
	const char* m_file;
	bool m_loaded;

private:
	GLuint m_texture;
	uint32_t m_width, m_height;

public:
	Texture(const char* file);
	Texture(const Texture& other) = delete;
	~Texture();

public:
	const uint32_t getWidth();
	const uint32_t getHeight();
	const GLuint getTexture();

public:
	void setActiveSlot(const GLuint activeSlot);

private:
	void load();
	void loadBMP(FILE* const file);
};