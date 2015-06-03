#ifndef VOLUMES_HPP
#define VOLUMES_HPP
#include <functional>
#include <glm/glm.hpp>
namespace Volumes{
	//Operations
	void replace(float* voxels, int size, int x, int y, int z, float value);
	void add(float* voxels, int size, int x, int y, int z, float value);
	void remove(float* voxels, int size, int x, int y, int z, float value);

	//Shapes
	void sphere(float* voxels, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, float radius);
}
#endif
