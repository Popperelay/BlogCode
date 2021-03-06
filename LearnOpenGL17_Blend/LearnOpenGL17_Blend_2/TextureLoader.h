#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
class TextureLoader {
public:
	GLuint LoadTexture(const GLchar* texturePath, GLint wrap_st = GL_REPEAT);
};