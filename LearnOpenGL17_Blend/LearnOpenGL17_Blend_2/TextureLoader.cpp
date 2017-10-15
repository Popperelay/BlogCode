#include "TextureLoader.h"
#include <iostream>
using namespace std;
GLuint TextureLoader::LoadTexture(const GLchar* texturePath, GLint wrap_st) {
	GLuint texture;
	//�����������texture
	glGenTextures(1, &texture);
	//����������Ŀ�꣨2D����
	glBindTexture(GL_TEXTURE_2D, texture);
	int imageWidth, imageHeight;
	//��������ͼ��
	unsigned char* image = SOIL_load_image(texturePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	if (image == NULL) {
		cout << "Load iamge failed!" << endl;
	}
	//����2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//�ͷ����������ڴ�
	SOIL_free_image_data(image);
	//��������s��t����Ļ��Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_st);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_st);
	//��������Ŵ����дʱ�Ĺ��˷�ʽ�����ع��ˡ�mipmap�߽���ˣ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//���ɶ༶��Զ����
	glGenerateMipmap(GL_TEXTURE_2D);
	//����������
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}