#include <GL/glew.h>
#include "RayTracer.hpp"
#define  GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#include <iostream>
inline std::string errToStr(GLenum err){
	switch (err){
	case GL_INVALID_ENUM:	return "Invalid enum";
	case GL_INVALID_VALUE:	return "Invalid value";
	case GL_INVALID_OPERATION:	return "Invalid operation";
	default: return "Unknown error";
	}
}
#define DEBUG { GLenum err = glGetError(); if(err!=GL_NO_ERROR) std::cerr<<"OpenGL("<<__LINE__<<") - "<<errToStr(err)<<std::endl;}

RayTracer::RayTracer(){
   voxels = new float[128*128*128];
   for (int i = 0; i < 128 * 128 * 128; ++i) voxels[i] = 1000000.0f;
   colors = new Color[128 * 128 * 128];
   for (int i = 0; i < 128 * 128 * 128; ++i) colors[i] = { 0, 0, 0 };
   features = new uint8_t[128 * 128 * 128];
   for (int i = 0; i < 128 * 128 * 128; ++i) features[i] = 0;

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
   glGenTextures(1, &sdfTexture);
   glBindTexture(GL_TEXTURE_3D, sdfTexture);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

   glGenTextures(1, &colorTexture);
   glBindTexture(GL_TEXTURE_3D, colorTexture);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glGenTextures(1, &featureTexture);
   glBindTexture(GL_TEXTURE_3D, featureTexture);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   refresh();//TODO may not be called but a call has to be guaranteed before first render call (probably)
   
}

void RayTracer::refresh(){
   glEnable(GL_TEXTURE_3D);

   glBindTexture(GL_TEXTURE_3D, sdfTexture);
   glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 128, 128, 128, 0, GL_RED,GL_FLOAT, voxels);
   DEBUG

   glBindTexture(GL_TEXTURE_3D, colorTexture);
   glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 128, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, colors);
   DEBUG;

   glBindTexture(GL_TEXTURE_3D, featureTexture);
   glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 128, 128, 128, 0, GL_RED, GL_UNSIGNED_BYTE, features);
   DEBUG
   
   glBindTexture(GL_TEXTURE_3D, 0);
   glDisable(GL_TEXTURE_3D);
}

void RayTracer::setVec3(const char* name, glm::vec3 vec){
   GLuint id=glGetUniformLocation(shader, name);
   glUniform3f(id,vec.x,vec.y,vec.z);
}

void RayTracer::setViewport(int x, int y){
	float dim = (float)glm::max(x, y);//TODO consider moving viewport into center
	viewport = glm::vec2(dim,dim);
}

void RayTracer::render(){
   glUseProgram(shader);
   setVec3("eyePos", player.pos);
   setVec3("lightPosition", light1);
   {
	   GLuint id = glGetUniformLocation(shader, "WindowSize");
	   glUniform2f(id, viewport.x, viewport.y);
   }
 
   glm::mat4 viewMatrix = glm::lookAt(
      glm::vec3(0,0,0),//player.pos,           // Camera is here
      player.direction, // and looks here : at the same position, plus "direction"
      player.up                  // Head is up (set to 0,-1,0 to look upside-down)
   );


   GLuint viewmatid=glGetUniformLocation(shader, "matrix");
   glUniformMatrix4fv(viewmatid, 1, GL_FALSE, &viewMatrix[0][0]);

   GLuint loc = glGetUniformLocation(shader, "VolumeTexture");
   glUniform1i(loc, 0);
   loc = glGetUniformLocation(shader, "ColorsTexture");
   glUniform1i(loc, 1);
   loc = glGetUniformLocation(shader, "FeatureTexture");
   glUniform1i(loc, 2);

   glEnable(GL_TEXTURE_3D);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_3D, sdfTexture);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_3D, colorTexture);

   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_3D, featureTexture);

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
   glActiveTexture(GL_TEXTURE0);
   glDisable(GL_TEXTURE_3D);
   DEBUG

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
		 case sf::Keyboard::L:
			 light1 = player.pos;
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
