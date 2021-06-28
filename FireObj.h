#pragma once
#include "Object3D.h"
class FireObj :
	public Object3D
{
public:
	FireObj();
	~FireObj();

	void render(int deltaTime);
	void setShader(Shader* p_shader);


	void setTexture2(GLuint id) { mTexture2 = id; };
	GLuint getTexture2() { return mTexture2; };

	int getUniformCamPos() { return mUniformCamPos; };
	int getUniformLightColor() { return mUniformLightColor; };
	int getUniformLightPos() { return mUniformLightPos; };


private:
	GLuint mTexture2;

	float mCurrentOffseTime;
	int	  mUniformTex;
	int	  mUniformTex2;

	int   mUniformOffsetTime;
	int   mUniformCamPos;
	int	  mUniformLightColor;
	int   mUniformLightPos;
};

