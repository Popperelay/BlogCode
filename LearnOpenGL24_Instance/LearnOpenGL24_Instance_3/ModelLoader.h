#pragma once
#include "mesh.h"
#include "TextureLoader.h"
//Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
using namespace Assimp;
class Model {
public:
	Model(GLchar* path) {
		LoadModel(path);
	}
	void Draw(shader myshader) {
		for (GLuint i = 0; i < meshes.size(); i++) {
			meshes[i].Draw(myshader);
		}
	}

	const vector<Mesh>& getMeshes() { return meshes; }

private:
	vector<Mesh> meshes;
	string directory;
	const aiScene* scene;
	vector<Texture> loadedTextures;  //存储已经加载过的纹理

	void LoadModel(string path) {
		//用Assimp加载模型文件到场景对象中
		Importer importer;
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
			cout << "Error:Assimp:" << importer.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		TraverseNode(scene->mRootNode);
	}

	//遍历所有mesh
	void TraverseNode(aiNode* node) {
		//方法1：直接遍历场景对象指针scene中的所有mesh
		for (int i = 0; i < scene->mNumMeshes; i++) {
			meshes.push_back(ProcessMesh(scene->mMeshes[i]));
		}

		/*
		//方法2：递归每个结点
		//取出自身结点中的所有mesh
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(aimesh));
		}
		//递归遍历子节点
		for (GLuint i = 0; i < node->mNumChildren; i++) {
		TraverseNode(node->mChildren[i]);
		}
		*/
	}

	//把Assimp的aiMesh转换为我们自定义的Mesh
	Mesh ProcessMesh(aiMesh* aimesh) {
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;
		//获取aiMesh中的所有顶点数据（包括顶点位置、法线向量、纹理坐标）
		for (GLuint i = 0; i < aimesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.position = vec3(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z);
			vertex.normal = vec3(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z);
			if (aimesh->mTextureCoords[0]) {
				vertex.texCoords = vec2(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y);
			}
			else {
				vertex.texCoords = vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}
		//获取aiMesh中所有面片（这里就是三角形）的顶点索引
		for (GLuint i = 0; i < aimesh->mNumFaces; i++) {
			aiFace aiface = aimesh->mFaces[i];
			for (GLuint j = 0; j < aiface.mNumIndices; j++) {
				indices.push_back(aiface.mIndices[j]);
			}
		}
		//获取aiMesh的材质的所有材质纹理
		if (aimesh->mMaterialIndex >= 0) {
			aiMaterial* aimat = scene->mMaterials[aimesh->mMaterialIndex];
			//获取所有的漫反射纹理
			for (GLuint i = 0; i < aimat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
				aiString str;
				//获取第i个漫反射纹理的名称
				aimat->GetTexture(aiTextureType_DIFFUSE, i, &str);
				string texName = str.C_Str(); //纹理名称
				string texPath = directory + '/' + texName;  //纹理路径
				bool skip = false;  //纹理是否已经加载过
				for (int j = 0; j < loadedTextures.size(); j++) {
					if (texPath == loadedTextures[j].texPath) {
						skip = true;
						textures.push_back(loadedTextures[j]);
						break;
					}
				}
				if (!skip) {
					Texture texture;
					TextureLoader textureLoader;
					texture.id = textureLoader.LoadTexture(texPath.c_str());
					texture.samplerName = "diffuse_texture" + IntToString(i);
					texture.texPath = texPath;
					textures.push_back(texture);
					loadedTextures.push_back(texture);
				}
			}
			//获取所有的镜面纹理
			for (GLuint i = 0; i < aimat->GetTextureCount(aiTextureType_SPECULAR); i++) {
				aiString str;
				//获取第i个镜面反射纹理的存储路径
				aimat->GetTexture(aiTextureType_SPECULAR, i, &str);
				string texName = str.C_Str(); //纹理名称
				string texPath = directory + '/' + texName;  //纹理路径
				bool skip = false;
				for (int j = 0; j < loadedTextures.size(); j++) {
					if (texPath == loadedTextures[j].texPath) {
						skip = true;
						textures.push_back(loadedTextures[j]);
						break;
					}
				}
				if (!skip) {
					Texture texture;
					TextureLoader textureLoader;
					texture.id = textureLoader.LoadTexture(texPath.c_str());
					texture.samplerName = "specular_texture" + IntToString(i);
					texture.texPath = texPath;
					textures.push_back(texture);
					loadedTextures.push_back(texture);
				}
			}
		}
		float shininess = 32;
		return Mesh(vertices, indices, textures, shininess);
	}

	//把int类型转换为string类型
	string IntToString(int number) {
		ostringstream outs;
		outs << number;
		return outs.str();
	}
};