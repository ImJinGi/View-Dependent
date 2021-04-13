#include "MyGlWindow.h"

#include <ctime>

#define PI 3.14159265
//#include <vector>

//#include  <glm/gtx/string_cast.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <glm/gtc/random.hpp>



static float DEFAULT_VIEW_POINT[3] = { 15, 15, 15 };
static float DEFAULT_VIEW_CENTER[3] = { 3.5, 0, 7.0f };
static float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

float deg_to_rad(float deg)
{
	return deg * PI / 180;
}

float rad_to_deg(float rad)
{
	return rad * 180 / PI;
}

MyGlWindow::MyGlWindow(int w, int h)
//==========================================================================
{
	srand(time(NULL));
	m_width = w;
	m_height = h;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	
	//
	near = 0.1f,/* far = 500.0f,*/ far_particle = 30.0f;
	fov = 45.0f;//deg_to_rad(45.0f);
	//

	m_viewer = new Viewer(viewPoint, viewCenter, upVector, fov, aspect);
	m_rotate = 0;

	TimingData::init();

	//-----------plane----------//
	m_plane = new Plane(40, 40, 2, 2);

	m_planeShader = new ShaderProgram();
	m_planeShader->initFromFiles("shaders/spotlightObject.vert", "shaders/spotlightObject.frag");
	m_planeShader->addUniform("ModelViewMatrix");
	m_planeShader->addUniform("NormalMatrix");
	m_planeShader->addUniform("MVP");

	m_planeShader->addUniform("Spot.position");
	m_planeShader->addUniform("Spot.intensity");
	m_planeShader->addUniform("Spot.direction");
	m_planeShader->addUniform("Spot.exponent");
	m_planeShader->addUniform("Spot.innerCutoff");
	m_planeShader->addUniform("Spot.outerCutoff");
	m_planeShader->addUniform("Spot.La");
	m_planeShader->addUniform("Spot.Ld");
	m_planeShader->addUniform("Spot.Ls");

	m_planeShader->addUniform("Ka");
	m_planeShader->addUniform("Kd");
	m_planeShader->addUniform("Ks");
	m_planeShader->addUniform("Shininess");

	//-----------streetLamp----------//
	m_streetLamp = new Mesh("Models/StreetLampOnly.fbx");
	m_streetLampShader = new ShaderProgram();
	std::cout << "StreetLight Shader" << std::endl;
	m_streetLampShader->initFromFiles("shaders/spotlightObject.vert", "shaders/spotlightObject.frag");

	m_streetLampShader->addUniform("ModelViewMatrix");
	m_streetLampShader->addUniform("NormalMatrix");
	m_streetLampShader->addUniform("MVP");

	m_streetLampShader->addUniform("Spot.position");
	m_streetLampShader->addUniform("Spot.intensity");
	m_streetLampShader->addUniform("Spot.direction");
	m_streetLampShader->addUniform("Spot.exponent");
	m_streetLampShader->addUniform("Spot.innerCutoff");
	m_streetLampShader->addUniform("Spot.outerCutoff");
	m_streetLampShader->addUniform("Spot.La");
	m_streetLampShader->addUniform("Spot.Ld");
	m_streetLampShader->addUniform("Spot.Ls");

	m_streetLampShader->addUniform("Ka");
	m_streetLampShader->addUniform("Kd");
	m_streetLampShader->addUniform("Ks");
	m_streetLampShader->addUniform("Shininess");
	std::cout << std::endl;

	//-----------particle----------//

	m_particles = new Particle();
	m_particles->initBuffers(frustum_center, frustum_radius, right_plane_vector, near_bottom_right, left_plane_vector, near_bottom_left);

	m_particleShader = new ShaderProgram();
	//m_particleShader->initFromFiles("shaders/Particle_spotlight.vert", "shaders/Particle_spotlight.frag");
	//m_particleShader->initFromFiles("shaders/Particle.vert", "shaders/Particle.frag");
	m_particleShader->initFromFiles("shaders/Particle_spotlight.vert", "shaders/Particle.frag");
	m_particleShader->addSubroutine(GL_VERTEX_SHADER, "render");
	m_particleShader->addSubroutine(GL_VERTEX_SHADER, "update");

	m_particleShader->addAttribute("VertexPosition");
	m_particleShader->addAttribute("VertexVelocity");
	m_particleShader->addAttribute("VertexAge");
	m_particleShader->addAttribute("VertexInitPos");
	m_particleShader->addAttribute("VertexLayer");

	m_particleShader->addUniform("ParticleLifetime");
	m_particleShader->addUniform("ParticleSize");

	m_particleShader->addUniform("Model");
	m_particleShader->addUniform("View");
	m_particleShader->addUniform("Proj");

	m_particleShader->addUniform("DeltaT");

	m_particleShader->addUniform("Spot.position");
	m_particleShader->addUniform("Spot.intensity");
	m_particleShader->addUniform("Spot.direction");
	m_particleShader->addUniform("Spot.exponent");
	m_particleShader->addUniform("Spot.innerCutoff");
	m_particleShader->addUniform("Spot.outerCutoff");
	m_particleShader->addUniform("Spot.La");
	m_particleShader->addUniform("Spot.Ld");
	m_particleShader->addUniform("Spot.Ls");
	////////////////////////////

	spotLightPos = glm::vec4(0.0f, 7.0f, 0.0f, 1.0f);
	//spotLightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
	spotLightIntensity = glm::vec3(0.1f, 0.1f, 0.1f);
	spotLightColor = glm::vec3(0.3, 0.3, 0.8);

	spotExponent = 10.0f;
	spotInnerCutoff = 15.0f;
	spotOuterCutoff = 45.0f;

	spotLightAmbient = glm::vec3(1.0f, 0.0f, 0.0f);
	spotLightDiffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	spotLightSpecular = glm::vec3(1.0f, 0.0f, 0.0f);

	pTime = 0.0f;

}

glm::mat4 lookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - look);
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));

	glm::mat4 R;

	R[0] = glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
	R[1] = glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
	R[2] = glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0.0f);
	R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 T;

	T[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	T[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	T[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	T[3] = glm::vec4(-pos.x, -pos.y, -pos.z, 1.0f);


	return R * T;
}


glm::mat4 perspective(float fov, float aspect, float n, float f)
{
	glm::mat4 P(0.0f);

	const float tanHalfFOV = 1.0f / (tan(glm::radians(fov) / 2.0f));

	float A = -(n + f) / (f - n);
	float B = -(2 * (n * f)) / (f - n);

	P[0] = glm::vec4(tanHalfFOV / aspect, 0, 0, 0);
	P[1] = glm::vec4(0, tanHalfFOV, 0, 0);
	P[2] = glm::vec4(0, 0, A, -1.0f);
	P[3] = glm::vec4(0, 0, B, 0.0f);


	return P;

}


void MyGlWindow::update()
{
	TimingData::get().update();

	deltaT = (float)TimingData::get().lastFrameDuration * 0.001f;

	if (ImGui::Begin("Program Setting"))
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
}

void MyGlWindow::drawPlane(ShaderProgram * shader, glm::mat4 & modelview, glm::mat3 & normalMatrix, glm::mat4 & mvp)
{
	glm::vec3 lp(10, 10, 10);
	m_planeShader->use();
	glUniform3fv(m_planeShader->uniform("LightLocation"), 1, glm::value_ptr(lp)); 

	glUniformMatrix4fv(m_planeShader->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelview));
	glUniformMatrix3fv(m_planeShader->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(m_planeShader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform3f(m_planeShader->uniform("Kd"), 0.6f, 0.6f, 0.6f);
	glUniform3f(m_planeShader->uniform("Ld"), 1, 1, 1);

	m_plane->draw();

	m_planeShader->disable();
}


void MyGlWindow::drawPlane(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection, glm::vec3 eye)
{
	glm::mat4 model{ 1.0f };

	model = glm::translate(model, glm::vec3(3.5f, 0.0f, 7.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	/*glm::vec3 Ka(0.24725, 0.1995, 0.0745);
	glm::vec3 Kd(0.75164, 0.60648, 0.22648);
	glm::vec3 Ks(0.628281, 0.555802, 0.366065);
*/
	glm::vec3 Ka(1.0, 1.0, 1.0);
	glm::vec3 Kd(1.0, 1.0, 1.0);
	glm::vec3 Ks(1.0, 1.0, 1.0);
	GLfloat shiness = 0;

	shader->use();
	glUniformMatrix4fv(shader->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mview));
	glUniformMatrix3fv(shader->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
	glUniformMatrix4fv(shader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform4fv(shader->uniform("Spot.position"), 1, glm::value_ptr(view * (spotLightPos + glm::vec4(6, 0, 6, 0))));
	glUniform3fv(shader->uniform("Spot.intensity"), 1, glm::value_ptr(spotLightIntensity));
	glUniform3fv(shader->uniform("Spot.direction"), 1, glm::value_ptr(view * glm::vec4(0, 0, 0, 1) - view * spotLightPos));
	glUniform1fv(shader->uniform("Spot.exponent"), 1, &spotExponent);
	glUniform1fv(shader->uniform("Spot.innerCutoff"), 1, &spotInnerCutoff);
	glUniform1fv(shader->uniform("Spot.outerCutoff"), 1, &spotOuterCutoff);
	glUniform3fv(shader->uniform("Spot.La"), 1, glm::value_ptr(spotLightAmbient));
	glUniform3fv(shader->uniform("Spot.Ld"), 1, glm::value_ptr(spotLightDiffuse));
	glUniform3fv(shader->uniform("Spot.Ls"), 1, glm::value_ptr(spotLightSpecular));

	glUniform3fv(shader->uniform("Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(shader->uniform("Kd"), 1, glm::value_ptr(Kd));
	glUniform3fv(shader->uniform("Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(shader->uniform("Shininess"), 1, &shiness);

	m_plane->draw();

	shader->disable();
}


void MyGlWindow::drawStreetLamp(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection, glm::vec3 eye)
{
	glm::mat4 model{ 1.0f };

	model = glm::translate(model, glm::vec3(4.0f, 0.0f, 7.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view * model;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	//glm::vec3 Ka(0.24725, 0.1995, 0.0745);
	//glm::vec3 Kd(0.75164, 0.60648, 0.22648);
	//glm::vec3 Ks(0.628281, 0.555802, 0.366065);

	glm::vec3 Ka(1.0, 1.0, 1.0);
	glm::vec3 Kd(1.0, 1.0, 1.0);
	glm::vec3 Ks(1.0, 1.0, 1.0);
	GLfloat shiness = 0;

	shader->use();
	glUniformMatrix4fv(shader->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mview));
	glUniformMatrix3fv(shader->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
	glUniformMatrix4fv(shader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	glUniform4fv(shader->uniform("Spot.position"), 1, glm::value_ptr(view *  (spotLightPos + glm::vec4(6, 0, 6, 0))));
	glUniform3fv(shader->uniform("Spot.intensity"), 1, glm::value_ptr(spotLightIntensity));
	glUniform3fv(shader->uniform("Spot.direction"), 1, glm::value_ptr(view * glm::vec4(m_viewer->getViewCenter(), 1) - view * spotLightPos));
	glUniform1fv(shader->uniform("Spot.exponent"), 1, &spotExponent);
	glUniform1fv(shader->uniform("Spot.innerCutoff"), 1, &spotInnerCutoff);
	glUniform1fv(shader->uniform("Spot.outerCutoff"), 1, &spotOuterCutoff);
	glUniform3fv(shader->uniform("Spot.La"), 1, glm::value_ptr(spotLightAmbient));
	glUniform3fv(shader->uniform("Spot.Ld"), 1, glm::value_ptr(spotLightDiffuse));
	glUniform3fv(shader->uniform("Spot.Ls"), 1, glm::value_ptr(spotLightSpecular));

	glUniform3fv(shader->uniform("Ka"), 1, glm::value_ptr(Ka));
	glUniform3fv(shader->uniform("Kd"), 1, glm::value_ptr(Kd));
	glUniform3fv(shader->uniform("Ks"), 1, glm::value_ptr(Ks));
	glUniform1fv(shader->uniform("Shininess"), 1, &shiness);

	for (int i = 0; i < m_streetLamp->meshEntries.size(); ++i)
	{
		m_streetLamp->meshEntries.at(i)->render();
	}

	shader->disable();
}


void MyGlWindow::draw(void)
{
	float aspect = (m_width / (float)(m_height));//(w / (float)h);

	v_right = glm::cross(m_viewer->getUpVector(), m_viewer->getViewDir());
	v_right = glm::normalize(v_right);

	far_height = 2 * tan(0.5 * fov) * far_particle;
	far_width = far_height * aspect;

	far_center = m_viewer->getViewPoint() + (m_viewer->getViewDir() * far_particle);
	far_bottom_right = far_center - (m_viewer->getUpVector() * (far_height / 2)) + (v_right * (far_width / 2));
	far_bottom_left = far_center - (m_viewer->getUpVector() * (far_height / 2)) - (v_right * (far_width / 2));
	far_top_right = far_center + (m_viewer->getUpVector() * (far_height / 2)) + (v_right * (far_width / 2));
	far_top_left = far_center + (m_viewer->getUpVector() * (far_height / 2)) - (v_right * (far_width / 2));

	near_height = 2 * tan(0.5 * fov) * near;
	near_width = near_height * aspect;

	near_center = m_viewer->getViewPoint() + (m_viewer->getViewDir() * near);
	near_bottom_right = near_center - (m_viewer->getUpVector() * (near_height / 2)) + (v_right * (near_width / 2));
	near_bottom_left = near_center - (m_viewer->getUpVector() * (near_height / 2)) - (v_right * (near_width / 2));
	near_top_right = near_center + (m_viewer->getUpVector() * (near_height / 2)) + (v_right * (near_width / 2));
	near_top_left = near_center + (m_viewer->getUpVector() * (near_height / 2)) - (v_right * (near_width / 2));

	nbr_fbr = far_bottom_right - near_bottom_right;
	nbr_ntr = near_top_right - near_bottom_right;

	right_plane_vector = glm::cross(nbr_fbr, nbr_ntr);
	right_plane_vector = glm::normalize(right_plane_vector);

	nbl_fbl = far_bottom_left - near_bottom_left;
	nbl_ntl = near_top_left - near_bottom_left;

	left_plane_vector = glm::cross(nbl_fbl, nbl_ntl);
	left_plane_vector = glm::normalize(left_plane_vector);

	nbl_nbr = near_bottom_right - near_bottom_left;
	bottom_plane_vector = glm::cross(nbl_nbr, nbl_fbl);
	bottom_plane_vector = glm::normalize(bottom_plane_vector);

	ntl_ntr = near_top_right - near_top_left;
	ntl_ftl = far_top_left - near_top_left;
	top_plane_vector = glm::cross(ntl_ftl, ntl_ntr);

	frustum_center = glm::vec3((far_center + near_center).x / 2, (far_center + near_center).y / 2, (far_center + near_center).z / 2);
	frustum_radius = glm::distance(far_center, near_center) / 2;

	glEnable(GL_RASTERIZER_DISCARD);

	m_particleShader->use();

	// Update pass
	m_particles->updateParticles(m_particleShader, deltaT, frustum_center, frustum_radius, near_bottom_left, near_top_right, 
		right_plane_vector, left_plane_vector, bottom_plane_vector, top_plane_vector, near_width, near_height);

	m_particleShader->disable();

	glDisable(GL_RASTERIZER_DISCARD);

	glClearColor(0.2f, 0.2f, 0.2f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 eye = m_viewer->getViewPoint(); // m_viewer->getViewPoint().x(), m_viewer->getViewPoint().y(), m_viewer->getViewPoint().z());
	glm::vec3 look = m_viewer->getViewCenter();   //(m_viewer->getViewCenter().x(), m_viewer->getViewCenter().y(), m_viewer->getViewCenter().z());
	glm::vec3 up = m_viewer->getUpVector(); // m_viewer->getUpVector().x(), m_viewer->getUpVector().y(), m_viewer->getUpVector().z());

	glm::mat4 model = m_model.getMatrix();
	glm::mat4 view = lookAt(eye, look, up);
	glm::mat4 projection = perspective(fov, 1.0f*m_width / m_height, near, 500.0f);

	glm::mat4 mview = view * model;
	glm::mat4 mvp = projection * view *  model;

	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));
	glViewport(0, 0, m_width, m_height);
	{
		drawPlane(m_planeShader, view, projection, m_viewer->getViewPoint());
		drawStreetLamp(m_streetLampShader, view, projection, m_viewer->getViewPoint());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_particleShader->use();

		m_particles->renderParticles(m_particleShader, model, view, projection);
		glUniform4fv(m_particleShader->uniform("Spot.position"), 1, glm::value_ptr(spotLightPos));
		glUniform3fv(m_particleShader->uniform("Spot.intensity"), 1, glm::value_ptr(spotLightIntensity));
		glUniform3fv(m_particleShader->uniform("Spot.direction"), 1, glm::value_ptr(spotLightPos - glm::vec4(0, 0, 0, 1)));
		glUniform1fv(m_particleShader->uniform("Spot.exponent"), 1, &spotExponent);
		glUniform1fv(m_particleShader->uniform("Spot.innerCutoff"), 1, &spotInnerCutoff);
		glUniform1fv(m_particleShader->uniform("Spot.outerCutoff"), 1, &spotOuterCutoff);
		glUniform3fv(m_particleShader->uniform("Spot.La"), 1, glm::value_ptr(spotLightAmbient));
		glUniform3fv(m_particleShader->uniform("Spot.Ld"), 1, glm::value_ptr(spotLightDiffuse));
		glUniform3fv(m_particleShader->uniform("Spot.Ls"), 1, glm::value_ptr(spotLightSpecular));
		glUniform3fv(m_particleShader->uniform("background_color"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));

		//glUniform4fv(shader->uniform("Spot.position"), 1, glm::value_ptr(view * spotLightPos));
		//glUniform3fv(shader->uniform("Spot.intensity"), 1, glm::value_ptr(spotLightIntensity));
		//glUniform3fv(shader->uniform("Spot.direction"), 1, glm::value_ptr(view * glm::vec4(0, 0, 0, 1) - view * spotLightPos));
		//glUniform1fv(shader->uniform("Spot.exponent"), 1, &spotExponent);
		//glUniform1fv(shader->uniform("Spot.innerCutoff"), 1, &spotInnerCutoff);
		//glUniform1fv(shader->uniform("Spot.outerCutoff"), 1, &spotOuterCutoff);
		//glUniform3fv(shader->uniform("Spot.La"), 1, glm::value_ptr(spotLightAmbient));
		//glUniform3fv(shader->uniform("Spot.Ld"), 1, glm::value_ptr(spotLightDiffuse));
		//glUniform3fv(shader->uniform("Spot.Ls"), 1, glm::value_ptr(spotLightSpecular));



		m_particleShader->disable();

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	// Swap buffers
	m_particles->drawBuf = 1 - m_particles->drawBuf;
}

MyGlWindow::~MyGlWindow()
{
	delete m_plane;
	delete m_planeShader;

	delete m_particleShader;
}

void MyGlWindow::initialize()
{



}

