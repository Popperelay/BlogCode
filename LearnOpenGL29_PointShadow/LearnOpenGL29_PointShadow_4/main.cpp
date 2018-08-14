#include "shader.h"
#include "TextureLoader.h"
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

void renderScene(const shader& vShader, int vCubeVAO);

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

	GLfloat cubeVertices[] = 
	{
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	//立方体cubeVAO和数据解析
	GLuint cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);

	TextureLoader texLoader;
	GLuint WoodTexture = texLoader.LoadTexture("../Textures/wood.png");

	//生成立方体贴图
	const GLuint ShadowWidth = 1024, ShadowHeight = 1024;
	GLuint DepthCubeMap = -1;
	glGenTextures(1, &DepthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, DepthCubeMap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, ShadowWidth, ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GLuint ShadowFBO = -1;
	glGenFramebuffers(1, &ShadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mat4 Model;
	mat4 View;
	mat4 Projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	vec3 lightPos(0.0f, 0.0f, 0.0f);

	

	shader ShadowCubeMap_Shader("ShadowCubeMapVS.glsl", "ShadowCubeMapFS.glsl", "ShadowCubeMapGS.glsl");
	ShadowCubeMap_Shader.Use();
	GLfloat ShadowNearPlane = 0.1f, ShadowFarPlane = 25.0f;
	mat4 ShadowProjection = perspective(radians(90.0f), GLfloat(ShadowWidth) / ShadowHeight, ShadowNearPlane, ShadowFarPlane);
	std::vector<mat4> ShadowVPMatrics;
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(1, 0, 0), vec3(0, -1, 0)));
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(-1, 0, 0), vec3(0, -1, 0)));
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(0, 1, 0), vec3(0, 0, 1)));
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, -1)));
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(0, 0, 1), vec3(0, -1, 0)));
	ShadowVPMatrics.push_back(ShadowProjection * lookAt(lightPos, lightPos + vec3(0, 0, -1), vec3(0, -1, 0)));
	for (int i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(ShadowCubeMap_Shader.shaderProgram, ("u_ShadowVPMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, value_ptr(ShadowVPMatrics[i]));
	glUniform3fv(glGetUniformLocation(ShadowCubeMap_Shader.shaderProgram, "u_LightPos"), 1, &lightPos[0]);
	glUniform1f(glGetUniformLocation(ShadowCubeMap_Shader.shaderProgram, "u_FarPlane"), ShadowFarPlane);
	
	shader ShadowLighting_Shader("ShadowLightingVS.glsl", "ShadowLightingFS.glsl");
	ShadowLighting_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));
	glUniform1i(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_DiffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_DepthCubeMap"), 1);
	glUniform1f(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_FarPlane"), ShadowFarPlane);
	glUniform3fv(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_LightPos"), 1, &lightPos[0]);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, ShadowWidth, ShadowHeight);
		ShadowCubeMap_Shader.Use();
		renderScene(ShadowCubeMap_Shader, cubeVAO);
		glViewport(0, 0, WIDTH, HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3, 0.4, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShadowLighting_Shader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WoodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthCubeMap);
		View = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));
		glUniform3fv(glGetUniformLocation(ShadowLighting_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);
		renderScene(ShadowLighting_Shader, cubeVAO);
		
		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void renderScene(const shader& vShader, int vCubeVAO)
{
	mat4 Model = mat4();
	Model = scale(Model, vec3(10));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glUniform1i(glGetUniformLocation(vShader.shaderProgram, "u_IsReverseNormal"), 1);
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUniform1i(glGetUniformLocation(vShader.shaderProgram, "u_IsReverseNormal"), 0);

	Model = mat4();
	Model = translate(Model, vec3(4.0f, -3.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	Model = mat4();
	Model = translate(Model, vec3(2.0f, 3.0f, 1.0));
	Model = scale(Model, vec3(1.5));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	Model = mat4();
	Model = translate(Model, vec3(-3.0f, -1.0f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	Model = mat4();
	Model = translate(Model, vec3(-1.5f, 1.0f, 1.5));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	Model = mat4();
	Model = translate(Model, vec3(-1.5f, 2.0f, -3.0));
	Model = glm::rotate(Model, 60.0f, normalize(vec3(1.0, 0.0, 1.0)));
	Model = glm::scale(Model, vec3(1.5));
	glUniformMatrix4fv(glGetUniformLocation(vShader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(vCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
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