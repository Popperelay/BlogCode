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

bool IsApplyParallaxMap = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
void renderQuad();

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

	/*GLuint WallTexture = loadTexture("../Textures/bricks2.jpg");
	GLuint WallNormalTexture = loadTexture("../Textures/bricks2_normal.jpg");
	GLuint WallDepthMap = loadTexture("../Textures/bricks2_disp.jpg");*/
	GLuint WallTexture = loadTexture("../Textures/wood.png");
	GLuint WallNormalTexture = loadTexture("../Textures/toy_box_normal.png");
	GLuint WallDepthMap = loadTexture("../Textures/toy_box_disp.png");

	shader Wall_Shader("Wall_VS.glsl", "Wall_FS.glsl");
	Wall_Shader.Use();
	glUniform1i(glGetUniformLocation(Wall_Shader.shaderProgram, "u_WallTexture"), 0);
	glUniform1i(glGetUniformLocation(Wall_Shader.shaderProgram, "u_WallNormalMap"), 1);
	glUniform1i(glGetUniformLocation(Wall_Shader.shaderProgram, "u_DepthMap"), 2);
	glUniform1f(glGetUniformLocation(Wall_Shader.shaderProgram, "u_DepthScale"), 0.1);	//0.01
	glUniformMatrix4fv(glGetUniformLocation(Wall_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, glm::value_ptr(Projection));
	mat4 Model;
	glUniformMatrix4fv(glGetUniformLocation(Wall_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, glm::value_ptr(Model));
	mat4 View;
	

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		View = mycamera.GetViewMatrix();

		glClearColor(0.01, 0.01, 0.01, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Wall_Shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(Wall_Shader.shaderProgram, "u_View"), 1, GL_FALSE, glm::value_ptr(View));
		glUniform3fv(glGetUniformLocation(Wall_Shader.shaderProgram, "u_ViewPos"), 1, &mycamera.cameraPos[0]);
		glUniform1i(glGetUniformLocation(Wall_Shader.shaderProgram, "u_IsApplyParallaxMap"), IsApplyParallaxMap);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WallTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, WallNormalTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, WallDepthMap);
		renderQuad();

		do_movement();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 nm(0.0, 0.0, 1.0);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// - triangle 1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// - triangle 2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		GLfloat quadVertices[] = {
			// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		IsApplyParallaxMap = !IsApplyParallaxMap;
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