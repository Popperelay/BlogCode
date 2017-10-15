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
	//立方体各面顶点的坐标、法线、纹理坐标
	GLfloat vertices[] = {
		//位置坐标           //法线     //纹理坐标
		//立方体前面
		0.5f, 0.5f, 0.5f,   0, 0, 1,   1.0f, 1.0f,
		0.5f,-0.5f, 0.5f,   0, 0, 1,   1.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,	 0, 0, 1,   0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,   0, 0, 1,   0.0f, 1.0f,
		//立方体后
		0.5f, 0.5f,-0.5f,   0, 0,-1,   1.0f, 1.0f,
		0.5f,-0.5f,-0.5f,   0, 0,-1,   1.0f, 0.0f,
		-0.5f,-0.5f,-0.5f,   0, 0,-1,   0.0f, 0.0f,
		-0.5f, 0.5f,-0.5f,   0, 0,-1,   0.0f, 1.0f,
		//立方体上面
		0.5f, 0.5f, 0.5f,   0, 1, 0,   1.0f, 1.0f,
		0.5f, 0.5f,-0.5f,   0, 1, 0,   1.0f, 0.0f,
		-0.5f, 0.5f,-0.5f,   0, 1, 0,   0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,   0, 1, 0,   0.0f, 1.0f,
		//立方体下面
		0.5f,-0.5f, 0.5f,   0,-1, 0,   1.0f, 1.0f,
		0.5f,-0.5f,-0.5f,   0,-1, 0,   1.0f, 0.0f,
		-0.5f,-0.5f,-0.5f,   0,-1, 0,   0.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,   0,-1, 0,   0.0f, 1.0f,
		//立方体右面
		0.5f, 0.5f, 0.5f,   1, 0, 0,   1.0f, 1.0f,
		0.5f, 0.5f,-0.5f,   1, 0, 0,   1.0f, 0.0f,
		0.5f,-0.5f,-0.5f,   1, 0, 0,   0.0f, 0.0f,
		0.5f,-0.5f, 0.5f,   1, 0, 0,   0.0f, 1.0f,
		//立方体左面
		-0.5f, 0.5f, 0.5f,  -1, 0, 0,   1.0f, 1.0f,
		-0.5f, 0.5f,-0.5f,  -1, 0, 0,   1.0f, 0.0f,
		-0.5f,-0.5f,-0.5f,  -1, 0, 0,   0.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,  -1, 0, 0,   0.0f, 1.0f
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	//受光物体VAO
	GLuint lightObjectVAO;
	glGenVertexArrays(1, &lightObjectVAO);
	glBindVertexArray(lightObjectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	TextureLoader textureLoader;
	GLuint diffuseMap = textureLoader.LoadTexture("container2.png");
	GLuint specularMap = textureLoader.LoadTexture("container2_specular.png");
	//10个木箱子的位移（世界坐标）
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
	//点光源位置
	vec3 pointLightPositions[] = {
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f)
	};
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
		//观察矩阵
		mat4 view = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightSource.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		//投影矩阵
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightSource.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//绑定VAO，完成顶点输入初始化
		glBindVertexArray(lightSourceVAO);
		//绘图
		for (int i = 0; i < 4; i++) {
			mat4 lightSource_model;
			lightSource_model = translate(lightSource_model, pointLightPositions[i]);
			lightSource_model = scale(lightSource_model, vec3(0.2f));
			glUniformMatrix4fv(glGetUniformLocation(lightSource.shaderProgram, "model"), 1, GL_FALSE, value_ptr(lightSource_model));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		//解绑VAO
		glBindVertexArray(0);
		//激活受光物体着色器程序对象
		lightObject.Use();
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//设置定向光源
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
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
		//设置材质的环境属性和漫反射属性（都来自漫反射贴图，这里设置的是采样器/纹理单元的值）
		glUniform1i(glGetUniformLocation(lightObject.shaderProgram, "material.diffuse"), 0);
		//设置材质的镜面反射属性（来自镜面贴图）
		glUniform1i(glGetUniformLocation(lightObject.shaderProgram, "material.specular"), 1);
		//设置材质的镜面高光系数属性
		glUniform1f(glGetUniformLocation(lightObject.shaderProgram, "material.shininess"), 32.0f);

		//激活纹理单元0并绑定
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		//激活纹理单元1并绑定
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glBindVertexArray(lightObjectVAO);
		for (int i = 0; i < 10; i++) {
			mat4 lightObject_model;
			lightObject_model = translate(lightObject_model, cubeTranlate[i]);
			lightObject_model = rotate(lightObject_model, radians(105.0f), vec3(0.5f, 1.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "model"), 1, GL_FALSE, value_ptr(lightObject_model));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
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