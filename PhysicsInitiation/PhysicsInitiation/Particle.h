#include "glew\glew.h"
#include "freeglut\freeglut.h"
#include "glm\glm\glm.hpp"
#pragma once

using namespace glm;

#define num_particles 1

class Particle
{
	vec3 m_x[num_particles];
	vec3 m_oldx[num_particles];
	vec3 m_a[num_particles];
	vec3 m_vGravity;
	float m_fTimeStep;
public:
	Particle();
	~Particle();
	void TimeStep(GLfloat*, int, int);

private:
	void Verlet();
	void SatisfyConstraints();
	void AccumulateForces();
};

