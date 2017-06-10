#include "main.h"
#include "Window.h"
#include "ServerGame.h"
#include "ClientGame.h"
#include <process.h>

GLFWwindow* window;

ServerGame * server;
ClientGame * client;

void serverLoop(void *);
void clientLoop();

void serverLoop(void * arg)
{
	while(true)
		server->update();
}

void clientLoop()
{
	while(true){}
}
void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks()
{
	// Set the key callback
	glfwSetKeyCallback(window, keyCallback);
	// Set the window resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	glfwSetCursorPosCallback(window, cursorCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
}

void setup_glew()
{
	// Initialize GLEW
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void setup_opengl_settings()
{
	// Setup GLEW
	setup_glew();
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Disable backface culling to render both sides of polygons
	glDisable(GL_CULL_FACE);
	// Allow the alpha channel
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ALPHA);
	// Set clear color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void print_versions()
{
	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	//If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

int main(void)
{
	// Create the GLFW window
	window = createWindow(1024, 768);
	// Print OpenGL and GLSL versions
	print_versions();
	// Setup callbacks
	setup_callbacks();
	// Setup OpenGL settings, including lighting, materials, etc.
	setup_opengl_settings();
	// Initialize objects/pointers for rendering
	initObjects();

	// Initialize game server and client
	server = new ServerGame();
	_beginthread(serverLoop, 0, (void*)12);
	client = new ClientGame();

	// Loop while GLFW window should stay open
	while(!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
		displayCallback(window);
		// Idle callback. Updating objects, etc. can be done here.
		update();
	}

	destroyObjects();
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();

	// Kill the server and client
	delete server;
	delete client;

	exit(EXIT_SUCCESS);
}
