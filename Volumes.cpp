#include <algorithm>
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

void Volumes::sphere(float* voxels, int size, std::function<void(float*,int,int,int,int,float)> method, glm::vec3 center, float radius){
   FOR3(size){
      method(voxels,size,x,y,z,
		  (glm::length(glm::vec3(x,y,z)-center)-radius));
   }
}

void Volumes::cuboid(float* voxels, int size, std::function<void(float*, int, int, int, int, float)> method, glm::vec3 center, glm::vec3 extents){
	FOR3(size){
		glm::vec3 d = glm::abs(glm::vec3(x,y,z) - center) - extents;
		method(voxels, size, x, y, z,
			(glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) +
			glm::length(glm::max(d, 0.0f))));
	}
}