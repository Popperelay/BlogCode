#include "shader.h"
#include "TextureLoader.h"
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
#include <GLM/glm//gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;
//���ڳߴ�
const GLuint WIDTH = 800, HEIGHT = 600;
//�����λ��
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
//������
vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
//��������䷽�򣨹۲�����ϵ-z�ᣩ
vec3 cameraFront = normalize(vec3(0, 0, -1));
//�����+x�᷽�򣨹۲�����ϵ+x�ᣩ
vec3 cameraRight = normalize(cross(upVector, -cameraFront));
//�洢����״̬������/�ͷţ�������
bool keys[1024];
//��ǰ֡����һ֡��ʱ���
GLfloat deltaTime = 0.0f;
//��һ֡��ʼ��ʱ��
GLfloat lastTime = 0.0f;
//��һ֡���λ��
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
//����������ǣ����ȣ�
GLfloat Pitch = asin(cameraFront.y);
//�����ƫ���ǣ����ȣ�
//GLfloat Yaw = asin(sqrt((1 - cameraFront.y * cameraFront.y)));
GLfloat Yaw = asin(cameraFront.z / cos(Pitch));
//�������Ұ
GLfloat fov = 45.0f;
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
	//��������涥������ꡢ��ɫ����������
	GLfloat vertices[] = {
		//λ������          //��ɫ           //��������
		//������ǰ��
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f, 0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//���������
		0.5f, 0.5f,-0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f,-0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//����������
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f, 0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//����������
		0.5f,-0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//����������
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//����������
		-0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		-0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f
	};
	//��������
	GLuint indices[] = {
		//������ǰ��
		0,1,3,
		1,2,3,
		//���������
		4,5,7,
		5,6,7,
		//����������
		8,9,11,
		9,10,11,
		//����������
		12,13,15,
		13,14,15,
		//����������
		16,17,19,
		17,18,19,
		//����������
		20,21,23,
		21,22,23
	};
	GLuint VAO, VBO, EBO;
	//���ɶ����������VAO����¼���ݵĴ洢�����ʹ�õ�ϸ����Ϣ
	glGenVertexArrays(1, &VAO);
	//�����㻺�����VBO�����𶥵����ݵĴ洢
	glGenBuffers(1, &VBO);
	//���������������EBO�������������ݵĴ洢
	glGenBuffers(1, &EBO);
	//��������������Ŀ��(�������飬����Ҫ����)
	glBindVertexArray(VAO);
	//�����㻺������Ŀ�꣨�����ڴ棩
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��������������Ŀ�꣨���������ڴ棩
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//�Ѷ������ݸ��Ƶ��Կ��Ļ����ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//���������ݸ��Ƶ��Կ��Ļ����ڴ���
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//����OpenGL��ν����Դ��еĶ���λ����������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	//�򿪶���λ����������
	glEnableVertexAttribArray(0);
	//����OpenGL��ν����Դ��еĶ�����ɫ��������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//�򿪶�����ɫ��������
	glEnableVertexAttribArray(1);
	//����OpenGL��ν����Դ��еĶ�������������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//�򿪶�������������������
	glEnableVertexAttribArray(2);
	//���VBO��EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//���VAO
	glBindVertexArray(0);
	TextureLoader textureLoader;
	GLuint texture1 = textureLoader.LoadTexture("container.jpg");
	GLuint texture2 = textureLoader.LoadTexture("awesomeface.png");
	//�����Զ�����ɫ����shader�Ķ���
	shader myshader("CameraVS.glsl", "CameraFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������������¡�����ƶ���
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//������ɫ���������
		myshader.Use();
		//����������Ԫ
		glActiveTexture(GL_TEXTURE0);
		//��ȡuniform����λ��
		GLint uniformLocation1 = glGetUniformLocation(myshader.shaderProgram, "ourTexture1");
		//���ò�������ֵ����Ӧ������Ԫֵ��
		glUniform1i(uniformLocation1, 0);
		//����������
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		GLint uniformLocation2 = glGetUniformLocation(myshader.shaderProgram, "ourTexture2");
		glUniform1i(uniformLocation2, 1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		//ͶӰ����
		mat4 projection;
		projection = perspective(radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		GLint modelLocation = glGetUniformLocation(myshader.shaderProgram, "model");
		GLint viewLocation = glGetUniformLocation(myshader.shaderProgram, "view");
		GLint projectionLocation = glGetUniformLocation(myshader.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		vec3 cubeTranlate[] = {
			vec3(0.0f,  0.0f,  0.0f),
			vec3(2.0f,  5.0f, -15.0f),
			vec3(-1.5f, -2.2f, -2.5f),
			vec3(-3.8f, -2.0f, -12.3f),
			vec3(2.4f, -0.4f, -3.5f),
			vec3(-1.7f,  3.0f, -7.5f),
			vec3(1.3f, -2.0f, -2.5f),
			vec3(1.5f,  2.0f, -2.5f),
			vec3(1.5f,  0.2f, -1.5f),
			vec3(-1.3f,  1.0f, -1.5f)
		};
		//��VAO����ɶ��������ʼ��
		glBindVertexArray(VAO);
		//�Բ�ͬ��λ�ú���ת�ǶȻ���10��������
		for (int i = 0; i < 10; i++) {
			//ģ�;���
			mat4 model;
			model = translate(model, cubeTranlate[i]);
			//model = rotate(model, (GLfloat)glfwGetTime()*radians(50.0f), vec3(0.5f, 1.0f, 1.0f));
			model = rotate(model, radians(50.0f), vec3(0.5f, 1.0f, 1.0f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
			//�۲����
			mat4 view = lookAt(cameraPos, cameraPos + cameraFront, upVector);
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
			//��ͼ
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		do_movement();
		//���VAO
		glBindVertexArray(0);
		glfwSwapBuffers(window); //������ɫ����
	}
	//ɾ��VAO��VBO��EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W]) {
		cameraPos += cameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
	}
	if (keys[GLFW_KEY_S]) {
		cameraPos -= cameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
	}
	if (keys[GLFW_KEY_D]) {
		cameraPos += cameraSpeed * cameraRight;
	}
	if (keys[GLFW_KEY_A]) {
		cameraPos -= cameraSpeed * cameraRight;
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
	GLfloat sensitivity = 0.03f;
	GLfloat xOffset = (xpos - lastCursorX) * sensitivity;
	GLfloat yOffset = (lastCursorY - ypos) * sensitivity;
	lastCursorX = xpos;
	lastCursorY = ypos;
	//����ƫ���Ǻ͸����ǣ����ȣ�
	Yaw += radians(xOffset);
	Pitch += radians(yOffset);
	//������������-89��~89��֮��
	if (Pitch > radians(89.0f)) {
		Pitch = radians(89.0f);
	}
	else if (Pitch < radians(-89.0f)) {
		Pitch = radians(89.0f);
	}
	//�������Ŀ�������
	cameraFront.x = cos(Pitch) * cos(Yaw);
	cameraFront.y = sin(Pitch);
	cameraFront.z = cos(Pitch) * sin(Yaw);
	cameraFront = normalize(cameraFront);
	cameraRight = normalize(cross(upVector, -cameraFront));
}
//�����ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= 1.0f&& fov <= 45.0f) {
		fov -= yoffset;
	}
	if (fov < 1.0f) {
		fov = 1.0f;
	}
	else if (fov > 45.0f) {
		fov = 45.0f;
	}
}