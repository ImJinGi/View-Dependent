
#include "Particle.h"
#include <chrono>

#include <glm/gtc/random.hpp>

#define _USE_MATH_DEFINES
#include  <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Particle::Particle()
	: drawBuf(1.0f), nParticles(MaxParticles), particleLifetime(0.5f), particleSize(0.1f)
{

}

glm::vec3 pos_generator(glm::vec3 frustum_center, float frustum_radius)
{
	/*
	 * make random distribution number for particle position
	 * this position must be in frustum-circle
	 */
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> distribution(0, 1);
	std::uniform_real_distribution<float> _radius(-frustum_radius, frustum_radius);

	float theta = 2 * M_PI * distribution(generator); // (rad = 0 ~ 6.28319~) , (deg = 0 ~ 360)
	float phi = acos(2 * distribution(generator) - 1); // -1 ~ 1 (rad = 3.141592 ~ 0), (deg = 180 ~ 0)

	float x = sin(phi) * cos(theta) * _radius(generator) + frustum_center.x;
	float y = sin(phi) * sin(theta) * _radius(generator) + frustum_center.y;
	float z = cos(phi) * _radius(generator) + frustum_center.z;

	//float theta = 2 * M_PI * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // (rad = 0 ~ 6.28319~) , (deg = 0 ~ 360)
	//float phi = acos(2 * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 1); // -1 ~ 1 (rad = 3.141592 ~ 0), (deg = 180 ~ 0)
	//
	//float LO = -frustum_radius;
	//float HI = frustum_radius;
	//float randSeed = LO + static_cast <float> (rand()) / (static_cast <float> (65534 / (HI - LO)));
	//
	//float x = sin(phi) * cos(theta) * randSeed + frustum_center.x;
	//float y = sin(phi) * sin(theta) * randSeed + frustum_center.y;
	//float z = cos(phi) * randSeed + frustum_center.z;


	return glm::vec3(x, y, z);
}

glm::vec2 theta_phi_generator(float frustum_radius)
{
	glm::vec2 result;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> distribution(0, 1);
	
	float theta = 2 * M_PI * distribution(generator); // (rad = 0 ~ 6.28319~) , (deg = 0 ~ 360)
	float phi = acos(2 * distribution(generator) - 1); // -1 ~ 1 (rad = 3.141592 ~ 0), (deg = 180 ~ 0)

	result.x = theta;
	result.y = phi;

	return result;
}

float radius_generator(float frustum_radius)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);

	std::uniform_real_distribution<float> _radius(-frustum_radius, frustum_radius);

	float result = _radius(generator);

	return result;
}

glm::vec3 pos_generator(float near_width, float near_height, glm::vec3 center)
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> _height(-near_height, near_height);
	std::uniform_real_distribution<float> _width(-near_width, near_width);

	float x = _width(generator) * _height(generator) + center.x;
	float y = _width(generator) * _height(generator) + center.y;
	float z = _width(generator) * _height(generator) + center.z;

	return glm::vec3(x, y, z);
}

float uniformRand(float min, float max)
{
	std::random_device rn;
	std::mt19937_64 rnd(rn());

	//std::uniform_int_distribution<int> range(min, max);
	std::uniform_real_distribution<float> range(min, max);

	return range(rnd);
}

void Particle::initBuffers(glm::vec3 frustum_center, float _radius, glm::vec3 right_plane_vec, glm::vec3 nbr, glm::vec3 left_plane_vec, glm::vec3 nbl)
{
	int size = nParticles * 3 * sizeof(GLfloat);

	glGenTextures(1, &tex_array);
	ktx::file::load("RainArray.ktx", tex_array);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex_array);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



	int j = 0;
	while (j < nParticles)
	{
		layer.push_back(glm::vec3(0, 0, rand() % 64));
		j++;
	}

	glGenVertexArrays(2, particleArray);

	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers
	glGenBuffers(1, &initPosBuf);      
	glGenBuffers(1, &layerBuf);
	
	glGenBuffers(1, &testBuf);

	glGenTransformFeedbacks(2, feedback);

	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[i]);

		glBindBuffer(GL_ARRAY_BUFFER, posBuf[i]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particle_randomPos.size() * 3, particle_randomPos.data(), GL_DYNAMIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nParticles * 3, 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[i]);

		glBindBuffer(GL_ARRAY_BUFFER, velBuf[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nParticles * 3, 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[i]);

		glBindBuffer(GL_ARRAY_BUFFER, age[i]);
		glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[i]);
	}

	// Create vertex arrays for each set of buffers
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindVertexArray(particleArray[i]);

		glBindBuffer(GL_ARRAY_BUFFER, posBuf[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, velBuf[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, age[i]);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		//glBindBuffer(GL_ARRAY_BUFFER, initPosBuf);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particle_randomPos.size() * 3, particle_randomPos.data(), GL_STATIC_DRAW);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
		//glEnableVertexAttribArray(3);  // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, initPosBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nParticles * 3, 0, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
		glEnableVertexAttribArray(3);  // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, layerBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * layer.size() * 3, layer.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(4);  // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, testBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, 0, GL_STATIC_DRAW);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
		glEnableVertexAttribArray(5);  // Vertex position
	}


	std::cout << index << std::endl;
	glBindVertexArray(0);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void Particle::updateParticles(ShaderProgram* shader, float deltaT)
{
	subroutineIndex = shader->subroutine("update");
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &subroutineIndex);
	
	glUniform1fv(shader->uniform("DeltaT"), 1, &deltaT);

	glBindBuffer(GL_ARRAY_BUFFER, initPosBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nParticles * 3, particle_randomPos.data(), GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * nParticles * 3, particle_randomPos.data());

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(particleArray[1 - drawBuf]);
	glVertexAttribDivisor(0, 0);  //every vertex....
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	//glVertexAttribDivisor(3, 0);
	glDrawArrays(GL_POINTS, 0, nParticles);
	glEndTransformFeedback();
}

int index = 0;
void Particle::updateParticles(ShaderProgram* shader, float deltaT, glm::vec3 center, float radius, glm::vec3 nbl, glm::vec3 ntr,
	glm::vec3 right_plane_vector, glm::vec3 left_plane_vector, glm::vec3 bottom_plane_vector, glm::vec3 top_plane_vector, float near_width, float near_height)
{

	//std::cout << "new update" << std::endl;
	subroutineIndex = shader->subroutine("update");
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &subroutineIndex);

	glUniform1fv(shader->uniform("DeltaT"), 1, &deltaT);

	int j = 0;
	std::vector<glm::vec3> rand_pos_data;
	rand_pos_data.clear();
	while (j < nParticles)
	{
		glm::vec3 rand_pos = pos_generator(center, radius);

		glm::vec3 v_particle_nbl = rand_pos - nbl;
		glm::vec3 v_particle_ntr = rand_pos - ntr;

		float par_nbl_dot_rightPlane_vec = glm::dot(v_particle_nbl, right_plane_vector);
		float par_ntr_dot_leftPlane_vec = glm::dot(v_particle_ntr, left_plane_vector);
		float par_ntr_dot_topPlane_vec = glm::dot(v_particle_ntr, top_plane_vector);
		float par_nbl_dot_bottomPlane_vec = glm::dot(v_particle_nbl, top_plane_vector);

		//if (par_nbl_dot_rightPlane_vec > 0 && par_ntr_dot_leftPlane_vec < 0 && par_ntr_dot_topPlane_vec < 0 && par_nbl_dot_bottomPlane_vec < 0)
		if (par_nbl_dot_rightPlane_vec > 0 && par_ntr_dot_leftPlane_vec < 0)
		{
			rand_pos_data.push_back(glm::vec3(rand_pos.x, rand_pos.y, rand_pos.z));

		}
		j++;
	}

	glBindBuffer(GL_ARRAY_BUFFER, initPosBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rand_pos_data.size() * 3, rand_pos_data.data(), GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * rand_pos_data.size() * 3, rand_pos_data.data());

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(particleArray[1 - drawBuf]);
	glVertexAttribDivisor(0, 0);  //every vertex....
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	//glVertexAttribDivisor(3, 0);
	glDrawArrays(GL_POINTS, 0, nParticles);
	glEndTransformFeedback();
	index++;
}

void Particle::renderParticles(ShaderProgram* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	subroutineIndex = shader->subroutine("render");
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &subroutineIndex);

	glUniformMatrix4fv(shader->uniform("Model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(shader->uniform("View"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader->uniform("Proj"), 1, GL_FALSE, glm::value_ptr(projection));

	//glUniform3fv(shader->uniform(""), 1, glm::value_ptr(particle_randomPos.data()));

	glUniform1f(shader->uniform("ParticleSize"), particleSize);
	glUniform1f(shader->uniform("ParticleLifetime"), particleLifetime);


	
	glDepthMask(GL_FALSE);
	glBindVertexArray(particleArray[drawBuf]);
	glVertexAttribDivisor(0, 1);  //every instance
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	//glVertexAttribDivisor(3, 1);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
