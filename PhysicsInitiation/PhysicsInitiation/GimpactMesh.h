#pragma once

#include "glew\glew.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision\Gimpact\btGImpactShape.h"
#include <iostream>

using namespace glm;

class GimpactMesh
{
public:
	GimpactMesh(float* data, GLuint* indexes, int sizeofdata, int sizeofindexes);
	~GimpactMesh();

	void drawObject(GLint shaderProgram, mat4 mvp);
	void updateGraphicsPosition();
	btRigidBody* getRigidBody();

private:
	int sizeofdata;
	int sizeofindexes;
	GLfloat* data;
	GLuint* indexes;
	btScalar mat[16];

	//OpenGL rendering variables
	GLuint VAO;
	GLuint VBO_vertices;
	GLuint VBO_indexes;

	//Bullet simulation Objects
	btTriangleMesh *mTriMesh;
	btGImpactMeshShape *GimpactMeshShape;
	btDefaultMotionState *GimpactMeshMotionState;
	btRigidBody::btRigidBodyConstructionInfo *GimpactMeshRigidBodyCI;
	btRigidBody *GimpactMeshRigidBody;

	void setupBulletRigidBody();
};

