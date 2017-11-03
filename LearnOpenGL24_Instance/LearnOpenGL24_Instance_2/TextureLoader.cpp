#include "TextureLoader.h"
GLuint TextureLoader::LoadTexture(const GLchar* texturePath) {
	GLuint texture;
	//�����������texture
	glGenTextures(1, &texture);
	//����������Ŀ�꣨2D����
	glBindTexture(GL_TEXTURE_2D, texture);
	int imageWidth, imageHeight;
	//��������ͼ��
	unsigned char* image = SOIL_load_image(texturePath, &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	//����2D����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//�ͷ����������ڴ�
	SOIL_free_image_data(image);
	//��������s��t����Ļ��Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//��������Ŵ����дʱ�Ĺ��˷�ʽ�����ع��ˡ�mipmap�߽���ˣ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//���ɶ༶��Զ����
	glGenerateMipmap(GL_TEXTURE_2D);
	//����������
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}