
//#define GLFW_INCLUDE_GLU

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "MyGlWindow.h"



MyGlWindow * win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;

//#define TARGET_FPS 15;
const int TARTGET_FPS = 60;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	win->setSize(width, height);
	win->setAspect(width / (float)height);

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}


void mouseDragging(double width, double height)
{

	//if (!ImGui::IsMouseHoveringAnyWindow()) 
	{
		if (lbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (mbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->zoom(fractionChangeY);
		}
		else if (rbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			win->m_viewer->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	}


	m_lastMouseX = cx;
	m_lastMouseY = cy;

}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "glfw is not initilaize" << std::endl;
	}

	std::cout << glfwGetVersionString() << std::endl;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_REFRESH_RATE, 15);


	int width = 800;
	int height = 800;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);


	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	// Setup ImGui binding
	//ImGui_ImplGlfwGL3_Init(window, true);

		// Setup ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	 // GL 3.0 + GLSL 130
	const char* glsl_version = "#version 430 core";

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glfwMakeContextCurrent(window);


	/* Make the window's context current */

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
		std::cout << "glewInit failed, aborting." << std::endl;
		return 0;
	}

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSwapInterval(0);  //enable vsync

	win = new MyGlWindow(width, height);

	bool show_test_window = true;
	bool show_another_window = false;


	double previousTime = glfwGetTime();
	int frameCount = 0;
	double lastTime = glfwGetTime();;


	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);


	glfwSetWindowTitle(window, "myGlWindow");
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		win->update();

		win->draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//glfwWaitEvents();

		mouseDragging(display_w, display_h);

	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
