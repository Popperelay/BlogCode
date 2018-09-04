#include "shader.h"
#include "camera.h"
#include "ModelLoader.h"
#include <iostream>
#include <vector>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
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

int IsApplyNormalMap = 1;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);

GLuint loadTexture(GLchar *vPath);

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

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), GLfloat(WIDTH) / HEIGHT, 0.1f, 100.0f);

	/*GLuint WallTexture = loadTexture("../Textures/brickwall.jpg");
	GLuint WallNormalTexture = loadTexture("../Textures/brickwall_normal.jpg");*/

	shader RenderModel_Shader("RenderModel_VS.glsl", "RenderModel_FS.glsl");
	RenderModel_Shader.Use();
	glUniform1i(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_WallTexture"), 0);
	glUniform1i(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_WallNormalMap"), 1);
	glUniformMatrix4fv(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, glm::value_ptr(Projection));
	mat4 Model;
	//Model = glm::rotate(Model, glm::radians(25.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, glm::value_ptr(Model));
	mat4 View;
	
	CModel ObjModel("../Objects/cyborg/cyborg.obj");

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		View = mycamera.GetViewMatrix();

		glClearColor(0.01, 0.01, 0.01, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderModel_Shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_View"), 1, GL_FALSE, glm::value_ptr(View));
		glUniform3fv(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);
		glUniform1i(glGetUniformLocation(RenderModel_Shader.shaderProgram, "u_IsApplyNormalMap"), IsApplyNormalMap);
		ObjModel.Draw(RenderModel_Shader);

		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

GLuint loadTexture(GLchar *vPath)
{
	stbi_set_flip_vertically_on_load(true);
	GLint ImageWidth = 0, ImageHeight = 0, NumChannels = 0;
	unsigned char *pImageData = stbi_load(vPath, &ImageWidth, &ImageHeight, &NumChannels, 0);
	_ASSERT(pImageData);
	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(pImageData);
	glBindTexture(GL_TEXTURE_2D, 0);
	return Texture;
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

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		IsApplyNormalMap = !IsApplyNormalMap;
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