#include "ModelMesh.hpp"

template <typename T>
inline void ModelMesh::set(const String& staticName, const List<T>& data) {
	set(staticName, data.data(), data.size(), sizeof(T));
}
