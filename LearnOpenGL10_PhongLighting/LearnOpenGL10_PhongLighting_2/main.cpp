#include "shader.h"
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
//光源位置
vec3 lightPos(1.0f, 0.0f, 0.0f);
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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	//立方体各面顶点的坐标、法线
	GLfloat vertices[] = {
		//位置坐标           //法线  
		//立方体前面
		0.5f, 0.5f, 0.5f,   0, 0, 1,
		0.5f,-0.5f, 0.5f,   0, 0, 1,
		-0.5f,-0.5f, 0.5f,	 0, 0, 1,
		-0.5f, 0.5f, 0.5f,   0, 0, 1,
		//立方体后
		0.5f, 0.5f,-0.5f,   0, 0,-1,
		0.5f,-0.5f,-0.5f,   0, 0,-1,
		-0.5f,-0.5f,-0.5f,   0, 0,-1,
		-0.5f, 0.5f,-0.5f,   0, 0,-1,
		//立方体上面
		0.5f, 0.5f, 0.5f,   0, 1, 0,
		0.5f, 0.5f,-0.5f,   0, 1, 0,
		-0.5f, 0.5f,-0.5f,   0, 1, 0,
		-0.5f, 0.5f, 0.5f,   0, 1, 0,
		//立方体下面
		0.5f,-0.5f, 0.5f,   0,-1, 0,
		0.5f,-0.5f,-0.5f,   0,-1, 0,
		-0.5f,-0.5f,-0.5f,   0,-1, 0,
		-0.5f,-0.5f, 0.5f,   0,-1, 0,
		//立方体右面
		0.5f, 0.5f, 0.5f,   1, 0, 0,
		0.5f, 0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f, 0.5f,   1, 0, 0,
		//立方体左面
		-0.5f, 0.5f, 0.5f,  -1, 0, 0,
		-0.5f, 0.5f,-0.5f,  -1, 0, 0,
		-0.5f,-0.5f,-0.5f,  -1, 0, 0,
		-0.5f,-0.5f, 0.5f,  -1, 0, 0
	};
	//顶点索引
	GLuint indices[] = {
		//立方体前面
		0,1,3,
		1,2,3,
		//立方体后面
		4,5,7,
		5,6,7,
		//立方体上面
		8,9,11,
		9,10,11,
		//立方体下面
		12,13,15,
		13,14,15,
		//立方体右面
		16,17,19,
		17,18,19,
		//立方体左面
		20,21,23,
		21,22,23
	};
	//光源VAO
	GLuint lightSourceVAO, VBO, EBO;
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(lightSourceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	//受光物体VAO
	GLuint lightObjectVAO;
	glGenVertexArrays(1, &lightObjectVAO);
	glBindVertexArray(lightObjectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	//定义自定义着色器类shader的对象
	shader lightSource("LightSourceVS.glsl", "LightSourceFS.glsl");
	shader lightObject("LightingObjectVS.glsl", "LightingObjectFS.glsl");
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

		//激活光源着色器程序对象
		lightSource.Use();
		//获取3个uniform矩阵的位置
		GLint modelLocation = glGetUniformLocation(lightSource.shaderProgram, "model");
		GLint viewLocation = glGetUniformLocation(lightSource.shaderProgram, "view");
		GLint projectionLocation = glGetUniformLocation(lightSource.shaderProgram, "projection");
		//观察矩阵
		mat4 view = mycamera.GetViewMatrix();
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		//投影矩阵
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		//模型矩阵
		mat4 lightSource_model;
		lightSource_model = translate(lightSource_model, lightPos);
		lightSource_model = rotate(lightSource_model, radians(15.0f), vec3(0.5f, 1.0f, 1.0f));
		lightSource_model = scale(lightSource_model, vec3(0.5f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(lightSource_model));
		//绑定VAO，完成顶点输入初始化
		glBindVertexArray(lightSourceVAO);
		//绘图
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//解绑VAO
		glBindVertexArray(0);
		//激活受光物体着色器程序对象
		lightObject.Use();
		GLint lightColorLoc = glGetUniformLocation(lightObject.shaderProgram, "lightColor");
		GLint objectColorLoc = glGetUniformLocation(lightObject.shaderProgram, "objectColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		modelLocation = glGetUniformLocation(lightObject.shaderProgram, "model");
		viewLocation = glGetUniformLocation(lightObject.shaderProgram, "view");
		projectionLocation = glGetUniformLocation(lightObject.shaderProgram, "projection");
		mat4 lightObject_model;
		lightObject_model = translate(lightObject_model, vec3(-1, 0, -1));
		lightObject_model = rotate(lightObject_model, radians(45.0f), vec3(0.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(lightObject_model));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));

		//设置uniform变量光源位置lightPos
		GLint lightPosLoc = glGetUniformLocation(lightObject.shaderProgram, "lightPos");
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		//设置uniform变量观察位置viewPos
		GLint viewPosLoc = glGetUniformLocation(lightObject.shaderProgram, "viewPos");
		glUniform3f(viewPosLoc, mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		glBindVertexArray(lightObjectVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		do_movement();
		glfwSwapBuffers(window); //交换颜色缓存
	}
	//删除VAO、VBO、EBO
	glDeleteVertexArrays(1, &lightSourceVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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