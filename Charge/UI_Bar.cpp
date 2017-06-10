#include "UI_Bar.h"
#include "Texture.h"

bool isGameOver = false;

const GLfloat texCoords[12] = {0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
							   0.f, 1.f, 1.f, 0.f, 0.f, 0.f};

UI_Bar::UI_Bar(bool s, bool l)
{
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(2, rectVAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, rectVBO);
	
	isSelfUI = s;
	GLfloat uiRect[12] = {l? -.73f : -.67f, s? -1.f : .62f, 
		l? .67f : .73f, s? -1.f : .62f, 
		l ? .67f : .73f, s? -.56f : 1.f,
		l? -.73f : -.67f, s? -1.f : .62f, 
		l ? .67f : .73f, s? -.56f : 1.f, 
		l? -.73f : -.67f, s? -.56f : 1.f};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiRect), uiRect, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glBindVertexArray(0);

	GLfloat hpRect[12] = {l? -.03f : .03f, s? -.63f : .62f, 
		l ? .67f : .73f, s? -.63f : .62f, 
		l ? .67f : .73f, s? -.57f : 1.f,
		l ? -.03f : .03f, s? -.63f : .62f, 
		l ? .67f : .73f, s? -.57f : 1.f,
		l ? -.03f : .03f, s? -.57f : 1.f};

	glBindVertexArray(rectVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hpRect), hpRect, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindVertexArray(0);

	if(s){
		GLfloat nrgRect[12] ={ l ? -.03f : .03f, -.69f, 
			l? .67f : .73f, -.69f , 
			l ? .67f : .73f, -.63f,
			l ? -.03f : .03f, -.69f, 
			l ? .67f : .73f, -.63f,
			l ? -.03f : .03f, -.63f};

		glBindVertexArray(rectVAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(nrgRect), nrgRect, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindVertexArray(0);
	}

	glGenTextures(1, &texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	int width, height;
	unsigned char* image;
	image = loadPPM(s? "textures/P2_UI.ppm" : "textures/P2_HP.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	delete(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

UI_Bar::~UI_Bar()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(2, rectVAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, rectVBO);
	glDeleteTextures(1, &texID);
}

void UI_Bar::fetchUniforms(GLuint uiShader, GLuint rectShader){
	glUseProgram(uiShader);
	uSampler = glGetUniformLocation(uiShader, "tex");
	glUseProgram(rectShader);
	uColor = glGetUniformLocation(rectShader, "color");
}

void UI_Bar::draw(GLuint uiShader, GLuint rectShader){
	glUseProgram(uiShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(uSampler, 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glUseProgram(rectShader);
	glUniform3f(uColor, .95f, .15f, .25f);
	glBindVertexArray(rectVAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if(isSelfUI){
		glUniform3f(uColor, .1f, .4f, .95f);
		glBindVertexArray(rectVAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

void UI_Bar::update(float selfHP, float selfNRG, float foeHP, bool l){
	if(selfHP < 0){
		selfHP = 0;
		isGameOver = true;
		cout << "Game over! Opponent wins!" << endl;
	}
	if(foeHP < 0){
		foeHP = 0;
		isGameOver = true;
		cout << "Game over! You win!" << endl;
	}
	if(isSelfUI){
		GLfloat hpRect[12] = { l ? -.03f : .03f, -.63f,
			(l? -.03f : .03f) + selfHP * .7f, -.63f, 
			(l ? -.03f : .03f) + selfHP * .7f, -.57f,
			l ? -.03f : .03f, -.63f, 
			(l ? -.03f : .03f) + selfHP * .7f, -.57f,
			l ? -.03f : .03f, -.57f};

		glBindVertexArray(rectVAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(hpRect), hpRect, GL_STATIC_DRAW);
		glBindVertexArray(0);

		GLfloat nrgRect[12] ={ l ? -.03f : .03f, -.69f, 
			(l ? -.03f : .03f) + selfNRG * .7f, -.69f ,
			(l ? -.03f : .03f) + selfNRG * .7f, -.63f,
			l ? -.03f : .03f, -.69f,
			(l ? -.03f : .03f) + selfNRG * .7f, -.63f,
			l ? -.03f : .03f, -.63f};

		glBindVertexArray(rectVAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(nrgRect), nrgRect, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	else{
		GLfloat hpRect[12] ={ l ? -.03f : .03f, .62f, 
			(l ? -.03f : .03f) + foeHP * .7f, .62f,
			(l ? -.03f : .03f) + foeHP * .7f, 1.f,
			l ? -.03f : .03f, .62f, 
			(l ? -.03f : .03f) + foeHP * .7f, 1.f,
			l ? -.03f : .03f, 1.f};

		glBindVertexArray(rectVAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(hpRect), hpRect, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
}