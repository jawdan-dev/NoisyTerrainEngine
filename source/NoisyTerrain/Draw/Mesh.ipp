#include "Mesh.hpp"

template <typename T>
inline void Mesh::set(const String& staticName, const List<T>& data) {
	set(staticName, data.data(), data.size(), sizeof(T));
}
