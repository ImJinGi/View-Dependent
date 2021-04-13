#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Loader.h"
#include <vector>

class Rain
{
public:
	Rain();
	~Rain();
	void init();
	void draw() const;
public:
	GLuint posBuf[2], velBuf[2];
	GLuint particleArray[2];
	GLuint feedback[2], initVel, age[2];
	GLuint drawBuf, query;
	int nParticles;
	float rainAlpha;
	//std::vector<glm::vec3> particlePos;
	float particleLifetime;
};

