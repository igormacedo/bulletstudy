#pragma once

#include "glew\glew.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"

using namespace glm;

class Point
{
public:
	Point();
	Point(vec3);
	Point(vec3,vec3);
	~Point();

	void UpdatePosition(vec3);
	void DrawObject(GLint shaderProgram, mat4 mvp);

private:
	GLuint VAO;
	GLuint VBO;
	GLfloat data[8];
};

