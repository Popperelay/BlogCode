#include "shader.h"
#include "camera.h"
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
void renderCube();
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

	stbi_set_flip_vertically_on_load(true);
	GLint ImageWidth = 0, ImageHeight = 0, NumChannels = 0;
	GLfloat *pImageData = stbi_loadf("../Textures/hdr/newport_loft.hdr", &ImageWidth, &ImageHeight, &NumChannels, 0);
	_ASSERT(pImageData);
	GLuint HdrTexture;
	glGenTextures(1, &HdrTexture);
	glBindTexture(GL_TEXTURE_2D, HdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ImageWidth, ImageHeight, 0, GL_RGB, GL_FLOAT, pImageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(pImageData);

	//cubemap环境贴图
	GLuint EnvCubeMap = -1;
	glGenTextures(1, &EnvCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubeMap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GLuint SphereMap2CubeMapFBO = -1;
	glGenFramebuffers(1, &SphereMap2CubeMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SphereMap2CubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, EnvCubeMap, 0);
	/*GLuint DepthRenderBuffer = -1;
	glGenRenderbuffers(1, &DepthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);*/
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat EnvCubeNearPlane = 0.1f, EnvCubeFarPlane = 25.0f;
	mat4 ShadowProjection = perspective(radians(90.0f), GLfloat(512) / 512, EnvCubeNearPlane, EnvCubeFarPlane);
	//从中心看向CubeMap六个面的投影矩阵*观察矩阵
	std::vector<mat4> VPMatrices4LookAtSixFaces;
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(1, 0, 0), vec3(0, -1, 0)));
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(-1, 0, 0), vec3(0, -1, 0)));
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(0, 1, 0), vec3(0, 0, 1)));
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(0, -1, 0), vec3(0, 0, -1)));
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(0, 0, 1), vec3(0, -1, 0)));
	VPMatrices4LookAtSixFaces.push_back(ShadowProjection * lookAt(vec3(0), vec3(0, 0, -1), vec3(0, -1, 0)));

	shader SphereMap2CubeMap_Shader("SphereMap2CubeMap_VS.glsl", "SphereMap2CubeMap_FS.glsl", "SphereMap2CubeMap_GS.glsl");
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//渲染一次，利用FBO将球面贴图转换为cubemap
	glBindFramebuffer(GL_FRAMEBUFFER, SphereMap2CubeMapFBO);
	glClearColor(0.01, 0.01, 0.01, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);
	SphereMap2CubeMap_Shader.Use();
	for (int i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(SphereMap2CubeMap_Shader.shaderProgram, ("u_VPMatrices4LookAtSixFaces[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, value_ptr(VPMatrices4LookAtSixFaces[i]));
	glBindTexture(GL_TEXTURE_2D, HdrTexture);
	renderCube();
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//卷积后的辐照度cubemap贴图
	GLuint IrradianceCubeMap = -1;
	glGenTextures(1, &IrradianceCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceCubeMap);
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	shader ConvolutionCubeMap_Shader("ConvolutionCubeMap_VS.glsl", "ConvolutionCubeMap_FS.glsl", "ConvolutionCubeMap_GS.glsl");

	//渲染一次，利用FBO来卷积cubemap环境贴图(可以重复利用将球面转换到cubemap的那个FBO，只是改一下绑定的纹理附件即可)
	glBindFramebuffer(GL_FRAMEBUFFER, SphereMap2CubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, IrradianceCubeMap, 0);
	glClearColor(0.01, 0.01, 0.01, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 32, 32);
	ConvolutionCubeMap_Shader.Use();
	for (int i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(ConvolutionCubeMap_Shader.shaderProgram, ("u_VPMatrices4LookAtSixFaces[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, value_ptr(VPMatrices4LookAtSixFaces[i]));
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubeMap);
	renderCube();
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mat4 Projection = perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);	

	shader ShowCubeMap_Shader("ShowCubeMap_VS.glsl", "ShowCubeMap_FS.glsl");
	ShowCubeMap_Shader.Use();
	glUniformMatrix4fv(glGetUniformLocation(ShowCubeMap_Shader.shaderProgram, "u_Projection"), 1, GL_FALSE, value_ptr(Projection));

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

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents();

		glClearColor(0.01, 0.01, 0.01, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//绘制cubemap
		ShowCubeMap_Shader.Use();
		mat4 View = mat4(mat3(mycamera.GetViewMatrix()));
		//mat4 View = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(ShowCubeMap_Shader.shaderProgram, "u_View"), 1, GL_FALSE, value_ptr(View));
		glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubeMap);
		renderCube();
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//绘制一系列球体
		PBR_Shader.Use();
		View = mycamera.GetViewMatrix();
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
				Model = translate(Model, vec3((Col - ColCount / 2)*Spacing, (Row - RowCount / 2)*Spacing, -2.0f));
				glUniformMatrix4fv(glGetUniformLocation(PBR_Shader.shaderProgram, "u_Model"), 1, GL_FALSE, value_ptr(Model));
				glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceCubeMap);
				renderSphere();
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
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
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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