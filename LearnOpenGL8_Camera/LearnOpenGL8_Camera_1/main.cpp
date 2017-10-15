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
//窗口尺寸
const GLuint WIDTH = 800, HEIGHT = 600;
//函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
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
	//注册回调函数
	glfwSetKeyCallback(window, key_callback);
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
	//三角形顶点的坐标、颜色、纹理坐标
	GLfloat vertices[] = {
		//位置坐标          //颜色           //纹理坐标
		//立方体前面
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f, 0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//立方体后面
		0.5f, 0.5f,-0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f,-0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//立方体上面
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f, 0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//立方体下面
		0.5f,-0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//立方体右面
		0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
		//立方体左面
		-0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		-0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f,-0.5f, 0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f
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
	GLuint VAO, VBO, EBO;
	//生成顶点数组对象VAO，记录数据的存储和如何使用的细节信息
	glGenVertexArrays(1, &VAO);
	//生顶点缓冲对象VBO，负责顶点数据的存储
	glGenBuffers(1, &VBO);
	//生成索引缓冲对象EBO，负责索引数据的存储
	glGenBuffers(1, &EBO);
	//给顶点数组对象绑定目标(顶点数组，不需要参数)
	glBindVertexArray(VAO);
	//给顶点缓冲对象绑定目标（数组内存）
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//给索引缓冲对象绑定目标（索引数组内存）
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//把顶点数据复制到显卡的缓冲内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//把索引数据复制到显卡的缓冲内存中
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//告诉OpenGL如何解析显存中的顶点位置属性数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	//打开顶点位置属性数组
	glEnableVertexAttribArray(0);
	//告诉OpenGL如何解析显存中的顶点颜色属性数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//打开顶点颜色属性数组
	glEnableVertexAttribArray(1);
	//告诉OpenGL如何解析显存中的顶点纹理坐标属性数据
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//打开顶点纹理坐标属性数组
	glEnableVertexAttribArray(2);
	//解绑VBO、EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解绑VAO
	glBindVertexArray(0);
	TextureLoader textureLoader;
	GLuint texture1 = textureLoader.LoadTexture("container.jpg");
	GLuint texture2 = textureLoader.LoadTexture("awesomeface.png");

	//定义自定义着色器类shader的对象
	shader myshader("CameraVS.glsl", "CameraFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //检测是否有事件被触发（按键按下、鼠标移动）
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//激活着色器程序对象
		myshader.Use();
		//激活纹理单元
		glActiveTexture(GL_TEXTURE0);
		//获取uniform变量位置
		GLint uniformLocation1 = glGetUniformLocation(myshader.shaderProgram, "ourTexture1");
		//设置采样器的值（对应纹理单元值）
		glUniform1i(uniformLocation1, 0);
		//绑定纹理对象
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		GLint uniformLocation2 = glGetUniformLocation(myshader.shaderProgram, "ourTexture2");
		glUniform1i(uniformLocation2, 1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//投影矩阵
		mat4 projection;
		projection = perspective(radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

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
		//绑定VAO，完成顶点输入初始化
		glBindVertexArray(VAO);
		//以不同的位置和旋转角度绘制10个立方体
		for (int i = 0; i < 10; i++) {
			//模型矩阵
			mat4 model;
			model = translate(model, cubeTranlate[i]);
			model = rotate(model, (GLfloat)glfwGetTime()*radians(50.0f), vec3(0.5f, 1.0f, 1.0f));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
			//观察矩阵
			GLfloat radius = 10.0f;
			GLfloat cameraX = radius*sin(glfwGetTime());
			GLfloat cameraZ = radius*cos(glfwGetTime());
			mat4 view = lookAt(vec3(cameraX, 0.0f, cameraZ), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
			//绘图
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		//解绑VAO
		glBindVertexArray(0);
		glfwSwapBuffers(window); //交换颜色缓存
	}
	//删除VAO、VBO、EBO
	glDeleteVertexArrays(1, &VAO);
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
}