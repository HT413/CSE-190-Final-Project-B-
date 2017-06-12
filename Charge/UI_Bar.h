#ifndef _UI_INFO_BAR_
#define _UI_INFO_BAR_

#include "Globals.h"


class UI_Bar
{
private:
	GLuint VAO, VBO[2], texID;
	bool isSelfUI;
	GLuint rectVAO[2], rectVBO[2];
	GLuint uColor, uSampler;

public:
	UI_Bar(bool, bool);
	~UI_Bar();
	void fetchUniforms(GLuint uiShader, GLuint rectShader);
	void draw(GLuint uiShader, GLuint rectShader);
	void update(float, float, float, bool);
};

class UI_Square
{
private:
	GLuint VAO, VBO[2], texID;
	GLuint uSampler, uView, uProjection, uModel;
	mat4 model;

public:
	UI_Square(float, float, int);
	~UI_Square();
	void fetchUniforms(GLuint shader);
	void draw(GLuint shader, mat4&, mat4&);
};

#endif