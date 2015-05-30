#include <GL/glew.h>
#include "RayTracer.hpp"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

RayTracer::RayTracer(){
   shader = LoadShaders("vertex.glsl","fragment.glsl");
}


void RayTracer::render(){

   glUseProgram(shader);
   //GLuint eyepos=glGetUniform
   glLoadIdentity();
   glBegin(GL_QUADS);                      // Draw A Quad
   glVertex3f( -400.0f,  -300.0f, 0.0f);              // Top Left
   glVertex3f( 400.0f,   -300.0f, 0.0f);              // Top Right
   glVertex3f( 400.0f,   300.0f,  0.0f);              // Bottom Right
   glVertex3f( -400.0f,  300.0f,  0.0f);              // Bottom Left
   glEnd();
   sf::Shader::bind(0);
}

void RayTracer::update(){

}
