#include "util/game.h"
#include "util/logging.h"
#include "util/gl_debug.h"
#include "util/resource_mgr.h"
#include "util/reset_gl_properties.h"

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

// TODO(sasiala): find a better way than a global variable
class ResetGlProperties : public IResetGlProperties {
private:
	void reset_fbo_impl() const override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void reset_viewport_impl() const override
	{
		glViewport(0, 0, kScreenWidth, kScreenHeight);
	}
} g_gl_property_resetter_; // class ResetGlProperties

Game g_breakout_{ g_gl_property_resetter_, kScreenWidth, kScreenHeight };

int main(int argc, char *argv[])
{
	glfwInit();
	// TODO(sasiala): debug callback requires >= 4.3
#ifdef UTIL_GL_DEBUG
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// TODO(sasiala): using the core profile causes gladLoadGLLoader to
	// give an error, GL_INVALID_ENUM, when it calls glGetIntegerV with
	// GL_NUM_EXTENSIONS as input.  I figured this out, because it looks 
	// like the core profile is more strict in that when something is 
	// deprecated it isn't allowed, while the compatibility profile will
	// keep some of this functionality around.  I think using compatibility
	// should be OK, but look into the advantages and disadvantages more
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, false);
#ifdef UTIL_GL_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	GLFWwindow* window = glfwCreateWindow(kScreenWidth, kScreenHeight, "Breakout", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
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
#ifdef UTIL_GL_DEBUG
	// enable debug callback if debugging is enabled
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		// make sure errors are displayed synchronously
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(util::gl_debug_output, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	// OpenGL Configuration
	g_gl_property_resetter_.reset_viewport();
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
		delta_time = static_cast<float>(current_frame) - last_frame;
		last_frame = static_cast<float>(current_frame);
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

		util::check_for_gl_errors();
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