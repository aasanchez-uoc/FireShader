#pragma once
#include "gl/glew.h"

#define REFLECTION_WIDTH  320
#define REFLECTION_HEIGHT  180

#define BACKGROUND_WIDTH  1280
#define BACKGROUND_HEIGHT  720


class FireFrameBuffers
{
	
public:
	FireFrameBuffers();
	~FireFrameBuffers();
	void cleanUp();
	void bindReflectionFrameBuffer();
	void bindBackgroundFrameBuffer();
	void unbindCurrentFrameBuffer();
	int getReflectionTexture();
	int getBackgroundTexture();

private:
	void initialiseReflectionFrameBuffer();
	void initialiseBackgroundFrameBuffer();
	void bindFrameBuffer(int frameBuffer, int width, int height);
	GLuint createFrameBuffer();
	GLuint createTextureAttachment(int width, int height);


	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint backgroundFrameBuffer;
	GLuint backgroundTexture;

};

