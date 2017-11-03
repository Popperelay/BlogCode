#include "shader.h"
#include "TextureLoader.h"
#include "camera.h"
#include "ModelLoader.h"
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
#include <string>
#include <sstream>
using namespace std;
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
string IntToString(int number);
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
	//1000����ʯ���ģ�;���
	GLuint amount = 1000;
	mat4* modelMatrices = new mat4[amount];
	srand(glfwGetTime());
	GLfloat radius = 20.0f;
	GLfloat offset = 2.5f;
	for (GLuint i = 0; i < amount; i++) {
		mat4 model;
		/********ƽ��*********/
		GLfloat angle = (GLfloat)i / amount* 360.0f;
		//���һ����-offset��offset֮��������
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle)*radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = displacement*0.4;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle)*radius + displacement;
		model = translate(model, vec3(x, y, z));
		/********��ת*********/
		GLfloat rotAngle = rand() % 360;
		model = rotate(model, rotAngle, vec3(0.4f, 0.6f, 0.8f));
		/********����*********/
		GLfloat scaleSize = (rand() % 20) / 100.0f + 0.05f;
		model = scale(model, vec3(scaleSize));
		modelMatrices[i] = model;
	}
	//�����Զ�����ɫ����shader�Ķ���
	shader planetAndRock_shader("PlanetVS.glsl", "PlanetFS.glsl");
	Model planet("planet/planet.obj");
	Model rock("rock/rock.obj");
	glEnable(GL_DEPTH_TEST);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������簴�����¡�����ƶ���
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		planetAndRock_shader.Use();
		mat4 view = mycamera.GetViewMatrix();
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(planetAndRock_shader.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(planetAndRock_shader.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//����С����
		mat4 model;
		model = translate(model, vec3(0.0f, -5.0f, 0.0f));
		model = scale(model, vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(glGetUniformLocation(planetAndRock_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		planet.Draw(planetAndRock_shader);
		//����С�����ܱߵ���ʯ��
		for (int i = 0; i < amount; i++) {
			glUniformMatrix4fv(glGetUniformLocation(planetAndRock_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(modelMatrices[i]));
			rock.Draw(planetAndRock_shader);
		}
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
//��int����ת��Ϊstring����
string IntToString(int number) {
	ostringstream outs;
	outs << number;
	return outs.str();
}