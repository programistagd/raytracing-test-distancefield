#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>

struct Color{
	uint8_t r, g, b;
	inline operator bool(){
		if (r == 255 && g == 0 && b == 255) return false;
		return true;
	}
};

class RayTracer{
public:
   RayTracer();

   void event(sf::Event evt);
   void update(sf::Window& window);
   void render();

   float * voxels;
   Color * colors;
   uint8_t * features;
   void refresh();

   void setViewport(int x, int y);

   ~RayTracer();
protected:
   GLuint sdfTexture;
   GLuint colorTexture;
   GLuint featureTexture;
   GLuint shader;
   GLuint vertexbuffer;
   void setVec3(const char* name, glm::vec3 vec);
   struct Player{
      const float normalSpeed=4.f;
      const float fastSpeed=16.f;
      glm::vec3 pos=glm::vec3(-5,-5,-5);
      glm::vec3 speed;
      float speedFactor=normalSpeed;
      glm::vec3 direction,right,up;
      glm::vec2 angles;
   } player;
   glm::vec2 viewport;
   glm::vec3 light1 = glm::vec3(-40, -50, 100);
};

#endif
