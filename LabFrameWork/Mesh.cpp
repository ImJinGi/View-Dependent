#include "Mesh.h"


#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>


#include "Loader.h"
#include "stb_image.h"


Mesh::MeshEntry::MeshEntry(aiMesh *mesh) 
{
	vbo[VERTEX_BUFFER] = NULL;
	vbo[TEXCOORD_BUFFER] = NULL;
	vbo[NORMAL_BUFFER] = NULL;
	vbo[INDEX_BUFFER] = NULL;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	elementCount = mesh->mNumFaces * 3;

	if (mesh->HasPositions()) {
		float *vertices = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &vbo[VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		delete vertices;
	}

	if (mesh->HasNormals()) {
		float *normals = new float[mesh->mNumVertices * 3];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &vbo[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		delete normals;
	}


	if (mesh->HasTextureCoords(0)) {
		float *texCoords = new float[mesh->mNumVertices * 2];
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &vbo[TEXCOORD_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		delete texCoords;
	}


	if (mesh->HasFaces()) {
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
		for (int i = 0; i < mesh->mNumFaces; ++i) {
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &vbo[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		//glEnableVertexAttribArray (3);

		delete indices;
	}
}

/**
*	Deletes the allocated OpenGL buffers
**/
Mesh::MeshEntry::~MeshEntry() {
	if (vbo[VERTEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[VERTEX_BUFFER]);
	}

	if (vbo[TEXCOORD_BUFFER]) {
		glDeleteBuffers(1, &vbo[TEXCOORD_BUFFER]);
	}

	if (vbo[NORMAL_BUFFER]) {
		glDeleteBuffers(1, &vbo[NORMAL_BUFFER]);
	}

	if (vbo[INDEX_BUFFER]) {
		glDeleteBuffers(1, &vbo[INDEX_BUFFER]);
	}

	glDeleteVertexArrays(1, &vao);
}

/**
*	Renders this MeshEntry
**/

void Mesh::MeshEntry::render()
{
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
}

void Mesh::MeshEntry::render(ShaderProgram * shader, glm::mat4 & model, glm::mat4 & view, glm::mat4 & projection, glm::vec3 & camPos)
{
	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_2d[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_2d[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_2d[2]);


	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);

	shader->disable();
}

/**
*	Mesh constructor, loads the specified filename if supported by Assimp
**/
Mesh::Mesh(const char *filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes);

	//const aiScene *scene = importer.ReadFile(filename, NULL);
	if (!scene) {
		printf("Unable to laod mesh: %s\n", importer.GetErrorString());
	}

	for (int i = 0; i < scene->mNumMeshes; ++i) {
		meshEntries.push_back(new Mesh::MeshEntry(scene->mMeshes[i]));
	}
}

/**
*	Clears all loaded MeshEntries
**/
Mesh::~Mesh(void)
{
	for (int i = 0; i < meshEntries.size(); ++i) {
		delete meshEntries.at(i);
	}
	meshEntries.clear();
}

/**
*	Renders all loaded MeshEntries
**/
void Mesh::render(glm::mat4 & model, glm::mat4 & view, glm::mat4 & projection, glm::vec3 & camPos)
{
	for (int i = 0; i < meshEntries.size(); ++i)
	{
		meshEntries.at(i)->render(m_shaderProgram, model, view, projection, camPos);
	}
}

void Mesh::render()
{
	for (int i = 0; i < meshEntries.size(); ++i)
	{
		meshEntries.at(i)->render();
	}
}


//void initialise(std::string vertexShaderSource, std::string fragmentShaderSource)
//{
//	// Compile the shaders and return their id values
//	vertexShaderId = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
//	fragmentShaderId = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
//
//	// Attach the compiled shaders to the shader program
//	glAttachShader(programId, vertexShaderId);
//	glAttachShader(programId, fragmentShaderId);
//
//	// Link the shader program - details are placed in the program info log
//	glLinkProgram(programId);
//
//	// Once the shader program has the shaders attached and linked, the shaders are no longer required.
//	// If the linking failed, then we're going to abort anyway so we still detach the shaders.
//	glDetachShader(programId, vertexShaderId);
//	glDetachShader(programId, fragmentShaderId);
//
//	// Check the program link status and throw a runtime_error if program linkage failed.
//	GLint programLinkSuccess = GL_FALSE;
//	glGetProgramiv(programId, GL_LINK_STATUS, &programLinkSuccess);
//	if (programLinkSuccess == GL_TRUE)
//	{
//		if (DEBUG)
//		{
//			std::cout << "Shader program link successful." << std::endl;
//		}
//	}
//	else
//	{
//		std::cout << "Shader program link failed: " << getInfoLog(ObjectType::PROGRAM, programId) << std::endl;
//	}
//
//	// Validate the shader program
//	glValidateProgram(programId);
//
//	// Check the validation status and throw a runtime_error if program validation failed
//	GLint programValidatationStatus;
//	glGetProgramiv(programId, GL_VALIDATE_STATUS, &programValidatationStatus);
//	if (programValidatationStatus == GL_TRUE)
//	{
//		if (DEBUG)
//		{
//			std::cout << "Shader program validation successful." << std::endl;
//		}
//	}
//	else
//	{
//		std::cout << "Shader program validation failed: " << getInfoLog(ObjectType::PROGRAM, programId) << std::endl;
//	}
//
//	// Finally, the shader program is initialised
//	initialised = true;
//}