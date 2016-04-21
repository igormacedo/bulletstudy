#pragma once

#include "glew\glew.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "btBulletDynamicsCommon.h"

using namespace glm;

class Point
{
public:
	Point(vec3 = vec3(0.0f, 0.0f, 0.0f), vec3 = vec3(0.7f, 0.0f, 1.0f));
	~Point();

	void updatePosition(vec3);
	void drawObject(GLint shaderProgram, mat4 mvp);
	btRigidBody* getRigidBody();

private:
	//Object States
	vec3 position;
	vec3 color;
	btScalar radio = 0.70f;
	btScalar mass = 1.0f;

	//OpenGL rendering variables
	GLuint VAO;
	GLuint VBO;
	GLfloat data[8];

	//Bullet simulation Objects
	btCollisionShape *sphereShape;
	btDefaultMotionState *sphereMotionState;
	btScalar massRigidBody;
	btVector3 sphereInertia;
	btRigidBody::btRigidBodyConstructionInfo *sphereRigidBodyCI;
	btRigidBody *sphereRigidBody;

	void setupBulletRigidBody();
};

