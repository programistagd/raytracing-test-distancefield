#include <GL/glew.h>
#include "RayTracer.hpp"
#define  GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

RayTracer::RayTracer(){
   voxels = new float[128*128*128];
   for (int i = 0; i < 128 * 128 * 128; ++i) voxels[i] = 1000000.0f;

   shader = LoadShaders("vertex.glsl","fragment.glsl");
   player.direction = glm::vec3(cos(player.angles.y)*sin(player.angles.x),sin(player.angles.y),cos(player.angles.y)*cos(player.angles.x));
   player.right = glm::vec3(sin(player.angles.x - 3.14f/2.0f),0,cos(player.angles.x - 3.14f/2.0f));
   player.up = glm::cross( player.right, player.direction );

   GLuint VertexArrayID;
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);
   // An array of 3 vectors which represents 3 vertices
   static const GLfloat g_vertex_buffer_data[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,

      -1.0f, 1.0f, 0.0f,
      1.0f,  1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
   };
   glGenBuffers(1, &vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, 2*sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   //prep texture
   glEnable(GL_TEXTURE_3D);
   glGenTextures(1, &texture3d);
   glBindTexture(GL_TEXTURE_3D, texture3d);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
   refresh();//TODO may not be called but a call has to be guaranteed before first render call (probably)
   
}

void RayTracer::refresh(){
   glEnable(GL_TEXTURE_3D);
   glBindTexture(GL_TEXTURE_3D, texture3d);
   glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 128, 128, 128, 0, GL_RED,GL_FLOAT, voxels);
   glBindTexture(GL_TEXTURE_3D, 0);
   glDisable(GL_TEXTURE_3D);
}

void RayTracer::setVec3(const char* name, glm::vec3 vec){
   GLuint id=glGetUniformLocation(shader, name);
   glUniform3f(id,vec.x,vec.y,vec.z);
}

#include <iostream>
void RayTracer::render(){
   glUseProgram(shader);
   setVec3("eyePos",player.pos);

   glm::mat4 viewMatrix = glm::lookAt(
      glm::vec3(0,0,0),//player.pos,           // Camera is here
      player.direction, // and looks here : at the same position, plus "direction"
      player.up                  // Head is up (set to 0,-1,0 to look upside-down)
   );


   GLuint viewmatid=glGetUniformLocation(shader, "matrix");
   glUniformMatrix4fv(viewmatid, 1, GL_FALSE, &viewMatrix[0][0]);

   glEnable(GL_TEXTURE_3D);
   glBindTexture(GL_TEXTURE_3D, texture3d);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
   glVertexAttribPointer(
      0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
   );
   glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
   glDisableVertexAttribArray(0);
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glDisable(GL_TEXTURE_3D);

   sf::Shader::bind(0);
}

void RayTracer::update(sf::Window& window){
   float dt = 1.0f/60.0f;
   sf::Vector2i mouse = sf::Mouse::getPosition(window);
   sf::Mouse::setPosition(sf::Vector2i(800/2,600/2),window);
   player.angles.x-=1.0f*dt*float(800/2-mouse.x);
   player.angles.y+=1.0f*dt*float(600/2-mouse.y);
   player.direction = glm::vec3(cos(player.angles.y)*sin(player.angles.x),sin(player.angles.y),cos(player.angles.y)*cos(player.angles.x));
   player.right = glm::vec3(sin(player.angles.x - 3.14f/2.0f),0,cos(player.angles.x - 3.14f/2.0f));
   player.up = glm::cross( player.right, player.direction );

   player.pos += player.direction*(player.speedFactor*player.speed.y*dt)+player.right*(player.speedFactor*player.speed.x*dt);
   
}

void RayTracer::event(sf::Event evt){
   if(evt.type==sf::Event::KeyPressed){
      switch(evt.key.code){
         case sf::Keyboard::W:
            player.speed.y=1.0;
         break;
         case sf::Keyboard::S:
         player.speed.y=-1.0;
         break;
         case sf::Keyboard::D:
         player.speed.x=1.0;
         break;
         case sf::Keyboard::A:
         player.speed.x=-1.0;
         break;
         case sf::Keyboard::LShift:
         player.speedFactor=player.fastSpeed;
         break;
      }
   }
   else if(evt.type==sf::Event::KeyReleased){
      switch(evt.key.code){
         case sf::Keyboard::W:
         case sf::Keyboard::S:
         player.speed.y=0;
         break;
         case sf::Keyboard::D:
         case sf::Keyboard::A:
         player.speed.x=0;
         break;
         case sf::Keyboard::LShift:
         player.speedFactor=player.normalSpeed;
         break;
      }
   }
}

RayTracer::~RayTracer(){
   delete [] voxels;
}
