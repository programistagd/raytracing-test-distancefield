#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

class RayTracer{
public:
   RayTracer();

   void event(sf::Event evt);
   void update(sf::Window& window);
   void render();
protected:
   GLuint shader;
   GLuint vertexbuffer;
   void setVec3(const char* name, glm::vec3 vec);
   struct Player{
      glm::vec3 pos;
      glm::vec3 speed;
      glm::vec3 direction,right,up;
      glm::vec2 angles;
   } player;
};

#endif
