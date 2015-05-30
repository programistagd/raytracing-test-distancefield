#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <SFML/Graphics.hpp>

class RayTracer{
public:
   RayTracer();

   void update();
   void render();
protected:
   GLuint shader;
   sf::Vector3f playerPos;
};

#endif
