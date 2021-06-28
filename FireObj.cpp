#include "FireObj.h"

#define FIRE_SPEED 0.5f;
FireObj::FireObj()
{
	mCurrentOffseTime = 0.0f;
}


FireObj::~FireObj()
{
}

void FireObj::render(int deltaTime)
{
	//Sets texture
	if (mTexture != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glUniform1i(mUniformTex, 0);
	}
	if (mTexture2 != -1) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mTexture2);
		glUniform1i(mUniformTex2, 1);
	}
	mCurrentOffseTime += deltaTime * 0.001f * FIRE_SPEED;

	glUniform1f(mUniformOffsetTime, mCurrentOffseTime);
	//Set VAO
	prepareVAO();
	//Draw VAO
	glDrawElements(GL_TRIANGLES, mIndexData.size(), GL_UNSIGNED_INT, NULL);
	//Diable VAO
	diableVAO();
}

void FireObj::setShader(Shader * p_shader)
{
	mShaderforDraw = p_shader;
	mUniformModelM = glGetUniformLocation(mShaderforDraw->getID(), "model");
	mUniformTex = glGetUniformLocation(mShaderforDraw->getID(), "texture1");
	mUniformTex2 = glGetUniformLocation(mShaderforDraw->getID(), "texture2");
	mUniformOffsetTime = glGetUniformLocation(mShaderforDraw->getID(), "timeOffset");
	mUniformCamPos = glGetUniformLocation(mShaderforDraw->getID(), "camPos");
	mUniformLightColor = glGetUniformLocation(mShaderforDraw->getID(), "lightcolor");
	mUniformLightPos = glGetUniformLocation(mShaderforDraw->getID(), "lightPos");
}
