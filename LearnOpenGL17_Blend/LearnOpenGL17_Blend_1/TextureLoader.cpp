#include "TextureLoader.h"
#include <iostream>
using namespace std;
GLuint TextureLoader::LoadTexture(const GLchar* texturePath, GLint wrap_st) {
	GLuint texture;
	//生成纹理对象texture
	glGenTextures(1, &texture);
	//给纹理对象绑定目标（2D纹理）
	glBindTexture(GL_TEXTURE_2D, texture);
	int imageWidth, imageHeight;
	//加载纹理图像
	unsigned char* image = SOIL_load_image(texturePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	if (image == NULL) {
		cout << "Load iamge failed!" << endl;
	}
	//生成2D纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//释放纹理数据内存
	SOIL_free_image_data(image);
	//设置纹理s和t方向的环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_st);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_st);
	//设置纹理放大和缩写时的过滤方式（纹素过滤、mipmap边界过滤）
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//生成多级渐远纹理
	glGenerateMipmap(GL_TEXTURE_2D);
	//解绑纹理对象
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}