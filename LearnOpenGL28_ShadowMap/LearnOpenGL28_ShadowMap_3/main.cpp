#include "shader.h"
#include "TextureLoader.h"
#include "camera.h"
#include <iostream>
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
		//位置坐标          //纹理坐标   法线坐标
		//立方体前面
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,   0, 0, 1,
		0.5f,-0.5f, 0.5f,   1.0f,0.0f,   0, 0, 1,
		-0.5f,-0.5f, 0.5f,  0.0f,0.0f,   0, 0, 1,
		-0.5f, 0.5f, 0.5f,  0.0f,1.0f,   0, 0, 1,
		//立方体后面
		0.5f, 0.5f,-0.5f,   1.0f,1.0f,   0, 0,-1,
		0.5f,-0.5f,-0.5f,   1.0f,0.0f,   0, 0,-1,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,   0, 0,-1,
		-0.5f, 0.5f,-0.5f,  0.0f,1.0f,   0, 0,-1,
		//立方体上面
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,   0, 1, 0,
		0.5f, 0.5f,-0.5f,   1.0f,0.0f,   0, 1, 0,
		-0.5f, 0.5f,-0.5f,  0.0f,0.0f,   0, 1, 0,
		-0.5f, 0.5f, 0.5f,  0.0f,1.0f,   0, 1, 0,
		//立方体下面
		0.5f,-0.5f, 0.5f,   1.0f,1.0f,   0,-1, 0,
		0.5f,-0.5f,-0.5f,   1.0f,0.0f,   0,-1, 0,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,   0,-1, 0,
		-0.5f,-0.5f, 0.5f,  0.0f,1.0f,   0,-1, 0,
		//立方体右面
		0.5f, 0.5f, 0.5f,   1.0f,1.0f,   1, 0, 0,
		0.5f, 0.5f,-0.5f,   1.0f,0.0f,   1, 0, 0,
		0.5f,-0.5f,-0.5f,   0.0f,0.0f,   1, 0, 0,
		0.5f,-0.5f, 0.5f,   0.0f,1.0f,   1, 0, 0,
		//立方体左面
		-0.5f, 0.5f, 0.5f,  1.0f,1.0f,  -1, 0, 0,
		-0.5f, 0.5f,-0.5f,  1.0f,0.0f,  -1, 0, 0,
		-0.5f,-0.5f,-0.5f,  0.0f,0.0f,  -1, 0, 0,
		-0.5f,-0.5f, 0.5f,  0.0f,1.0f,  -1, 0, 0
	};

	GLuint cubeIndices[] = {
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
	//立方体cubeVAO和数据解析
	GLuint cubeVAO, cubeVBO, cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);

	GLfloat planeVertices[] = {
		//位置坐标            //纹理坐标   //法线坐标
		25.0f,  -0.5f,  25.0f,  2.0f, 0.0f,  0, 1, 0,
		-25.0f, -0.5f,  25.0f,  0.0f, 0.0f,  0, 1, 0,
		-25.0f, -0.5f, -25.0f,  0.0f, 2.0f,  0, 1, 0,
		25.0f,  -0.5f,  25.0f,  2.0f, 0.0f,  0, 1, 0,
		-25.0f, -0.5f, -25.0f,  0.0f, 2.0f,  0, 1, 0,
		25.0f,  -0.5f, -25.0f,  2.0f, 2.0f,  0, 1, 0
	};
	//地面planeVAO和数据解析
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);

	GLfloat windowQuadVertices[] = {
		//位置坐标   //纹理坐标
		1.0f, 1.0f,  1.0f,1.0f,
		1.0f,-1.0f,  1.0f,0.0f,
		-1.0f, 1.0f,  0.0f,1.0f,
		-1.0f, 1.0f,  0.0f,1.0f,
		1.0f,-1.0f,  1.0f,0.0f,
		-1.0f,-1.0f,  0.0f,0.0f
	};
	GLuint windowQuadVAO, windowQuadVBO;
	glGenVertexArrays(1, &windowQuadVAO);
	glBindVertexArray(windowQuadVAO);
	glGenBuffers(1, &windowQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, windowQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadVertices), windowQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	TextureLoader texLoader;
	GLuint cubeTexture = texLoader.LoadTexture("container2.png");
	GLuint planeTexture = texLoader.LoadTexture("container.jpg");

	GLuint DepthMap;
	glGenTextures(1, &DepthMap);
	glBindTexture(GL_TEXTURE_2D, DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat BorderColor[] = { 1.0,0.0,0.0,1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint DepthMapFBO;
	glGenFramebuffers(1, &DepthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	vec3 lightPos(4.0f, 3.0f, 3.0f);
	vec3 lightDirection(-1, -1, -1);

	shader ShadowMap_Shader("ShadowMapVS.glsl", "ShadowMapFS.glsl");
	shader GenerateDepthMap_Shader("GenerateDepthMapVS.glsl", "GenerateDepthMapFS.glsl");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		mat4 View = lookAt(lightPos, lightPos + lightDirection, vec3(0, 1, 0));
		mat4 Projection = ortho(-5.0, 5.0, -5.0, 5.0, 0.1, 20.0);
		mat4 LightSpaceVP = Projection * View;
		mat4 CubeModel;
		CubeModel = translate(CubeModel, glm::vec3(-1.0f, 0.0f, -1.0f));
		mat4 LightSpaceMVPCube = LightSpaceVP * CubeModel;
		mat4 PlaneModel;
		PlaneModel = mat4();
		mat4 LightSpaceMVPPlane = LightSpaceVP * PlaneModel;

		//----渲染深度贴图----
		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		GenerateDepthMap_Shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(GenerateDepthMap_Shader.shaderProgram, "LightSpaceMVP"), 1, GL_FALSE, value_ptr(LightSpaceMVPCube));
		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glUniformMatrix4fv(glGetUniformLocation(GenerateDepthMap_Shader.shaderProgram, "LightSpaceMVP"), 1, GL_FALSE, value_ptr(LightSpaceMVPPlane));
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glCullFace(GL_BACK);
		
		//----渲染场景和阴影----
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ShadowMap_Shader.Use();
		View = mycamera.GetViewMatrix();
		Projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));
		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));
		glUniform3f(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_LightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_LightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_ViewPos"), mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		glUniform3f(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_LightColor"), 1, 1, 1);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_DepthMapSampler2"), 0);
		glBindTexture(GL_TEXTURE_2D, DepthMap);

		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_LightSpaceMVP"), 1, GL_FALSE, value_ptr(LightSpaceMVPCube));
		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(CubeModel));
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_DiffuseMapSampler1"), 1);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_LightSpaceMVP"), 1, GL_FALSE, value_ptr(LightSpaceMVPPlane));
		glUniformMatrix4fv(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(PlaneModel));
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(ShadowMap_Shader.shaderProgram, "u_DiffuseMapSampler1"), 1);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		
		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
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