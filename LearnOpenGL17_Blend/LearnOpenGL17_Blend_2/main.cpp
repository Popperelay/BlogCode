#include "shader.h"
#include "TextureLoader.h"
#include "camera.h"
#include <iostream>
using namespace std;
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
#include <vector>
#include <map>
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

	//��ƽ��Ķ������ԣ�λ�����ꡢ�������꣩
	GLfloat grass_vertices[] = {
		//����λ������    //��������
		0.5f, 0.5f, 0.0f, 1.0f,1.0f,
		0.5f,-0.5f, 0.0f, 1.0f,0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f,1.0f,
		0.5f,-0.5f, 0.0f, 1.0f,0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f,1.0f,
		-0.5f,-0.5f, 0.0f, 0.0f,0.0f,
	};
	//����Ķ������ꡢ��������
	GLfloat planeVertices[] = {
		//λ������            //��������
		5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		5.0f,  -0.5f, -5.0f,  2.0f, 2.0f
	};
	//��ƽ�涥�����ݵİ󶨺ͽ���
	GLuint grassVAO, grassVBO;
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);
	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass_vertices), grass_vertices, GL_STATIC_DRAW);
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
	//���ӵ�λ��
	vector<vec3> window_location;
	window_location.push_back(vec3(-1.5f, 0.0f, -0.48f));
	window_location.push_back(vec3(1.5f, 0.0f, 0.51f));
	window_location.push_back(vec3(0.0f, 0.0f, 0.7f));
	window_location.push_back(vec3(-0.3f, 0.0f, -2.3f));
	window_location.push_back(vec3(0.5f, 0.0f, -0.6f));
	TextureLoader texLoader;
	GLuint grassTexture = texLoader.LoadTexture("window.png", GL_CLAMP_TO_EDGE);
	GLuint planeTexture = texLoader.LoadTexture("container.jpg");
	//�����Զ�����ɫ����shader�Ķ���
	shader window_shader("WindowVS.glsl", "WindowFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//��Ȳ��Ժ���
	glDepthFunc(GL_LESS);
	//�������
	glEnable(GL_BLEND);
	//��Ϸ���
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������簴�����¡�����ƶ���
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window_shader.Use();
		mat4 view = mycamera.GetViewMatrix();
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(window_shader.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(window_shader.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//���Ƶ��棨����Ͳݵ�shader��һ���ģ����ǻ���һ��������ѣ�
		mat4 model = mat4();
		glUniformMatrix4fv(glGetUniformLocation(window_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//���ƴ���
		//�Ѷ������λ�ô���map���ݽṹ�У��Դ��ӵ�������ľ���Ϊ��
		map<float, vec3> sortedWindow;
		for (int i = 0; i < window_location.size(); i++) {
			GLfloat distance = length(mycamera.cameraPos - window_location[i]);
			sortedWindow[distance] = window_location[i];
		}
		glBindVertexArray(grassVAO);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		//������ʼ�ֵ��map����Զ�����ػ���͸������
		for (map<float, vec3>::reverse_iterator it = sortedWindow.rbegin(); it != sortedWindow.rend(); ++it) {
			model = mat4();
			model = translate(model, it->second);
			glUniformMatrix4fv(glGetUniformLocation(window_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
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