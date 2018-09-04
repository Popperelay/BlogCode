#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

class TextureLoader
{
public:
	GLuint LoadTexture(const GLchar* texturePath);
};