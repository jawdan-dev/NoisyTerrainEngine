#include "VoxelInt.hpp"

const VoxelInt mod(const VoxelInt a, const VoxelInt b) {
	const VoxelInt x = a % b;
	return x < 0 ? x + b : x;
}