#include "Volumes.hpp"

#define FOR3(k) for(int x=0;x<k;++x)for(int y=0;y<k;++y)for(int z=0;z<k;++z)
#define INDEX(x,y,z) (x+(y+z*size)*size)

void Volumes::sphere(float* voxels, int size, glm::vec3 center, float radius){
   FOR3(size){
      voxels[(x+(y+z*size)*size)]=(glm::length(glm::vec3(x,y,z)-center)-radius);
   }
}
