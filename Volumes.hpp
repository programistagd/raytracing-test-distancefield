#ifndef VOLUMES_HPP
#define VOLUMES_HPP
#include <functional>
#include <glm/glm.hpp>
#include "RayTracer.hpp"
namespace Volumes{
	//Operations
	void replace(float* voxels, int size, int x, int y, int z, float value);
	void add(float* voxels, int size, int x, int y, int z, float value);
	void remove(float* voxels, int size, int x, int y, int z, float value);

	//Shapes
	void sphere(RayTracer& rt, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, float radius, glm::vec3 color);
	void cuboid(RayTracer& rt, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, glm::vec3 extents, glm::vec3 color);

}
#endif
