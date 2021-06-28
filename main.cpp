//Using SDL, SDL OpenGL, GLEW, standard IO, and strings
#include <string>
#include <iostream>
#include <fstream>

#include <SDL.h>
#include <SDL_image.h>
#include "gl/glew.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "SDL_opengl.h"

#include "Shader.h"
#include "Camera3D.h"
#include "Object3D.h"
#include "FireFrameBuffers.h"
#include "FireObj.h"

#include "global_defines.h"


//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
void initGL();

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

void waitTime();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

int UniformViewM;
int UniformProjectionM;
int UniformDistord;
float mCurrentOffsetWave;

Shader FireShader;
Shader TextureMatrixColorShader;

Camera3D MyCamera;

//objeto del fuego
FireObj MyFire;

//objetos de la escena
Object3D MyCube;
Object3D backgroundPlane;

// FBO para el fuego
FireFrameBuffers* FireFBO;
// Clip Plane
glm::vec4 clipPlane;

int lastTime = 0, currentTime, deltaTime;
float msFrame = 1 / (FPS / 1000.0f);

enum direction {
	DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT,
	_DIR_LAST_
};
bool		movement[_DIR_LAST_];

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		gWindow = SDL_CreateWindow("SDL + OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize GLEW
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if (glewError != GLEW_OK)
				{
					printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
				}

				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				MyCamera.init();
				initGL();
			}
		}
	}

	// Init Movements
	for (int i = 0; i < _DIR_LAST_; i++) {
		movement[i] = false;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	return success;
}

void initGL()
{
	FireShader.init("Fire");
	TextureMatrixColorShader.init("TextureMatrixColor");

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Zbuffer enable
	glEnable(GL_DEPTH_TEST);

	// Back face culling
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	//Generate Objects
	MyFire.loadObjFromDisk("Assets/FirePlane.txt");
	MyFire.setShader(&FireShader);
	MyFire.setPosition(glm::vec3(0.0, 0.0, -2.0));

	//Pintamos un cubo para que seaprecie mejor el reflejo
	MyCube.loadObjFromDisk("Assets/Cube.txt");
	MyCube.loadTextureFromDisk("Assets/textures/texture.png");
	MyCube.setShader(&TextureMatrixColorShader);
	MyCube.setPosition(glm::vec3(-1.3f, 1.0f, 1.5f));


	backgroundPlane.loadObjFromDisk("Assets/Pool.txt");
	backgroundPlane.setShader(&TextureMatrixColorShader);
	backgroundPlane.loadTextureFromDisk("Assets/textures/floor.jpg");
	backgroundPlane.setPosition(glm::vec3(0.0, 0.0, 0.0));
	// Create Frame Buffer Objects (FBO)
	FireFBO = new FireFrameBuffers;
}

void update()
{
	// Move Camera
	MyCamera.update(movement);
}

void renderFire()
{
	//Bind program
	FireShader.Use();
	UniformViewM = glGetUniformLocation(FireShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(FireShader.getID(), "projection");

	MyFire.setTexture(FireFBO->getReflectionTexture());
	MyFire.setTexture2(FireFBO->getBackgroundTexture());

	//Sets Projection Matrix
	MyCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	//Sets View Matrix (Camera)
	MyCamera.setUniformViewMatrix(UniformViewM);

	// Draw objects
	MyFire.render(deltaTime);

}

void renderScene() {
	//Clear color buffer & Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind program
	TextureMatrixColorShader.Use();
	UniformViewM = glGetUniformLocation(TextureMatrixColorShader.getID(), "view");
	UniformProjectionM = glGetUniformLocation(TextureMatrixColorShader.getID(), "projection");

	//Sets Projection Matrix
	MyCamera.setUniformProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, UniformProjectionM);
	//Sets View Matrix (Camera)
	MyCamera.setUniformViewMatrix(UniformViewM);


	MyCube.render();

	backgroundPlane.render();
}

void render() {


	// Reflection texture render
	FireFBO->bindReflectionFrameBuffer();
	// Recalculate Camera position for Render Reflection
	float distance = 2.0f * (MyCamera.getCameraPos().z - MyFire.getPosition().z);
	glm::vec3 fire = distance * MyCamera.getCameraFront();
	MyCamera.setCameraPos(MyCamera.getCameraPos().x , MyCamera.getCameraPos().y , MyCamera.getCameraPos().z + fire.z);
	MyCamera.invertYaw();
	MyCamera.update();

	renderScene();

	// Camera Recovery position
	MyCamera.setCameraPos(MyCamera.getCameraPos().x , MyCamera.getCameraPos().y , MyCamera.getCameraPos().z - fire.z);
	MyCamera.invertYaw();
	MyCamera.update();

	// Refraction texture render
	FireFBO->bindBackgroundFrameBuffer();

	renderScene();

	FireFBO->unbindCurrentFrameBuffer();

	renderScene();

	renderFire();
}

void close()
{
	//Deallocate program
	FireShader.deleteProgram();
	TextureMatrixColorShader.deleteProgram();

	//Destroy data in GPU
	MyFire.clearGPU();

	MyCube.clearGPU();

	backgroundPlane.clearGPU();
	// Clear FBO fire
	FireFBO->cleanUp();

	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					// Camera Move setting
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W)) {
						movement[DIR_UP] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S)) {
						movement[DIR_DOWN] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A)) {
						movement[DIR_LEFT] = true;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D)) {
						movement[DIR_RIGHT] = true;
					}
				}
				if (e.type == SDL_KEYUP) {
					// Camera Move Stop
					if ((e.key.keysym.scancode == SDL_SCANCODE_UP) ||
						(e.key.keysym.scancode == SDL_SCANCODE_W)) {
						movement[DIR_UP] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_DOWN) ||
						(e.key.keysym.scancode == SDL_SCANCODE_S)) {
						movement[DIR_DOWN] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_LEFT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_A)) {
						movement[DIR_LEFT] = false;
					}
					if ((e.key.keysym.scancode == SDL_SCANCODE_RIGHT) ||
						(e.key.keysym.scancode == SDL_SCANCODE_D)) {
						movement[DIR_RIGHT] = false;
					}
				}
				// Capture Mouse
				if (e.type == SDL_MOUSEMOTION) {
					MyCamera.setHeadTurn(e.motion.xrel, -e.motion.yrel);
				}
				// Capture Mouse wheel for Zoom
				if (e.type == SDL_MOUSEWHEEL) {
					MyCamera.setAddZoom(e.wheel.y);
				}
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			//Update
			update();
			//Render
			render();

			//Update screen
			SDL_GL_SwapWindow(gWindow);
			waitTime();
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

void waitTime() {
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - lastTime;
	if (deltaTime < (int)msFrame) {
		SDL_Delay((int)msFrame - deltaTime);
	}
	lastTime = currentTime;
}