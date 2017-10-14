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
//顶点着色器
const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout (location=0) in vec3 position;\n"
"void main(){\n"
"gl_Position=vec4(position.x,position.y,position.z,1.0f);\n"
"}\0";
//像素着色器
const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 ourColor;\n"
"void main(){\n"
"color=ourColor;\n"
"}\0";
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
	//三角形顶点坐标
	GLfloat vertices[] = {
		-0.5f,-0.5f,0.0f,
		0.5f,-0.5f,0.0f,
		0.0f,0.5f,0.0f
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
	//告诉OpenGL如何识别显存中的顶点数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//打开顶点属性数组
	glEnableVertexAttribArray(0);
	//解绑VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解绑VAO
	glBindVertexArray(0);
	//创建顶点着色器对象
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//把着色器源码附加到着色器对象
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//编译顶点着色器
	glCompileShader(vertexShader);
	//检测编译是否成功
	GLint success; //编译结果
	GLchar infoLog[512]; //错误信息
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//创建像素着色器对象
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//把像素着色器源码附加到像素着色器对象
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//编译像素着色器源码
	glCompileShader(fragmentShader);
	//检测是否编译成功
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//创建着色器程序对象
	GLuint shaderProgram = glCreateProgram();
	//把着色器附加到着色器程序对象上，然后链接起来
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//检查是否链接成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << std::endl;
	}
	//删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//渲染循环（游戏循环）
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //检测是否有事件被触发（按键按下、鼠标移动）
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//找到uniform变量在着色器程序对象中的位置
		GLint ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// 更新颜色
		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = sin(timeValue) / 2 + 0.5;
		//给uniform变量赋值
		glUniform4f(ourColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//激活着色器程序对象
		glUseProgram(shaderProgram);
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