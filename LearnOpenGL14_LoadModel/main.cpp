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
//窗口尺寸
const GLuint WIDTH = 800, HEIGHT = 600;
bool keys[1024];
//当前帧与上一帧的时间差
GLfloat deltaTime = 0.0f;
//上一帧开始的时间
GLfloat lastTime = 0.0f;
//上一帧鼠标位置
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
//定义摄像机类Camera的对象
Camera mycamera;
//函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
int main() {
	//初始化GLFW
	glfwInit();
	//GLFW配置
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心开发模式
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //窗口尺寸不可变
											  //创建能够使用GLFW函数的窗口对象（指针）
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//创建窗口环境
	glfwMakeContextCurrent(window);
	//注册按键回调函数
	glfwSetKeyCallback(window, key_callback);
	//注册鼠标位置回调函数
	glfwSetCursorPosCallback(window, cursor_callback);
	//隐藏鼠标
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//注册鼠标滚轮回调函数
	glfwSetScrollCallback(window, scroll_callback);
	//初始化GLEW
	glewExperimental = GL_TRUE; //保证使用现代技术管理OpenGL机能
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	//设置渲染窗口位置大小
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	//点光源位置
	vec3 pointLightPositions[] = {
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f)
	};
	//定义自定义着色器类shader的对象
	shader lightObject("LightingObjectVS.glsl", "LightingObjectFS.glsl");
	Model mymodel("nanosuit/nanosuit.obj");
	glEnable(GL_DEPTH_TEST);
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		//计算deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //检测是否有事件被触发（如按键按下、鼠标移动）
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//激活受光物体着色器程序对象
		lightObject.Use();
		//设置定向光源
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
		//设置点光源0
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].diffuse"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[0].quadratic"), 0.032);
		//设置点光源1
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].diffuse"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[1].quadratic"), 0.032);
		//设置点光源2
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].diffuse"), 0.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[2].quadratic"), 0.032);
		//设置点光源3
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "pointLights[3].quadratic"), 0.032);
		//设置聚光灯
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.position"), mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.lightAxis"), mycamera.cameraFront.x, mycamera.cameraFront.y, mycamera.cameraFront.z);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.inCutOff"), cos(radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.outCutOff"), cos(radians(17.5f)));
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.quadratic"), 0.032);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		//设置uniform变量观察位置viewPos
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "viewPos"), mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		mat4 view = mycamera.GetViewMatrix();
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		mat4 model;
		model = translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		mymodel.Draw(lightObject);
		do_movement();
		glfwSwapBuffers(window); //交换颜色缓存
	}
	//结束CLFW，释放由GLFW分配的资源
	glfwTerminate();
	return 0;
}
//按键回调函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	//响应键盘按键来设置相应按键状态
	if (action == GLFW_PRESS) {
		keys[key] = GL_TRUE;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = GL_FALSE;
	}
}
//根据按键状态移动摄像机
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
//鼠标位置回调函数
bool firstCursor = true;
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstCursor) {
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstCursor = false;
	}
	//计算前后两帧的鼠标位移量
	GLfloat xOffset = (xpos - lastCursorX);
	GLfloat yOffset = (lastCursorY - ypos);  //注意是相反的，因为鼠标坐标原点在窗口左上角
	lastCursorX = xpos;
	lastCursorY = ypos;
	mycamera.ProcessRotate(xOffset, yOffset);
}
//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mycamera.ProcessScroll(xoffset, yoffset);
}