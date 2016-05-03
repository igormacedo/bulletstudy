#include "GimpactMesh.h"


GimpactMesh::GimpactMesh(float* data, GLuint* indexes, int sizeofdata, int sizeofindexes)
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


GimpactMesh::~GimpactMesh()
{
}

void GimpactMesh::updateGraphicsPosition()
{
	btTransform trans;
	GimpactMeshRigidBody->getMotionState()->getWorldTransform(trans);
	
	//Finish translation here
	//trans.getOrigin
	//glm::vec3 vec = glm::make_vec3(mat);
	//WoodenCrateInstances.at(i).transform = glm::translate(glm::mat4(), vec);

	/*for (int i = 0; i < sizeof(data) / sizeof(GLfloat); i += 8)
	{
		if (mousePositionx > mousePositionxBefore)
		{
			vertexDataforIndex[i] = vertexDataforIndex[i] + (mousePositionx - mousePositionxBefore);
		}
		else if (mousePositionx < mousePositionxBefore)
		{
			vertexDataforIndex[i] = vertexDataforIndex[i] - (mousePositionxBefore - mousePositionx);
		}

		if (mousePositiony > mousePositionxBefore)
		{
			vertexDataforIndex[i + 1] = vertexDataforIndex[i + 1] + ((mousePositiony - mousePositionyBefore));
		}
		else if (mousePositiony < mousePositionxBefore)
		{
			vertexDataforIndex[i + 1] = vertexDataforIndex[i + 1] - ((mousePositionyBefore - mousePositiony));
		}
	}*/
}

void GimpactMesh::setupBulletRigidBody()
{
	mTriMesh = new btTriangleMesh();

	for (int i = 0; i < sizeofdata / (sizeof(GLfloat)); i += 8)
	{
		mTriMesh->findOrAddVertex(btVector3(data[i], data[i + 1], data[i + 2]), false);
		std::cout << data[i] << " " << data[i + 1] << " " << data[i + 2] << std::endl;
	}
	std::cout << sizeofdata / (sizeof(GLfloat) * 8) << std::endl;

	for (int i = 0; i < sizeofindexes / (sizeof(GLuint)); i += 3)
	{
		mTriMesh->addTriangleIndices(indexes[i], indexes[i + 1], indexes[i + 2]);
		std::cout << indexes[i] << " " << indexes[i + 1] << " " << indexes[i + 2] << std::endl;
	}
	std::cout << sizeofindexes / (sizeof(GLuint) * 3) << std::endl;

	GimpactMeshShape = new btGImpactMeshShape(mTriMesh);
	GimpactMeshShape->updateBound();
	GimpactMeshMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	btVector3 GimpactMeshInertia(0, 0, 0);
	GimpactMeshShape->calculateLocalInertia(1, GimpactMeshInertia);
	//triangleMeshRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(0, triangleMeshMotionState, triangleMeshShape, triangleMeshInertia);

	GimpactMeshRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(0, GimpactMeshMotionState, GimpactMeshShape, btVector3(0, 0, 0));
	//GimpactMeshRigidBodyCI = new btRigidBody::btRigidBodyConstructionInfo(1, GimpactMeshMotionState, GimpactMeshShape, GimpactMeshInertia);
	GimpactMeshRigidBody = new btRigidBody(*GimpactMeshRigidBodyCI);
	GimpactMeshRigidBody->setRestitution(btScalar(0.95));
}

btRigidBody* GimpactMesh::getRigidBody()
{
	return GimpactMeshRigidBody;
}

void GimpactMesh::drawObject(GLint shaderProgram, mat4 mvp)
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
