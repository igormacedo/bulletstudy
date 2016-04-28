#include "glew\glew.h"
#include "freeglut\freeglut.h"
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "Shader_Manager.h"
#include "btBulletDynamicsCommon.h"
#include "Point.h"
#include "TriangleMesh.h"

#include <list>
#include <iostream>
#include <time.h>

clock_t t;
float redrawtime;

using namespace std;
using namespace Managers;
using glm::vec3;
using glm::mat4;

float screenRatio = 0.75;
int width = 1024;
int height = width*screenRatio;
float orthoHalfWidth = 66.66f;
float orthoHalfHeight = orthoHalfWidth*screenRatio;

int mousePositionx;
int mousePositiony;

float rotate_x = 0;
float rotate_y = 0;

GLint programID;

GLfloat vertexDataforIndex[] = {
	//  X     Y     Z      U    V     R     G     B
	-20.0f, 0.0f, -20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //0
	20.0f, 0.0f, -20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  //1
	-20.0f, 40.0f, -20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  //2
	20.0f, 40.0f, -20.0f, 20.0f, 0.0f, 1.0f, 1.0f, 0.0f,   //3
	-20.0f, 0.0f, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  //4
	20.0f, 0.0f, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   //5
	-20.0f, 40.0f, 20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   //6
	20.0f, 40.0f, 20.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    //7
};

GLuint indexData[] = {
	0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 1, 5, 7, 1, 7, 3, 0, 4, 6, 0, 6, 2, 2, 3, 7, 2, 7, 6, 0, 1, 5, 0, 5, 4};

Shader_Manager shaderManager;

GLuint VBO;
GLuint indexBuffer;

void RenderSceneCB();
void installShaders();
void specialKeys(int, int, int);
void MouseMotion(int, int);
void Timer(int);
void mouseClick(int, int, int, int);


//
list<Point> points;
Point *point, *p2, *p3;
TriangleMesh *tm;

// Global Bullet objects
btDiscreteDynamicsWorld* dynamicsWorld;

bool debug = false;

void main(int argc, char** argv)
{
	t = clock();

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
	glutMouseFunc(mouseClick);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glewExperimental = GL_TRUE;
	glewInit();

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

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(btScalar(1));
	dynamicsWorld->addRigidBody(groundRigidBody);

	tm = new TriangleMesh(vertexDataforIndex, indexData, sizeof(vertexDataforIndex), sizeof(indexData));
	dynamicsWorld->addRigidBody(tm->getRigidBody());

	point = new Point(vec3(0.0, 10, 0.0));
	p2 = new Point(vec3(20.0, 10, 0.0), vec3(0.9, 0.9, 0.9));
	p3 = new Point(vec3(0.0, 20, 0.0));

	point->getRigidBody()->setCollisionFlags(point->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	point->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(point->getRigidBody());

	p2->getRigidBody()->setLinearFactor(btVector3(1, 1, 0));
	dynamicsWorld->addRigidBody(p2->getRigidBody());
	p3->getRigidBody()->setLinearFactor(btVector3(1, 1, 0));
	dynamicsWorld->addRigidBody(p3->getRigidBody());
	

	//=============================================================

	installShaders();

	t = clock() - t;
	cout << "It took " << ((float)t) / CLOCKS_PER_SEC << " seconds to initialize." << endl;

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
	t = clock();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float x = ((mousePositionx - width / 2) / (float)(width / 2))*orthoHalfWidth;//* 115.2;
	float y = -((mousePositiony - height / 2) / (float)(height / 2))*orthoHalfHeight;//*-86.5;

	dynamicsWorld->stepSimulation(1/100.f, 1000, 1/60.f);

	mat4 projectionMatrix = glm::ortho(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, 0.001f, 1000.0f);
	//mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(rotate_x, rotate_y, 3),  // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0),  // and looks at the origin
		glm::vec3(0, 1, 0)); // Head is up (set to 0,-1,0 to look upside-down)
	glm::mat4 mvp = projectionMatrix * View * Model;

	btTransform trans;
	point->getRigidBody()->getMotionState()->getWorldTransform(trans);
	trans.setOrigin(btVector3(x, y, 0));
	point->updatePosition(vec3(x, y, 0));
	point->getRigidBody()->getMotionState()->setWorldTransform(trans);
	point->drawObject(programID, mvp);
	if (debug) cout << "x:" << x << " y:" << y << " z:" << 0 << endl;

	p2->getRigidBody()->getMotionState()->getWorldTransform(trans);
	p2->updatePosition(vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	p2->drawObject(programID, mvp);
	if (debug) cout << "x:" << trans.getOrigin().getX() << " y:" << trans.getOrigin().getY() << " z:" << trans.getOrigin().getZ() << endl;

	p3->getRigidBody()->getMotionState()->getWorldTransform(trans);
	p3->updatePosition(vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	p3->drawObject(programID, mvp);
	if (debug) cout << "x:" << trans.getOrigin().getX() << " y:" << trans.getOrigin().getY() << " z:" << trans.getOrigin().getZ() << endl << endl;

	for (std::list<Point>::iterator it = points.begin(); it != points.end(); ++it)
	{
		(*it).getRigidBody()->getMotionState()->getWorldTransform(trans);
		(*it).updatePosition(vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
		(*it).drawObject(programID, mvp);
	}

	tm->drawObject(programID, mvp);

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
	t = clock() - t;
	//t_time = ((float)t) / CLOCKS_PER_SEC;
	redrawtime = ((float)t) / CLOCKS_PER_SEC;
	cout << "It took " << redrawtime << " seconds to redraw." << endl;
}

void mouseClick(int button, int state, int x, int y)
{
	float xs = ((mousePositionx - width / 2) / (float)(width / 2))*orthoHalfWidth;//* 115.2;
	float ys = -((mousePositiony - height / 2) / (float)(height / 2))*orthoHalfHeight;//*-86.5;

	if (button == GLUT_LEFT_BUTTON)// && state == GLUT_DOWN)
	{
		for (int i = 0; i < 10; i++)
		{
			points.push_back(Point(vec3(xs, ys, 0)));
			points.back().getRigidBody()->setLinearFactor(btVector3(1, 1, 0));
			dynamicsWorld->addRigidBody(points.back().getRigidBody());
		}
	}
}