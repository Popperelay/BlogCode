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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
void RenderQuad();
void RenderCube();

GLuint loadTexture(GLchar *vPath, bool IsGammaCorrection);

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

	GLuint WoodTexture = loadTexture("../Textures/wood.png", true);
	GLuint ContainerTexture = loadTexture("../Textures/container2.png", true);

	GLuint ColorBufferTextures[2];
	glGenTextures(2, ColorBufferTextures);
	for (GLuint i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, ColorBufferTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint DepthRenderBuffer = -1;
	glGenRenderbuffers(1, &DepthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLuint HDRFBO = -1;
	glGenFramebuffers(1, &HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBufferTextures[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ColorBufferTextures[1], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);
	GLuint Attachments[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, Attachments);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint PingpongColorBufferTextures[2];
	glGenTextures(2, PingpongColorBufferTextures);
	for (GLuint i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, PingpongColorBufferTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint PingpongFBO[2];
	glGenFramebuffers(2, PingpongFBO);
	for (int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, PingpongFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PingpongColorBufferTextures[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::vector<glm::vec3> LightPositions;
	LightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	LightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	LightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	LightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	std::vector<glm::vec3> LightColors;
	LightColors.push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	LightColors.push_back(glm::vec3(1.5f, 0.0f, 0.0f));
	LightColors.push_back(glm::vec3(0.0f, 0.0f, 1.5f));
	LightColors.push_back(glm::vec3(0.0f, 1.5f, 0.0f));
	
	shader RenderScene_Shader("RenderScene_VS.glsl", "RenderScene_FS.glsl");
	shader RenderLight_Shader("RenderLight_VS.glsl", "RenderLight_FS.glsl");
	shader GaussianBlur_Shader("GaussianBlur_VS.glsl", "GaussianBlur_FS.glsl");
	shader ScreenQuad_Shader("ScreenQuad_VS.glsl", "ScreenQuad_FS.glsl");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), GLfloat(WIDTH) / HEIGHT, 0.1f, 100.0f);
	mat4 Model;
	mat4 View;

	RenderScene_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));
	RenderLight_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(RenderLight_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));
	ScreenQuad_Shader.Use();
	glUniform1i(glGetUniformLocation(ScreenQuad_Shader.shaderProgram, "u_HDRTexture"), 0);
	glUniform1i(glGetUniformLocation(ScreenQuad_Shader.shaderProgram, "u_BluredTexture"), 1);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		View = mycamera.GetViewMatrix();

		glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene_Shader.Use();
		for (GLuint i = 0; i < LightPositions.size(); ++i)
		{
			glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, ("u_Lights[" + std::to_string(i) + "].Position").c_str()), 1, &LightPositions[i][0]);
			glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, ("u_Lights[" + std::to_string(i) + "].Color").c_str()), 1, &LightColors[i][0]);
		}
		glUniform3fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));

		//渲染一个大的cube作为地平面
		Model = mat4();
		Model = glm::translate(Model, vec3(0, -1, 0));
		Model = glm::scale(Model, vec3(12.5, 0.5, 12.5));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WoodTexture);
		RenderCube();

		//渲染多个cube作为场景中的物体
		glBindTexture(GL_TEXTURE_2D, ContainerTexture);
		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(0.0f, 1.5f, 0.0));
		Model = glm::scale(Model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 1.0));
		Model = glm::scale(Model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(-1.0f, -1.0f, 2.0));
		Model = glm::rotate(Model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(0.0f, 2.7f, 4.0));
		Model = glm::rotate(Model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		Model = glm::scale(Model, glm::vec3(1.25));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(-2.0f, 1.0f, -3.0));
		Model = glm::rotate(Model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		Model = glm::mat4();
		Model = glm::translate(Model, glm::vec3(-3.0f, 0.0f, 0.0));
		Model = glm::scale(Model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(RenderScene_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
		RenderCube();

		//渲染多个cube作为光源
		RenderLight_Shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(RenderLight_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));

		for (unsigned int i = 0; i < LightPositions.size(); i++)
		{
			Model = glm::mat4();
			Model = glm::translate(Model, glm::vec3(LightPositions[i]));
			Model = glm::scale(Model, glm::vec3(0.25f));
			glUniformMatrix4fv(glGetUniformLocation(RenderLight_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
			glUniform3fv(glGetUniformLocation(RenderLight_Shader.shaderProgram, "u_LightColor"), 1, &LightColors[i][0]);
			RenderCube();
		}

		//高斯模糊
		bool IsHorizontal = true, IsFirstIteration = true;
		GLuint Amount = 10;
		GaussianBlur_Shader.Use();
		for (GLuint i = 0; i < Amount; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, PingpongFBO[IsHorizontal]);
			glClear(GL_COLOR_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(GaussianBlur_Shader.shaderProgram, "u_IsHorizontal"), IsHorizontal);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, IsFirstIteration ? ColorBufferTextures[1] : PingpongColorBufferTextures[!IsHorizontal]);
			RenderQuad();
			IsHorizontal = !IsHorizontal;
			if (IsFirstIteration)
				IsFirstIteration = false;
		}
		
		//ImagePass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ScreenQuad_Shader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, PingpongColorBufferTextures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ColorBufferTextures[0]);
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

GLuint loadTexture(GLchar *vPath, bool IsGammaCorrection)
{
	stbi_set_flip_vertically_on_load(true);
	GLint ImageWidth = 0, ImageHeight = 0, NumChannels = 0;
	unsigned char *pImageData = stbi_load(vPath, &ImageWidth, &ImageHeight, &NumChannels, 0);
	_ASSERT(pImageData);
	GLenum InternalFormat;
	GLenum DataFormat;
	switch (NumChannels)
	{
	case 1:
		InternalFormat = GL_RED;
		DataFormat = GL_RED;
		break;
	case 2:
		InternalFormat = GL_RG;
		DataFormat = GL_RG;
		break;
	case 3:
		InternalFormat = IsGammaCorrection ? GL_SRGB : GL_RGB;
		DataFormat = GL_RGB;
		break;
	case 4:
		InternalFormat = IsGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		DataFormat = GL_RGBA;
		break;
	default:
		break;
	}

	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageWidth, ImageHeight, 0, DataFormat, GL_UNSIGNED_BYTE, pImageData);
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