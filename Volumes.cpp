#include <algorithm>
#include "RayTracer.hpp"
#include "Volumes.hpp"

#define FOR3(k) for(int x=0;x<k;++x)for(int y=0;y<k;++y)for(int z=0;z<k;++z)

void Volumes::replace(float* voxels, int size, int x, int y, int z, float value){
	voxels[(x + (y + z*size)*size)] = value;
}

void Volumes::add(float* voxels, int size, int x, int y, int z, float value){
	voxels[(x + (y + z*size)*size)] = std::min(voxels[(x + (y + z*size)*size)],value);
}

void Volumes::remove(float* voxels, int size, int x, int y, int z, float value){
	voxels[(x + (y + z*size)*size)] = std::max(voxels[(x + (y + z*size)*size)], -value);
}

Color convertColor(glm::vec3 color){
	return{ color.r * 255.0f, color.g * 255.0f, color.b * 255.0f };
}

void Volumes::sphere(RayTracer& rt, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, float radius, glm::vec3 color){
	Color c = convertColor(color);
   FOR3(size){
	   float value = glm::length(glm::vec3(x, y, z) - center) - radius;
      method(rt.voxels,size,x,y,z,
		  (value));
	  if (value <= 1.5 && c)	rt.colors[(x + (y + z*size)*size)] = c;
   }
}

void Volumes::cuboid(RayTracer& rt, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, glm::vec3 extents, glm::vec3 color){
	Color c = convertColor(color);
	FOR3(size){
		glm::vec3 d = glm::abs(glm::vec3(x, y, z) - center) - extents;
		float value = glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) +
			glm::length(glm::max(d, 0.0f));
		method(rt.voxels, size, x, y, z,
			(value));
		if (value <= 0.1 && c) rt.colors[(x + (y + z*size)*size)] = c;
	}
}