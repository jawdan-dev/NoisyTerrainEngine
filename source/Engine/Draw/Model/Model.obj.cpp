#include "Model.hpp"

void Model::loadOBJ(FILE* const file, ModelMesh& mesh) {
	// Variables.
	List<Vector3> vertexPositions;
	List<uint32_t> vertexIndices;

	// Interpret file.
	char c;
	String line;
	while (!feof(file)) {
		// Read line.
		line = "";
		while (!feof(file) && c != '\n') {
			const size_t r = fread(&c, sizeof(c), 1, file);
			// Add to line.
			if (r <= 0) break;
			else if (c != '\r' && c != '\n') line += c;
		}
		c = '\0';

		// Make sure line has a starting character.
		if (line.size() < 2 || line[1] != ' ') continue;

		// Handle character.
		switch (line[0]) {
			case 'v': {
				// Read vertex position.
				Vector3 position;
				sscanf(line.c_str(), "v %f %f %f", &position.x(), &position.y(), &position.z());

				// Add position.
				vertexPositions.emplace_back(position);
			} break;
			case 'f': {
				// Support check.
				if (line.find("/") != String::npos) J_ERROR_EXIT("Model.obj.cpp: Non-simple objs not supported.\n");

				// Read face integers.
				uint32_t indices[3];
				sscanf(line.c_str(), "f %u %u %u", &indices[0], &indices[1], &indices[2]);

				// Add indices.
				vertexIndices.emplace_back(indices[0] - 1);
				vertexIndices.emplace_back(indices[2] - 1);
				vertexIndices.emplace_back(indices[1] - 1);
			} break;
		}
	}

	// Set mesh data.
	if (vertexPositions.size() > 0 && vertexIndices.size() > 0) {
		mesh.set("v_position", vertexPositions);
		mesh.setIndices(vertexIndices);
	}
}