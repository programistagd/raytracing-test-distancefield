
#include <GL/glew.h>
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>

#include "RayTracer.hpp"
#include "Volumes.hpp"

sf::Font globalFont;
const int WIDTH = 1300;
const int HEIGHT = 720;

template<class T> sf::String toString(T f){
	std::ostringstream os;
	os<<f;
	return os.str();
}

bool running = true;
std::string globalError;

int main(int argc, char* argv[]) {
	std::cout << "Distancefield Ray Tracer\n";
	try{
	if(!globalFont.loadFromFile("FreeSans.ttf")){
		throw std::string("Font!");
	}

	sf::ContextSettings ctx;
	ctx.majorVersion = 3;
	ctx.minorVersion = 3;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Raytracer", sf::Style::Default, ctx);
	//window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr<<"Error: "<<glewGetErrorString(err)<<std::endl;
	}
	std::cout<<"Status: Using GLEW "<<glewGetString(GLEW_VERSION)<<std::endl;
	{
		sf::ContextSettings settings = window.getSettings();
		std::cout<<"OpenGL info\n";
		std::cout << "version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;
		std::cout << "depth bits: " << settings.depthBits << std::endl;
		std::cout << "stencil bits: " << settings.stencilBits << std::endl;
		std::cout << "antialiasing level: " << settings.antialiasingLevel << std::endl;
	}

	///initialize game loop
	float dt = 1.f/60.f;
	float accumulator=0.f;
	bool shouldDraw=true;
	sf::Clock clock;
	sf::Clock renderTimer;
	int latency=0;
	sf::Text latencyText;
	latencyText.setFont(globalFont);
	latencyText.setColor(sf::Color::White);
	latencyText.setCharacterSize(20);
	latencyText.setPosition(10, 10);
	sf::RectangleShape background(sf::Vector2f(60,25));
	background.setPosition(5, 9);
	background.setFillColor(sf::Color::Black);

	glClearColor(0.f,0.f,0.3f,1.0f);

	//initialize components
	RayTracer raytracer;
	{
		//Prepare volume
		std::cout << "Computing SDF\n";//TODO maybe load from file??
		Volumes::cuboid(raytracer, 128, &Volumes::add, glm::vec3(40, 50, 30), glm::vec3(18, 18, 18),glm::vec3(1,0,0));
		Volumes::sphere(raytracer, 128, &Volumes::add, glm::vec3(64, 64, 64), 24.0, glm::vec3(0, 1, 0));
		Volumes::sphere(raytracer, 128, &Volumes::remove, glm::vec3(80, 80, 64), 20.0, glm::vec3(1, 0, 1));//TODO possibly dont set color while removing?
		Volumes::sphere(raytracer, 128, &Volumes::add, glm::vec3(20, 20, 20), 8.0, glm::vec3(0.7, 0, 0.5),1);
		raytracer.refresh();
		raytracer.setViewport(WIDTH, HEIGHT);
	}
	std::cout << "Starting rendering\n";

	//start loop
	while (running && window.isOpen()) {
		accumulator+=clock.getElapsedTime().asSeconds();
		clock.restart();
		if(accumulator>10.0f*dt) accumulator=10.0f*dt;
		while(accumulator>=dt){
			accumulator-=dt;
			sf::Event event;
			while (window.pollEvent(event))
			{
				// "close requested" event: we close the window
				if (event.type == sf::Event::Closed){
					window.close();
				}
				else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
					window.close();
				}
				else if (event.type == sf::Event::Resized)
				{
					// adjust the viewport when the window is resized
					glViewport(0, 0, event.size.width, event.size.height);
					raytracer.setViewport(event.size.width, event.size.height);
				}
				raytracer.event(event);
			}

			raytracer.update(window);
			shouldDraw = true;
		}
		if(shouldDraw){
			renderTimer.restart();//measure only rendering time
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render
			raytracer.render();

			window.pushGLStates();
			latencyText.setString(toString(latency)+"ms");

			window.draw(background);
			window.draw(latencyText);
			window.popGLStates();
			window.display();
			latency = renderTimer.getElapsedTime().asMilliseconds();
			//renderTimer.restart();//measure absolute FPS
			shouldDraw = false;
		}else{
			sf::sleep(sf::milliseconds(1));
		}
	}

	if (window.isOpen())	window.close();

	}
	catch(std::string err){
		std::cerr<<"Error: "<<err<<std::endl;
	}
	/*catch(...){
		std::cerr<<"Unknown exception"<<std::endl;
	}*/
	std::cout<<"Quitted gracefully\n";
}
