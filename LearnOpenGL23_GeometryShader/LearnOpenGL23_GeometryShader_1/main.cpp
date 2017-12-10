#include "shader.h"
#include <iostream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>

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
	//隐藏鼠标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	GLfloat pointsVertices[] = {
		//位置坐标   //颜色
		0.5f, 0.5f,  1.0f, 0.0f, 0.0f,
		0.5f,-0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,-0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f,  1.0f, 1.0f, 0.0f
	};
	//点的pointsVAO和数据解析
	GLuint pointsVAO, pointsVBO;
	glGenVertexArrays(1, &pointsVAO);
	glBindVertexArray(pointsVAO);
	glGenBuffers(1, &pointsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), &pointsVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);
	//定义自定义着色器类shader的对象
	shader points_shader("PointsVS.glsl", "PointsFS.glsl", "PointsGS.glsl");
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		//计算deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //检测是否有事件被触发（如按键按下、鼠标移动）
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//mat4 view = mycamera.GetViewMatrix();
		//mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		points_shader.Use();
		glBindVertexArray(pointsVAO);
		glDrawArrays(GL_POINTS, 0, 4);
		glBindVertexArray(0);
		glfwSwapBuffers(window); //交换颜色缓存
	}
	//结束CLFW，释放由GLFW分配的资源
	glfwTerminate();
	return 0;
}
