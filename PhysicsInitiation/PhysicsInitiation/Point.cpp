#include "Point.h"


Point::Point()
{
}

Point::Point(vec3 position)
{
	data[0] = position.x;
	data[1] = position.y;
	data[2] = position.z;
	data[7] = 1.0f;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

Point::Point(vec3 position, vec3 color)
{
	data[0] = position.x;
	data[1] = position.y;
	data[2] = position.z;
	data[5] = color.x;
	data[6] = color.y;
	data[7] = color.z;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}


Point::~Point()
{
}

void Point::UpdatePosition(vec3 position)
{
	data[0] = position.x;
	data[1] = position.y;
	data[2] = position.z;

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* 3, data);
}

void Point::DrawObject(GLint shaderProgram, mat4 mvp)
{
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLint mvpMatrixUniformLocation = glGetUniformLocation(shaderProgram, "mvpMatrix");
	glUniformMatrix4fv(mvpMatrixUniformLocation, 1, GL_FALSE, &mvp[0][0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	glEnable(GL_POINT_SMOOTH);
	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, 1);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}
