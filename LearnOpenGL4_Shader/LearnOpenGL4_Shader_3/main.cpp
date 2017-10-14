#include "Shader.h"
#include<iostream>
//GLEW
#define GLEW_STATIC
#include<GL/glew.h>
//GLFW
#include<GLFW/glfw3.h>
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
	//三角形顶点的坐标和颜色
	GLfloat vertices[] = {
		//位置坐标        //颜色
		-0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f,
		0.5f, -0.5f,0.0f, 0.0f,1.0f,0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,0.0f,1.0f
	};
	GLuint VAO, VBO;
	//生成顶点数组对象VAO，记录数据的存储和如何使用的细节信息
	glGenVertexArrays(1, &VAO);
	//生顶点缓冲对象VBO，负责实际数据的存储
	glGenBuffers(1, &VBO);
	//给顶点数组对象绑定目标(顶点数组，不需要参数)
	glBindVertexArray(VAO);
	//给顶点缓冲对象绑定目标（数组内存）
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//把顶点数据复制到显卡的缓冲内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//告诉OpenGL如何解析显存中的顶点位置属性数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//打开顶点位置属性数组
	glEnableVertexAttribArray(0);
	//告诉OpenGL如何解析显存中的顶点颜色属性数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//打开顶点颜色属性数组
	glEnableVertexAttribArray(1);
	//解绑VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解绑VAO

	//定义自定义着色器类shader的对象
	shader myshader("TriangleVS.glsl", "TriangleFS.glsl");
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //检测是否有事件被触发（按键按下、鼠标移动）
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//激活着色器程序对象
		myshader.Use();
		//绑定VAO，完成顶点输入初始化
		glBindVertexArray(VAO);
		//绘图
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//解绑VAO
		glBindVertexArray(0);
		glfwSwapBuffers(window); //交换颜色缓存
	}
	//删除VAO、VBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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