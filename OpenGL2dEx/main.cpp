#include "util/game.h"
#include "util/logging.h"
#include "util/resource_mgr.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace util;

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// width of screen
constexpr unsigned int kScreenWidth{ 800 };
constexpr unsigned int kScreenHeight{ 600 };

Game g_breakout_{ kScreenWidth, kScreenHeight };

int main(int argc, char *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Breakout", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		ASSERT(false, "Failed to initialize GLAD");
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// OpenGL Configuration
	glViewport(0, 0, kScreenWidth, kScreenHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize game
	g_breakout_.initialize();

	// deltaTime variables
	auto delta_time = 0.0f;
	auto last_frame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		// calculate delta time
		auto current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		glfwPollEvents();

		// manage user input
		g_breakout_.process_input(delta_time);

		// update game state
		g_breakout_.update(delta_time);

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		g_breakout_.render();

		glfwSwapBuffers(window);
	}

	// delete all resources as loaded using manager
	ResourceManager::clear();

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			g_breakout_.set_key(key, true);
		}
		else if (action == GLFW_RELEASE)
		{
			g_breakout_.set_key(key, false);
		}
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; 
	// note that width and height will be significantly larger than
	// expected on retina displays
	glViewport(0, 0, width, height);
}