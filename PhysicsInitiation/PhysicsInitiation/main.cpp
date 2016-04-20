#include "glew\glew.h"
#include "freeglut\freeglut.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "Shader_Manager.h"
#include "btBulletDynamicsCommon.h"
#include "Point.h"

#include <iostream>

using namespace std;
using namespace Managers;
using glm::vec3;
using glm::mat4;

int width = 1024;
int height = 768;

int mousePositionx;
int mousePositiony;

float rotate_x = 0;
float rotate_y = 0;

GLint programID;

GLfloat vertexDataforIndex[] = {
	//  X     Y     Z      U    V     R     G     B
	-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

GLuint indexData[] = {
	0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 1, 5, 7, 1, 7, 3, 0, 4, 6, 0, 6, 2, 2, 3, 7, 2, 7, 6, 0, 1, 5, 0, 5, 4
};

float Vertices[9] = {
-1.0f, -1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
0.0f, 1.0f, 0.0f };

GLfloat uParticle[] = {
 //  X     Y     Z      U    V     R     G     B
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
};

Shader_Manager shaderManager;
//Particle particle;

GLuint VBO;
GLuint indexBuffer;

void RenderSceneCB();
void installShaders();
void specialKeys(int, int, int);
void MouseMotion(int, int);
void Timer(int);

Point *point, *p2, *p3;

// Global Bullet objects
btRigidBody* fallRigidBody;
btRigidBody* fallRigidBody2;
btDiscreteDynamicsWorld* dynamicsWorld;

bool debug = true;

void main(int argc, char** argv)
{
	//OPENGL initialization =============================================

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GL_MULTISAMPLE);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(700, 100);
	glutCreateWindow("CubeTry");

	glutDisplayFunc(RenderSceneCB);
	//glutIdleFunc(RenderSceneCB); // Calls the function as many times as it can
	glutSpecialFunc(specialKeys);
	glutTimerFunc(10, Timer, 0);
	glutPassiveMotionFunc(MouseMotion);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glewExperimental = GL_TRUE;
	glewInit();

	point = new Point(vec3(0.0, 0.4, 0.0));
	p2 = new Point(vec3(-0.0, 0.4, 0.0), vec3(0.9,0.9,0.9));
	p3 = new Point(vec3(0.0, 0.5, 0.0));
	

	//glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexDataforIndex), vertexDataforIndex, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(uParticle), uParticle, GL_STATIC_DRAW);
	//glGenBuffers(1, &indexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * 3 * sizeof(GLuint), &indexData[0], GL_STATIC_DRAW);

	cout << "OpenGL Version: " << (char*)glGetString(GL_VERSION) << " | Shader Language Version: " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "| Glut Version : " << glutGet(GLUT_VERSION) << endl;

	// Bullet Initialization and setup environment ================

	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//btCollisionShape* groundShape = new btBoxShape(btVector3(20, 20, 20));
	//btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 20, 0)));
	//btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	//groundRigidBody->setRestitution(btScalar(1));
	//dynamicsWorld->addRigidBody(groundRigidBody);

	btTriangleMesh *mTriMesh = new btTriangleMesh();
	btVector3 v0(20, 0, 20);
	btVector3 v1(20, 0, -20);
	btVector3 v2(-20, 0, 20);
	btVector3 v3(-20, 0, -20);
	btVector3 v4(20, 40, 20);
	btVector3 v5(20, 40, -20);
	btVector3 v6(-20, 40, 20);
	btVector3 v7(-20, 40, -20);

	mTriMesh->addTriangle(v2,v3,v7);
	mTriMesh->addTriangle(v2,v7,v6);
	mTriMesh->addTriangle(v0,v1,v2);
	mTriMesh->addTriangle(v1,v2,v3);
	mTriMesh->addTriangle(v0,v1,v5);
	mTriMesh->addTriangle(v0,v5,v4);
	//mTriMesh->addTriangle();
	//mTriMesh->addTriangle();
	//mTriMesh->addTriangle();
	//mTriMesh->addTriangle();

	btCollisionShape* boxShape = new btBvhTriangleMeshShape(mTriMesh,true);
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(0, boxMotionState, boxShape, btVector3(0, 0, 0));
	btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);
	boxRigidBody->setRestitution(btScalar(0.9));
	dynamicsWorld->addRigidBody(boxRigidBody);

	btCollisionShape* groundShape1 = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//btCollisionShape* groundShape = new btBoxShape(btVector3(20, 20, 20));
	btDefaultMotionState* groundMotionState1 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI1(0, groundMotionState1, groundShape1, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody1 = new btRigidBody(groundRigidBodyCI1);
	groundRigidBody1->setRestitution(btScalar(1));
	dynamicsWorld->addRigidBody(groundRigidBody1);


	/*btCollisionShape* fallShape = new btSphereShape(1);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	fallRigidBodyCI.m_restitution = 0.8;
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(fallRigidBody);

	btDefaultMotionState* fallMotionState2 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 5, 0)));
	btScalar mass2 = 1;
	btVector3 fallInertia2(0, 0, 0);
	fallShape->calculateLocalInertia(mass2, fallInertia2);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI2(mass2, fallMotionState2, fallShape, fallInertia2);
	fallRigidBodyCI2.m_restitution = 0;
	fallRigidBody2 = new btRigidBody(fallRigidBodyCI2);
	fallRigidBody2->setCollisionFlags(fallRigidBody2->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	fallRigidBody2->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(fallRigidBody2);*/

	point->getRigidBody()->setCollisionFlags(point->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	point->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(point->getRigidBody());

	p2->getRigidBody()->setLinearFactor(btVector3(1, 1, 0));
	dynamicsWorld->addRigidBody(p2->getRigidBody());
	p3->getRigidBody()->setLinearFactor(btVector3(1, 1, 0));
	dynamicsWorld->addRigidBody(p3->getRigidBody());
	

	//=============================================================

	installShaders();
	glutMainLoop();

	//== exit ====================================================

	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
}

void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float x = ((mousePositionx - width / 2) / (float)(width / 2))* 115.2;
	float y = ((mousePositiony - height / 2) / (float)(height / 2))*-86.5;

	dynamicsWorld->stepSimulation(1/30.f, 10000, 1/100.f);

	mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(rotate_x, rotate_y, 3),  // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0),  // and looks at the origin
		glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down)
	glm::mat4 mvp = projectionMatrix * View * Model;

	//GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransdformMatrix");
	//GLint projectionMatrixUniformLocation = glGetUniformLocation(programID, "projectionMatrix");
	//--GLint mvpMatrixUniformLocation = glGetUniformLocation(programID, "mvpMatrix");
	//--glUniformMatrix4fv(mvpMatrixUniformLocation, 1, GL_FALSE, &mvp[0][0]);
	//glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	//--glEnableVertexAttribArray(0);
	//--glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	//--glEnableVertexAttribArray(1);
	//--glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	//--glEnable(GL_POINT_SMOOTH);
	//--glPointSize(10);
	//--glDrawArrays(GL_POINTS, 0, 2);
	//glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, (void*)0);
	//--glDisableVertexAttribArray(0);
	//--glDisableVertexAttribArray(1);

	btTransform trans;
	point->getRigidBody()->getMotionState()->getWorldTransform(trans);
	trans.setOrigin(btVector3(x, y, 0));
	point->updatePosition(vec3(x/50, y/50, 0));
	point->getRigidBody()->getMotionState()->setWorldTransform(trans);
	point->drawObject(programID, mvp);
	if (debug) cout << "x:" << x << " y:" << y << " z:" << 0 << endl;

	p2->getRigidBody()->getMotionState()->getWorldTransform(trans);
	p2->updatePosition(vec3(trans.getOrigin().getX() / 50, trans.getOrigin().getY() / 50, trans.getOrigin().getZ() / 50));
	//p2->updatePosition(vec3(trans.getOrigin().getX() / 50, trans.getOrigin().getY() / 50, 0.0f));
	p2->drawObject(programID, mvp);
	if (debug) cout << "x:" << trans.getOrigin().getX() << " y:" << trans.getOrigin().getY() << " z:" << trans.getOrigin().getZ() << endl;

	p3->getRigidBody()->getMotionState()->getWorldTransform(trans);
	//p3->updatePosition(vec3(trans.getOrigin().getX() / 50, trans.getOrigin().getY() / 50, trans.getOrigin().getZ() / 50));
	p3->updatePosition(vec3(trans.getOrigin().getX() / 50, trans.getOrigin().getY() / 50, 0.0f));
	p3->drawObject(programID, mvp);
	if (debug) cout << "x:" << trans.getOrigin().getX() << " y:" << trans.getOrigin().getY() << " z:" << trans.getOrigin().getZ() << endl << endl;

	glutSwapBuffers();
}

void installShaders()
{
	shaderManager.CreateProgram("greenColor", "VertexShader.glsl", "FragmentShader.glsl");
	programID = shaderManager.GetShader("greenColor");
	glUseProgram(programID);
}

void specialKeys(int key, int x, int y) {

	//  Right arrow - increase rotation by 5 degree
	if (key == GLUT_KEY_RIGHT)
		rotate_x += 0.1;

	//  Left arrow - decrease rotation by 5 degree
	else if (key == GLUT_KEY_LEFT)
		rotate_x -= 0.1;

	else if (key == GLUT_KEY_UP)
		rotate_y += 0.1;

	else if (key == GLUT_KEY_DOWN)
		rotate_y -= 0.1;

	//  Request display update
	glutPostRedisplay();

}

void MouseMotion(int x, int y)
{
	mousePositionx = x;
	mousePositiony = y;
}

void Timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}