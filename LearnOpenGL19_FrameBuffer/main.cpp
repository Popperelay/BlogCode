#include "shader.h"
#include "TextureLoader.h"
#include "camera.h"
#include <iostream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
//SOIL
#include <SOIL/SOIL.h>
//GLM
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;
//���ڳߴ�
const GLuint WIDTH = 800, HEIGHT = 600;
bool keys[1024];
//��ǰ֡����һ֡��ʱ���
GLfloat deltaTime = 0.0f;
//��һ֡��ʼ��ʱ��
GLfloat lastTime = 0.0f;
//��һ֡���λ��
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
//�����������Camera�Ķ���
Camera mycamera;
//��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
int main() {
	//��ʼ��GLFW
	glfwInit();
	//GLFW����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //�ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ���Ŀ���ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //���ڳߴ粻�ɱ�
											  //�����ܹ�ʹ��GLFW�����Ĵ��ڶ���ָ�룩
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//�������ڻ���
	glfwMakeContextCurrent(window);
	//ע�ᰴ���ص�����
	glfwSetKeyCallback(window, key_callback);
	//ע�����λ�ûص�����
	glfwSetCursorPosCallback(window, cursor_callback);
	//�������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//ע�������ֻص�����
	glfwSetScrollCallback(window, scroll_callback);
	//��ʼ��GLEW
	glewExperimental = GL_TRUE; //��֤ʹ���ִ���������OpenGL����
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	//������Ⱦ����λ�ô�С
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//�����ζ�������ꡢ��������
	GLfloat cubeVertices[] = {
		//λ������          //��������
		//������ǰ��
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,
		0.5f,-0.5f, 0.5f,   1.0f,0.0f,
		-0.5f,-0.5f, 0.5f,   0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,   0.0f,1.0f,
		//���������
		0.5f, 0.5f,-0.5f,   1.0f,1.0f,
		0.5f,-0.5f,-0.5f,   1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,   0.0f,0.0f,
		-0.5f, 0.5f,-0.5f,   0.0f,1.0f,
		//����������
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,
		0.5f, 0.5f,-0.5f,   1.0f,0.0f,
		-0.5f, 0.5f,-0.5f,   0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,   0.0f,1.0f,
		//����������
		0.5f,-0.5f, 0.5f,   1.0f,1.0f,
		0.5f,-0.5f,-0.5f,   1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,   0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,   0.0f,1.0f,
		//����������
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,
		0.5f, 0.5f,-0.5f,   1.0f,0.0f,
		0.5f,-0.5f,-0.5f,   0.0f,0.0f,
		0.5f,-0.5f, 0.5f,   0.0f,1.0f,
		//����������
		-0.5f, 0.5f, 0.5f,   1.0f,1.0f,
		-0.5f, 0.5f,-0.5f,   1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,   0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,   0.0f,1.0f
	};
	//��������
	GLuint cubeIndices[] = {
		//������ǰ��
		0,3,1,
		1,3,2,
		//���������
		4,5,7,
		5,6,7,
		//����������
		8,9,11,
		9,10,11,
		//����������
		12,15,13,
		13,15,14,
		//����������
		16,19,17,
		17,19,18,
		//����������
		20,21,23,
		21,22,23
	};
	//����Ķ������ꡢ��������
	GLfloat planeVertices[] = {
		//λ������          //��������
		5.0f,-0.5f, 5.0f,  2.0f, 2.0f,
		5.0f,-0.5f,-5.0f,  2.0f, 0.0f,
		-5.0f,-0.5f, 5.0f,  0.0f, 2.0f,
		-5.0f,-0.5f, 5.0f,  0.0f, 2.0f,
		5.0f,-0.5f,-5.0f,  2.0f, 0.0f,
		-5.0f,-0.5f,-5.0f,  0.0f, 0.0f,
	};
	//�����ı��εĶ������ꡢ��������
	GLfloat windowQuadVertices[] = {
		//λ������
		1.0f, 1.0f,  1.0f,1.0f,
		1.0f,-1.0f,  1.0f,0.0f,
		-1.0f, 1.0f,  0.0f,1.0f,
		-1.0f, 1.0f,  0.0f,1.0f,
		1.0f,-1.0f,  1.0f,0.0f,
		-1.0f,-1.0f,  0.0f,0.0f
	};
	//������cubeVAO�����ݽ���
	GLuint cubeVAO, cubeVBO, cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);
	//����planeVAO�����ݽ���
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);
	//�����ı���windowQuadVAO�����ݽ���
	GLuint windowQuadVAO, windowQuadVBO;
	glGenVertexArrays(1, &windowQuadVAO);
	glBindVertexArray(windowQuadVAO);
	glGenBuffers(1, &windowQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, windowQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadVertices), windowQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
	//����FBO
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//����������
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//�����������ӵ�֡������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	//������Ⱦ�������
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//����Ⱦ������󸽼ӵ�֡������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//���֡�����Ƿ�����
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	TextureLoader texLoader;
	GLuint cubeTexture = texLoader.LoadTexture("container2.png");
	GLuint planeTexture = texLoader.LoadTexture("container.jpg");
	//�����Զ�����ɫ����shader�Ķ���
	shader depthTest_shader("PlaneAndCubeVS.glsl", "PlaneAndCubeFS.glsl");
	shader windowQuad_shader("ScreenQuadVS.glsl", "ScreenQuadFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//��Ȳ��Ժ���
	glDepthFunc(GL_LESS);
	//�������޳�
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������簴�����¡�����ƶ���

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		depthTest_shader.Use();
		mat4 view = mycamera.GetViewMatrix();
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(depthTest_shader.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(depthTest_shader.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//Draw Cube
		mat4 model;
		model = translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(glGetUniformLocation(depthTest_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//Draw Plane
		model = mat4();
		glUniformMatrix4fv(glGetUniformLocation(depthTest_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//��֡�����л�ΪĬ��֡����
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		windowQuad_shader.Use();
		glBindVertexArray(windowQuadVAO);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		do_movement();
		glfwSwapBuffers(window); //������ɫ����
	}
	//����CLFW���ͷ���GLFW�������Դ
	glfwTerminate();
	return 0;
}
//�����ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	//��Ӧ���̰�����������Ӧ����״̬
	if (action == GLFW_PRESS) {
		keys[key] = GL_TRUE;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = GL_FALSE;
	}
}
//���ݰ���״̬�ƶ������
void do_movement() {
	if (keys[GLFW_KEY_W]) {
		mycamera.ProcessMovemnet(mycamera.FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		mycamera.ProcessMovemnet(mycamera.BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		mycamera.ProcessMovemnet(mycamera.RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		mycamera.ProcessMovemnet(mycamera.LEFT, deltaTime);
	}
}
//���λ�ûص�����
bool firstCursor = true;
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstCursor) {
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstCursor = false;
	}
	//����ǰ����֡�����λ����
	GLfloat xOffset = (xpos - lastCursorX);
	GLfloat yOffset = (lastCursorY - ypos);  //ע�����෴�ģ���Ϊ�������ԭ���ڴ������Ͻ�
	lastCursorX = xpos;
	lastCursorY = ypos;
	mycamera.ProcessRotate(xOffset, yOffset);
}
//�����ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mycamera.ProcessScroll(xoffset, yoffset);
}