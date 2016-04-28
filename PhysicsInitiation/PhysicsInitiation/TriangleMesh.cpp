#include "TriangleMesh.h"


TriangleMesh::TriangleMesh(GLfloat *data, GLuint *indexes, int sizeofdata, int sizeofindexes)
{
	this->data = data;
	this->indexes = indexes;
	this->sizeofindexes = sizeofindexes;
	this->sizeofdata = sizeofdata;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeofdata, data, GL_STATIC_DRAW);
	std::cout << sizeofdata << std::endl;

	glGenBuffers(1, &VBO_indexes);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indexes);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofindexes, indexes, GL_STATIC_DRAW);

	setupBulletRigidBody();
}


TriangleMesh::~TriangleMesh()
{
}

void TriangleMesh::setupBulletRigidBody()
{
	mTriMesh = new btTriangleMesh();

	for (int i=0; i < sizeofdata/(sizeof(GLfloat)) ; i+=8)
	{
		mTriMesh->findOrAddVertex(btVector3(data[i], data[i + 1], data[i+2]), false);
		std::cout << data[i] << " " << data[i + 1] << " " << data[i + 2] << std::endl;
	}
	std::cout << sizeofdata / (sizeof(GLfloat)*8) << std::endl;

	for (int i=0; i < sizeofindexes/(sizeof(GLuint)); i+=3)
	{
		mTriMesh->addTriangleIndices(indexes[i], indexes[i+1], indexes[i+2]);
		std::cout << indexes[i] << " " << indexes[i + 1] << " " << indexes[i + 2] << std::endl;
	}
	std::cout << sizeofindexes / (sizeof(GLuint)*3) << std::endl;

	triangleMeshShape = new btBvhTriangleMeshShape(mTriMesh,true);
	triangleMeshMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	//btVector3 triangleMeshInertia(0, 0, 0);
	//triangleMeshShape->calculateLocalInertia(1, triangleMeshInertia);
	//triangleMeshRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(0, triangleMeshMotionState, triangleMeshShape, triangleMeshInertia);


	triangleMeshRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(0, triangleMeshMotionState, triangleMeshShape, btVector3(0, 0, 0));
	triangleMeshRigidBody = new btRigidBody(*triangleMeshRigidBodyCI);
	triangleMeshRigidBody->setRestitution(btScalar(0.95));
}

btRigidBody* TriangleMesh::getRigidBody()
{
	return triangleMeshRigidBody;
}

void TriangleMesh::drawObject(GLint shaderProgram, mat4 mvp)
{
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indexes);

	GLint mvpMatrixUniformLocation = glGetUniformLocation(shaderProgram, "mvpMatrix");
	glUniformMatrix4fv(mvpMatrixUniformLocation, 1, GL_FALSE, &mvp[0][0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, sizeofindexes, GL_UNSIGNED_INT, (void*)0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
