#include "shader.h"
#include "camera.h"
#include <iostream>
#include <vector>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;

const GLuint WIDTH = 800, HEIGHT = 600;
bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
Camera mycamera;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);

void renderSphere();

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	mat4 Projection = perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

	shader PBR_Shader("PBR_VS.glsl", "PBR_FS.glsl");
	PBR_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));
	glUniform3f(glGetUniformLocation(PBR_Shader.shaderProgram, "u_ObjectColor"), 0.5f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(PBR_Shader.shaderProgram, "u_AO"), 1.0f);

	int RowCount = 7, ColCount = 7;
	float Spacing = 2.5f;
	vec3 LightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	vec3 LightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		glClearColor(0.01, 0.01, 0.01, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		PBR_Shader.Use();
		mat4 View = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));
		glUniform3fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);

		//绘制接受光照的Row行 * Col列个球体
		mat4 Model;
		for (int Row = 0; Row < RowCount; ++Row)
		{
			glUniform1f(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Metalness"), float(Row) / RowCount);
			for (int Col = 0; Col < ColCount; ++Col)
			{
				glUniform1f(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Roughness"), clamp(float(Col) / ColCount, 0.1f, 1.0f));
				Model = mat4();
				Model = translate(Model, vec3((Col - ColCount / 2)*Spacing, (Row - RowCount / 2)*Spacing, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
				renderSphere();
			}
		}

		//绘制光源球
		for (int i = 0; i < sizeof(LightPositions) / sizeof(LightPositions[0]); ++i)
		{
			//vec3 NewPos = LightPositions[i] + vec3(sin(glfwGetTime()*5.0)*5.0, 0.0, 0.0);
			vec3 NewPos = LightPositions[i];
			glUniform3fv(glGetUniformLocation(PBR_Shader.shaderProgram, ("u_LightPos[" + std::to_string(i) + "]").c_str()), 1, &NewPos[0]);
			glUniform3fv(glGetUniformLocation(PBR_Shader.shaderProgram, ("u_LightColors[" + std::to_string(i) + "]").c_str()), 1, &LightColors[i][0]);

			Model = mat4();
			Model = translate(Model, NewPos);
			Model = scale(Model, vec3(0.5f));
			glUniformMatrix4fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
			renderSphere();
		}
		
		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

//程序计算球的顶点、索引，以及VAO
unsigned int SphereVAO = 0;
unsigned int IndexCount = 0;
void renderSphere()
{
	if (SphereVAO == 0)
	{
		glGenVertexArrays(1, &SphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		IndexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(SphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(SphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS) {
		keys[key] = GL_TRUE;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = GL_FALSE;
	}
}

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
	if (keys[GLFW_KEY_Q]) {
		mycamera.ProcessMovemnet(mycamera.UP, deltaTime);
	}
	if (keys[GLFW_KEY_E]) {
		mycamera.ProcessMovemnet(mycamera.DOWN, deltaTime);
	}
}

bool firstCursor = true;
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstCursor) {
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstCursor = false;
	}
	GLfloat xOffset = (xpos - lastCursorX);
	GLfloat yOffset = (lastCursorY - ypos);
	lastCursorX = xpos;
	lastCursorY = ypos;
	mycamera.ProcessRotate(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mycamera.ProcessScroll(xoffset, yoffset);
}