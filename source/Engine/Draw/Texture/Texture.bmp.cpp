#include "Texture.hpp"

void Texture::loadBMP(FILE* const file) {
	// Error macro.
#	define BitmapError(condition, ...) \
	if (condition) { \
		J_WARNING("Texture.bmp.cpp: " __VA_ARGS__); \
		return; \
	}

	// File reading headers.
#	pragma pack(push, 1)
	struct BitmapFileHeader {
		uint8_t m_identity[2];
		uint32_t m_fileSize;
		uint16_t m_reserved[2];
		uint32_t m_imageDataOffset;
	};
	struct BitmapInfoHeader {
		uint32_t m_headerSize;
		int32_t m_width, m_height;
		uint16_t m_colorPlanes;
		uint16_t m_bitsPerPixel;
		uint32_t m_compressionMethod;
		uint32_t m_imageSize;
		uint32_t m_horizontalResolution, m_verticalResolution;
		uint32_t m_colorPaletteSize, m_colorPaletteImportantColors;
	};
	struct BitmapCompression_BitFieldHeader {
		uint32_t m_bitFields[4];
	};
	struct BitmapPixelData {
		float m_r, m_g, m_b;
	};
#	pragma pack(pop)


	// Read file header.
	BitmapFileHeader fileHeader;
	BitmapError(fread(&fileHeader, sizeof(fileHeader), 1, file) < 1, "Failed to read Bitmap header.\n");

	// Data validation.
	BitmapError(*(uint16_t*)fileHeader.m_identity != ('B' | ('M' << 8)), "Bitmap Identity invalid. Recieved '%c%c' instead of the expected 'BM'.\n", (char)fileHeader.m_identity[0], (char)fileHeader.m_identity[1]);

	// Read info header.
	BitmapInfoHeader infoHeader;
	BitmapError(fread(&infoHeader, sizeof(infoHeader), 1, file) < 1, "Failed to read Bitmap header.\n");

	// Data validation.
	BitmapError(infoHeader.m_colorPlanes != 1, "Found %u color planes. Must be 1.\n", infoHeader.m_colorPlanes);
	BitmapError(infoHeader.m_width == 0 || infoHeader.m_height == 0, "Invalid texture dimesions of %i x %i found.\n", infoHeader.m_width, infoHeader.m_height);

	// Update details.
	m_width = infoHeader.m_width;
	m_height = infoHeader.m_height;

	// Get image data.
	const size_t rowSize = ((infoHeader.m_bitsPerPixel * m_width) / 32) * 4;
	uint8_t* const rowData = new uint8_t[rowSize];
	BitmapPixelData* const imageData = new BitmapPixelData[m_width * m_height];

	// Read image.
	switch (infoHeader.m_compressionMethod) {
		default: BitmapError(true, "Unsupported compression method '%lu'\n", infoHeader.m_compressionMethod);

		// Uncompressed.
		case 0: {
			switch (infoHeader.m_bitsPerPixel) {
				default: BitmapError(true, "Unsupported bits per pixel '%lu' with compression method '%lu'.\n", infoHeader.m_bitsPerPixel, infoHeader.m_compressionMethod)
			}
		}

		// BitFields.
		case 3:
		case 6: {
			// Read bit fields.
			BitmapCompression_BitFieldHeader bitFieldHeader = { 0, 0, 0, 0 };
			switch (infoHeader.m_headerSize - sizeof(BitmapInfoHeader)) {
				default: BitmapError(true, "Unsupported bitfield header size: %lu\n", infoHeader.m_headerSize - sizeof(BitmapInfoHeader));
				case 16: BitmapError(fread(&bitFieldHeader, sizeof(uint32_t), 4, file) < 1, "Failed to read Bit Field header.\n"); break;
			}

			// Find BitField bit offsets.
			uint8_t rOffset = 0, gOffset = 0, bOffset = 0, aOffset = 0;
			while (bitFieldHeader.m_bitFields[0] && !(bitFieldHeader.m_bitFields[0] & (0b1 << rOffset))) rOffset++;
			while (bitFieldHeader.m_bitFields[1] && !(bitFieldHeader.m_bitFields[1] & (0b1 << gOffset))) gOffset++;
			while (bitFieldHeader.m_bitFields[2] && !(bitFieldHeader.m_bitFields[2] & (0b1 << bOffset))) bOffset++;
			while (bitFieldHeader.m_bitFields[3] && !(bitFieldHeader.m_bitFields[3] & (0b1 << aOffset))) aOffset++;

			// Seek data start.
			fseek(file, fileHeader.m_imageDataOffset, SEEK_SET);

			// Read data.
			for (int32_t y = 0; y < m_height; y++) {
				// Read file.
				BitmapError(fread(rowData, sizeof(uint8_t), rowSize, file) < rowSize, "File ended unexpectedly.\n");

				// Interpret file.
				uint32_t byteOffset = 0, bitOffset = 0;
				for (int32_t x = 0; x < m_width; x++) {
					// Update byte offset.
					if (bitOffset >= 8) {
						byteOffset += bitOffset / 8;
						bitOffset %= 8;
					}

					// Get data.
					BitmapError(bitOffset != 0, "BitField bit-wise reading not supported.\n");
					const uint32_t data = *(uint32_t*)(rowData + byteOffset);

					// Interpret data.
					const float
						r = bitFieldHeader.m_bitFields[0] ? ((float)((data & bitFieldHeader.m_bitFields[0]) >> rOffset) / (float)(bitFieldHeader.m_bitFields[0] >> rOffset)) : 0,
						g = bitFieldHeader.m_bitFields[1] ? ((float)((data & bitFieldHeader.m_bitFields[1]) >> gOffset) / (float)(bitFieldHeader.m_bitFields[1] >> gOffset)) : 0,
						b = bitFieldHeader.m_bitFields[2] ? ((float)((data & bitFieldHeader.m_bitFields[2]) >> bOffset) / (float)(bitFieldHeader.m_bitFields[2] >> bOffset)) : 0,
						a = bitFieldHeader.m_bitFields[2] ? ((float)((data & bitFieldHeader.m_bitFields[3]) >> aOffset) / (float)(bitFieldHeader.m_bitFields[3] >> aOffset)) : 0;

					// Set pixel.
					BitmapPixelData& pixel = imageData[x + ((infoHeader.m_height - (y + 1)) * m_width)];
					pixel.m_r = r;
					pixel.m_g = g;
					pixel.m_b = b;

					// Update offset.
					bitOffset += infoHeader.m_bitsPerPixel;
				}
			}
		} break;
	}

	// Remember the previously bound texture.
	GLuint lastBoundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&lastBoundTexture);

	// Generate GL texture.
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load data.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup.
	glBindTexture(GL_TEXTURE_2D, lastBoundTexture);
	delete[] rowData;
	delete[] imageData;
#	undef BitmapError
}
