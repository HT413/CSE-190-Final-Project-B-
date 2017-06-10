#ifndef GLOBAL_DECLS
#define GLOBAL_DECLS

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

#define GLM_FORCE_RADIANS // Force GLM to use radians
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <vector>

bool checkFramebufferStatus(GLenum);
//bool checkGlError();
void glDebugCallbackHandler(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);

#define WINDOW_TITLE "Final Project"
#define PI 3.14159265359f
#define PROGERR(x) getchar(); exit(x);

extern int objCount;
extern bool isGameOver;

#endif
