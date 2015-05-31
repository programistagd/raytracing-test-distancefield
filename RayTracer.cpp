#include <GL/glew.h>
#include "RayTracer.hpp"
#define  GLM_FORCE_RADIANS
#include <glm/ext.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

RayTracer::RayTracer(){
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

}

void RayTracer::setVec3(const char* name, glm::vec3 vec){
   GLuint id=glGetUniformLocation(shader, name);
   glUniform3f(id,vec.x,vec.y,vec.z);
}

#include <iostream>
void RayTracer::render(){

   glUseProgram(shader);
   //setVec3("eyePos",player.pos);
   /*setVec3("direction",player.direction);
   setVec3("up",player.up);
   setVec3("right",player.right);*/
   /*std::cout<<player.pos.x<<" "<<player.pos.y<<" "<<player.pos.z<<" "<<std::endl;
   std::cout<<player.direction.x<<" "<<player.direction.y<<" "<<player.direction.z<<" "<<std::endl;
   std::cout<<player.up.x<<" "<<player.up.y<<" "<<player.up.z<<" "<<std::endl;
   std::cout<<std::endl;*/
   glm::mat4 viewMatrix = glm::lookAt(
      player.pos,           // Camera is here
      player.pos+player.direction, // and looks here : at the same position, plus "direction"
      player.up                  // Head is up (set to 0,-1,0 to look upside-down)
   );
   const float FoV = 60.0f;
   glm::mat4 projection = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
   glm::mat4 MVP = projection * viewMatrix;
   glm::mat4 matrix = glm::inverse(MVP);

   //GLuint viewmatid=glGetUniformLocation(shader, "matrix");
   //glUniformMatrix4fv(viewmatid, 1, GL_FALSE, &matrix[0][0]);

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

   sf::Shader::bind(0);
}

void RayTracer::update(sf::Window& window){
   float dt = 1.0f/60.0f;
   sf::Vector2i mouse = sf::Mouse::getPosition(window);
   sf::Mouse::setPosition(sf::Vector2i(800/2,600/2),window);
   player.angles.x+=1.0*dt*float(800/2-mouse.x);
   player.angles.y+=1.0*dt*float(600/2-mouse.y);
   player.direction = glm::vec3(cos(player.angles.y)*sin(player.angles.x),sin(player.angles.y),cos(player.angles.y)*cos(player.angles.x));
   player.right = glm::vec3(sin(player.angles.x - 3.14f/2.0f),0,cos(player.angles.x - 3.14f/2.0f));
   player.up = glm::cross( player.right, player.direction );

   player.pos += player.direction*player.speed.y*dt*4.0f-player.right*player.speed.x*dt*4.0f;
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
      }
   }
}
