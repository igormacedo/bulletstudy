#pragma once

#include "glew\glew.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "btBulletDynamicsCommon.h"
#include <iostream>

using namespace glm;

class TriangleMesh
{
public:
	TriangleMesh(GLfloat* data, GLuint* indexes, int sizeofdata, int sizeofindexes);
	~TriangleMesh();

	void drawObject(GLint shaderProgram, mat4 mvp);
	btRigidBody* getRigidBody();
private:
	int sizeofdata;
	int sizeofindexes;
	GLfloat* data;
	GLuint* indexes;

	//OpenGL rendering variables
	GLuint VAO;
	GLuint VBO_vertices;
	GLuint VBO_indexes;

	//Bullet simulation Objects
	btTriangleMesh *mTriMesh;
	btCollisionShape *triangleMeshShape;
	btDefaultMotionState *triangleMeshMotionState;
	btRigidBody::btRigidBodyConstructionInfo *triangleMeshRigidBodyCI;
	btRigidBody *triangleMeshRigidBody;

	void setupBulletRigidBody();
};

