#include "shader.h"
#include "camera.h"
#include <iostream>
#include <vector>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
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

int ToneMapppingType = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
void RenderQuad();
void RenderCube();

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

	GLuint WoodTexture = loadTexture("../Textures/wood.png");

	GLuint HDRTexture = -1;
	glGenTextures(1, &HDRTexture);
	glBindTexture(GL_TEXTURE_2D, HDRTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint DepthRenderBuffer = -1;
	glGenRenderbuffers(1, &DepthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLuint HDRFBO = -1;
	glGenFramebuffers(1, &HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<glm::vec3> LightPositions;
	LightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f));
	LightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	LightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	LightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
	std::vector<glm::vec3> LightColors;
	LightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	LightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	LightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	LightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
	
	shader RenderScene_Shader("RenderScene_VS.glsl", "RenderScene_FS.glsl");
	shader ScreenQuad_Shader("ScreenQuad_VS.glsl", "ScreenQuad_FS.glsl");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), GLfloat(WIDTH) / HEIGHT, 0.1f, 100.0f);
	mat4 Model;
	Model = glm::translate(Model, vec3(0, 0, 25.0));
	Model = glm::scale(Model, vec3(2.5, 2.5, 27.5));
	mat4 View;

	RenderScene_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		View = mycamera.GetViewMatrix();

		glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1, 1, 0, 1);
		RenderScene_Shader.Use();
		for (GLuint i = 0; i < LightPositions.size(); ++i)
		{
			glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, ("u_Lights[" + std::to_string(i) + "].Position").c_str()), 1, &LightPositions[i][0]);
			glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, ("u_Lights[" + std::to_string(i) + "].Color").c_str()), 1, &LightColors[i][0]);
		}
		glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));
		glBindTexture(GL_TEXTURE_2D, WoodTexture);
		RenderCube();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ScreenQuad_Shader.Use();
		glUniform1i(glGetUniformLocation(ScreenQuad_Shader.shaderProgram, "u_ToneMapppingType"), ToneMapppingType);
		glBindTexture(GL_TEXTURE_2D, HDRTexture);
		RenderQuad();

		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
void RenderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

GLuint loadTexture(GLchar *vPath)
{
	stbi_set_flip_vertically_on_load(true);
	GLint ImageWidth = 0, ImageHeight = 0, NumChannels = 0;
	unsigned char *pImageData = stbi_load(vPath, &ImageWidth, &ImageHeight, &NumChannels, 0);
	_ASSERT(pImageData);
	GLenum Format;
	switch (NumChannels)
	{
	case 1:
		Format = GL_RED;
		break;
	case 2:
		Format = GL_RG;
		break;
	case 3:
		Format = GL_RGB;
		break;
	case 4:
		Format = GL_RGBA;
		break;
	default:
		break;
	}

	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, Format, ImageWidth, ImageHeight, 0, Format, GL_UNSIGNED_BYTE, pImageData);
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

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		ToneMapppingType = 0;
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		ToneMapppingType = 1;
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		ToneMapppingType = 2;
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		ToneMapppingType = 3;
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		ToneMapppingType = 4;
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