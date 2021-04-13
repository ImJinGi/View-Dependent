#ifndef __ARROW_H__
#define __ARROW_H__

/*
 * openGL
 */
#include <GL/glew.h>

 /*
  * glm
  */
#include <glm/glm.hpp>


class Arrow
{
public:
	Arrow();
	Arrow(float x1, float y1, float z1, float x2, float y2, float z2);
	void setup(float x1, float y1, float z1, float x2, float y2, float z2);
	void draw();

private:

	GLuint vaoHandle;
	GLuint vbo_position, vbo_colors;;
};

#endif // ! __ARROW_H__ 

