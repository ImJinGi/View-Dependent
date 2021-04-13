
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <vector>
#include <random>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Loader.h"

#include "KTX.h"

const int MaxParticles = 10500;

class Particle
{
private:
	float particleLifetime;
	float particleSize;

	
public:
	GLuint posBuf[2], velBuf[2], age[2], particles_layer_buffer[2], randPos;
	GLuint initPosBuf, layerBuf;
	GLuint testBuf;
	GLuint tex_array;
	GLuint particleArray[2];
	GLuint feedback[2];
	GLuint drawBuf;
	GLuint subroutineIndex;
	std::vector<glm::vec3> layer;
	int index = 0;
	std::vector<float> particle_initAge;
	std::vector<float> particle_initialLife;

	int nParticles;
	std::vector<glm::vec3> particle_randomPos;
	std::vector<glm::vec3> particle_initPos;

	glm::vec3 uniformTest[MaxParticles];

	Particle();
	Particle(int _nParticles, glm::vec3 _emitterPos, glm::vec3 _emitterDir, float _particleLifeTime, float _particleSize);

	int getParticles() { return nParticles; }
	float getParticleLifetime() { return particleLifetime; }
	float getParticleSize() { return particleSize; }

	void initBuffers(glm::vec3 frustum_center, float _radius, 
		glm::vec3 right_plane_vec, glm::vec3 nbr,
		glm::vec3 left_plane_vec, glm::vec3 nbl);

	void updateParticles(ShaderProgram* shader, float deltaT);
	void updateParticles(ShaderProgram* shader, float deltaT, glm::vec3 center, float radius, glm::vec3 nbr, glm::vec3 nbl,
	glm::vec3 right_plane_vector, glm::vec3 left_plane_vector, glm::vec3 bottom_plane_vector, glm::vec3 top_plane_vector,
		float near_width, float near_height);
	void renderParticles(ShaderProgram* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
};

#endif