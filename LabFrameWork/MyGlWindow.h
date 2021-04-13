//#define  FREEGLUT_LIB_PRAGMAS  0

#pragma warning(push)
#pragma warning(disable:4311)		// convert void* to long
#pragma warning(disable:4312)		// convert long to void*

// std c++
#include <iostream>
#include <string>
#include <stack>

// opengl
//#include "GL/glew.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/matrix_inverse.hpp"

// third party
#include "Loader.h"
#include "Viewer.h"
#include "ModelView.h"
#include "timing.h"

// objects
#include "Plane.h"
#include "Particle.h"

#include "Mesh.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#pragma warning(pop)


class MyGlWindow {
public:
	MyGlWindow(int w, int h);
	~MyGlWindow();
	void draw();
	void update();
	void setSize(int w, int h) { m_width = w; m_height = h; }
	void setAspect(float r) { m_viewer->setAspectRatio(r); }

	Viewer *m_viewer;
	float m_rotate;
private:
	int m_width;
	int m_height;
	void initialize();

	void drawPlane(ShaderProgram * shader, glm::mat4 & modelview, glm::mat3 & normalMatrix, glm::mat4 & mvp);
	void drawPlane(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection, glm::vec3 eye);
	void drawStreetLamp(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection, glm::vec3 eye);

	Model m_model;
	Plane *m_plane;
	Mesh *m_streetLamp;

	float pTime, deltaT;

	glm::vec3 camera_dir;

	glm::vec3 v_right;


	glm::vec3 far_center;
	glm::vec3 far_bottom_right;
	glm::vec3 far_bottom_left;
	glm::vec3 far_top_right;
	glm::vec3 far_top_left;

	glm::vec3 near_center;
	glm::vec3 near_bottom_right;
	glm::vec3 near_bottom_left;
	glm::vec3 near_top_right;
	glm::vec3 near_top_left;

	glm::vec3 v_U, v_V, side_V;

	// right side plane
	glm::vec3 nbr_fbr, nbr_ntr;
	glm::vec3 right_plane_vector;
	// left side plane
	glm::vec3 nbl_fbl, nbl_ntl;
	glm::vec3 left_plane_vector;
	// bottom plane
	glm::vec3 nbl_nbr;
	glm::vec3 bottom_plane_vector;
	// top plane
	glm::vec3 ntl_ntr, ntl_ftl;
	glm::vec3 top_plane_vector;
	

	float far_height, far_width;
	float near_height, near_width;
	float fov;
	float near, /*far,*/ far_particle;

	glm::vec3 frustum_center;
	float frustum_radius;
public:
	//shaders
	ShaderProgram * m_planeShader;
	ShaderProgram * m_particleShader;
	ShaderProgram * m_streetLampShader;

	Particle *m_particles;

	glm::vec4 spotLightPos;// = glm::vec4(0.0f, 7.0f, 0.0f, 1.0f);
	glm::vec3 spotLightIntensity;// = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 spotLightColor;// = glm::vec3(0.3, 0.3, 0.8);

	float spotExponent;// = 10.0f;
	float spotInnerCutoff;// = 20.0f;
	float spotOuterCutoff;// = 60.0f;

	glm::vec3 spotLightAmbient;// = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 spotLightDiffuse;// = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 spotLightSpecular;// = glm::vec3(1.0f, 1.0f, 1.0f);
};

