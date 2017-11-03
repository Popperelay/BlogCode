#pragma once
#include "shader.h"
#include <iostream>
using namespace std;
#include <string>
#include <sstream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLM
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;
#include <vector>
struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
};
struct Texture {
	GLuint id;
	string samplerName;
	string texPath;
};
class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	float shininess;
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, float shininess) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->shininess = shininess;
		PrepareBeforeDraw();
	}
	void Draw(shader myshader) {
		for (GLuint i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			glUniform1i(glGetUniformLocation(myshader.shaderProgram, ("material." + textures[i].samplerName).c_str()), i);
		}
		glUniform1f(glGetUniformLocation(myshader.shaderProgram, "material.shininess"), shininess);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	GLuint VAO, VBO, EBO;
	//在绘制之前的绑定、发送数据、解析等准备工作
	void PrepareBeforeDraw() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
};