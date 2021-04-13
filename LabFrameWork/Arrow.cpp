
#include "Arrow.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

Arrow::Arrow()
{

}

Arrow::Arrow(float x1, float y1, float z1, float x2, float y2, float z2)
{
	setup(x1, y1, z1, x2, y2, z2);
}

void Arrow::setup(float x1, float y1, float z1, float x2, float y2, float z2)
{
	GLfloat lightPosition[] = {
		x1, y1, z1,
		x2, y2, z2,
	};

	GLfloat cube_colors[] = {
		// front colors
		1.0, 0.0, 0.0,

	};

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightPosition) * 3 * 2, lightPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, &cube_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(
		1, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Arrow::draw()
{
	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_LINES, 0, 3);
	glBindVertexArray(0);
}
