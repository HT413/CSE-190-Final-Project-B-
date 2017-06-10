#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "Globals.h"
#include "Materials.h"

#include <algorithm>
#include <fstream>
#include <sstream>

class OBJObject
{
private:
	vector<unsigned int> indices;
	vector<vec3> vertices;
	vector<vec3> normals;
	GLuint VBO, NBO, VAO, EBO;
	mat4 model, parentModel;
	GLuint shaderProg, uModel;
	Material *mat;

public:
	OBJObject(const char* filepath);
	~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
	void setMaterial(Material* a){ mat = a; }
	void setModel(mat4 m){ model = m; }
	void setParentModel(mat4 m){ parentModel = m; }
};

#endif