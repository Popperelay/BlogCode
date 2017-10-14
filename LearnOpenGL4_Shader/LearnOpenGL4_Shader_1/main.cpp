#include<iostream>
//GLEW
#define GLEW_STATIC
#include<GL/glew.h>
//GLFW
#include<GLFW/glfw3.h>
//���ڳߴ�
const GLuint WIDTH = 800, HEIGHT = 600;
//��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//������ɫ��
const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout (location=0) in vec3 position;\n"
"void main(){\n"
"gl_Position=vec4(position.x,position.y,position.z,1.0f);\n"
"}\0";
//������ɫ��
const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 ourColor;\n"
"void main(){\n"
"color=ourColor;\n"
"}\0";
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
	//ע��ص�����
	glfwSetKeyCallback(window, key_callback);
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
	//�����ζ�������
	GLfloat vertices[] = {
		-0.5f,-0.5f,0.0f,
		0.5f,-0.5f,0.0f,
		0.0f,0.5f,0.0f
	};
	GLuint VAO, VBO;
	//���ɶ����������VAO����¼���ݵĴ洢�����ʹ�õ�ϸ����Ϣ
	glGenVertexArrays(1, &VAO);
	//�����㻺�����VBO������ʵ�����ݵĴ洢
	glGenBuffers(1, &VBO);
	//��������������Ŀ��(�������飬����Ҫ����)
	glBindVertexArray(VAO);
	//�����㻺������Ŀ�꣨�����ڴ棩
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//�Ѷ������ݸ��Ƶ��Կ��Ļ����ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//����OpenGL���ʶ���Դ��еĶ�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//�򿪶�����������
	glEnableVertexAttribArray(0);
	//���VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//���VAO
	glBindVertexArray(0);
	//����������ɫ������
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//����ɫ��Դ�븽�ӵ���ɫ������
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//���붥����ɫ��
	glCompileShader(vertexShader);
	//�������Ƿ�ɹ�
	GLint success; //������
	GLchar infoLog[512]; //������Ϣ
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//����������ɫ������
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//��������ɫ��Դ�븽�ӵ�������ɫ������
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//����������ɫ��Դ��
	glCompileShader(fragmentShader);
	//����Ƿ����ɹ�
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//������ɫ���������
	GLuint shaderProgram = glCreateProgram();
	//����ɫ�����ӵ���ɫ����������ϣ�Ȼ����������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//����Ƿ����ӳɹ�
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << std::endl;
	}
	//ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //����Ƿ����¼����������������¡�����ƶ���
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//�ҵ�uniform��������ɫ����������е�λ��
		GLint ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// ������ɫ
		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = sin(timeValue) / 2 + 0.5;
		//��uniform������ֵ
		glUniform4f(ourColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//������ɫ���������
		glUseProgram(shaderProgram);
		//��VAO����ɶ��������ʼ��
		glBindVertexArray(VAO);
		//��ͼ
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//���VAO
		glBindVertexArray(0);
		glfwSwapBuffers(window); //������ɫ����
	}
	//ɾ��VAO��VBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//����CLFW���ͷ���GLFW�������Դ
	glfwTerminate();
	return 0;
}
//�����ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}