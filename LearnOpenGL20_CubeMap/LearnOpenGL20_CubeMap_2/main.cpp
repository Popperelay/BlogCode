#include "shader.h"
#include "TextureLoader.h"
#include "cubemapLoader.h"
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
	//立方体顶点的坐标、法线向量
	GLfloat cubeVertices[] = {
		//位置坐标          //法线
		//立方体前面
		0.5f, 0.5f, 0.5f,   0, 0, 1,
		0.5f,-0.5f, 0.5f,   0, 0, 1,
		-0.5f,-0.5f, 0.5f,  0, 0, 1,
		-0.5f, 0.5f, 0.5f,  0, 0, 1,
		//立方体后面
		0.5f, 0.5f,-0.5f,   0, 0,-1,
		0.5f,-0.5f,-0.5f,   0, 0,-1,
		-0.5f,-0.5f,-0.5f,  0, 0,-1,
		-0.5f, 0.5f,-0.5f,  0, 0,-1,
		//立方体上面
		0.5f, 0.5f, 0.5f,   0, 1, 0,
		0.5f, 0.5f,-0.5f,   0, 1, 0,
		-0.5f, 0.5f,-0.5f,  0, 1, 0,
		-0.5f, 0.5f, 0.5f,  0, 1, 0,
		//立方体下面
		0.5f,-0.5f, 0.5f,   0,-1, 0,
		0.5f,-0.5f,-0.5f,   0,-1, 0,
		-0.5f,-0.5f,-0.5f,  0,-1, 0,
		-0.5f,-0.5f, 0.5f,  0,-1, 0,
		//立方体右面
		0.5f, 0.5f, 0.5f,   1, 0, 0,
		0.5f, 0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f, 0.5f,   1, 0, 0,
		//立方体左面
		-0.5f, 0.5f, 0.5f, -1, 0, 0,
		-0.5f, 0.5f,-0.5f, -1, 0, 0,
		-0.5f,-0.5f,-0.5f, -1, 0, 0,
		-0.5f,-0.5f, 0.5f, -1, 0, 0
	};
	//顶点索引
	GLuint cubeIndices[] = {
		//立方体前面
		0,3,1,
		1,3,2,
		//立方体后面
		4,5,7,
		5,6,7,
		//立方体上面
		8,9,11,
		9,10,11,
		//立方体下面
		12,15,13,
		13,15,14,
		//立方体右面
		16,19,17,
		17,19,18,
		//立方体左面
		20,21,23,
		21,22,23
	};
	//立方体cubeVAO和数据解析
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);
	//天空盒的顶点坐标
	GLfloat skyboxVertices[] = {
		//位置坐标         
		//立方体前面
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		//立方体后面
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		//立方体上面
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		//立方体下面
		1.0f,-1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		//立方体右面
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		//立方体左面
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f
	};
	//天空盒的顶点索引
	GLuint skyboxIndices[] = {
		//立方体前面
		0,3,1,
		1,3,2,
		//立方体后面
		4,5,7,
		5,6,7,
		//立方体上面
		8,9,11,
		9,10,11,
		//立方体下面
		12,15,13,
		13,15,14,
		//立方体右面
		16,19,17,
		17,19,18,
		//立方体左面
		20,21,23,
		21,22,23
	};
	//天空盒skyboxVAO和数据解析
	GLuint skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &skyboxEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindVertexArray(0);
	//TextureLoader texLoader;
	//GLuint cubeTexture = texLoader.LoadTexture("container2.png");
	//立方体贴图的6个纹理的路径
	vector<const GLchar*> facePaths;
	facePaths.push_back("skybox/right.jpg");
	facePaths.push_back("skybox/left.jpg");
	facePaths.push_back("skybox/top.jpg");
	facePaths.push_back("skybox/bottom.jpg");
	facePaths.push_back("skybox/back.jpg");
	facePaths.push_back("skybox/front.jpg");
	CubemapLoader cubemapLoader;
	GLuint cubemap = cubemapLoader.loadCubemap(facePaths);
	//定义自定义着色器类shader的对象
	shader cube_shader("CubeVS.glsl", "CubeFS.glsl");
	shader skybox_shader("CubemapVS.glsl", "CubemapFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//深度测试函数
	glDepthFunc(GL_LEQUAL);
	//开启面剔除
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		//计算deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //检测是否有事件被触发（如按键按下、鼠标移动）
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Draw skybox
		glDisable(GL_CULL_FACE);
		//glDepthMask(GL_FALSE);
		skybox_shader.Use();
		mat4 view = mat4(mat3(mycamera.GetViewMatrix()));
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skybox_shader.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skybox_shader.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		//Draw Cube
		cube_shader.Use();
		view = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(cube_shader.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cube_shader.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		mat4 model;
		model = translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = rotate(model, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glUniformMatrix4fv(glGetUniformLocation(cube_shader.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		glUniform3f(glGetUniformLocation(cube_shader.shaderProgram, "cameraPos"), mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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