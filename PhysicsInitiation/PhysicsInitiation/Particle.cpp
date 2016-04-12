#include "Particle.h"


Particle::Particle()
{
	m_vGravity = vec3(0, -9.8, 0);
	m_x[0] = vec3(-0.999, 0, 0);
	m_oldx[0] = vec3(-1, 0, 0);
	m_fTimeStep = 0.0005;
}

Particle::~Particle()
{
}

void Particle::Verlet()
{
	for (int i = 0; i < num_particles; i++)
	{
		vec3& x = m_x[i];
		vec3 temp = x;
		vec3& oldx = m_oldx[i];
		vec3& a = m_a[i];
		x += x - oldx + a*m_fTimeStep*m_fTimeStep;
		oldx = temp;
	}
}

void Particle::AccumulateForces()
{
	for (int i = 0; i < num_particles; i++)
	{
		m_a[i] = m_vGravity;
	}
}

void Particle::SatisfyConstraints()
{
	for (int i = 0; i < num_particles; i++) {
		vec3& x = m_x[i];
		if (x.x < -1) x.x = -1;
		if (x.y < -1) x.y = -1;
		if (x.z < -1) x.z = -1;

		if (x.x > 1) x.x = 1;
		if (x.y > 1) x.y = 1;
		if (x.z > 1) x.z = 1;
	}
}

void Particle::TimeStep(GLfloat* v, int n, int elementSize)
{
	AccumulateForces();
	Verlet();
	SatisfyConstraints();

	for (int i = 0; i < n; i++)
	{
		v[elementSize*i] = m_x[i].x;
		v[elementSize*i+1] = m_x[i].y;
		v[elementSize*i+2] = m_x[i].z;
	}
}
