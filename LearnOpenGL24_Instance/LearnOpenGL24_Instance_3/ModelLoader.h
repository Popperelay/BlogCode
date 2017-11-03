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
	vector<Texture> loadedTextures;  //�洢�Ѿ����ع�������

	void LoadModel(string path) {
		//��Assimp����ģ���ļ�������������
		Importer importer;
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
			cout << "Error:Assimp:" << importer.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		TraverseNode(scene->mRootNode);
	}

	//��������mesh
	void TraverseNode(aiNode* node) {
		//����1��ֱ�ӱ�����������ָ��scene�е�����mesh
		for (int i = 0; i < scene->mNumMeshes; i++) {
			meshes.push_back(ProcessMesh(scene->mMeshes[i]));
		}

		/*
		//����2���ݹ�ÿ�����
		//ȡ���������е�����mesh
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(aimesh));
		}
		//�ݹ�����ӽڵ�
		for (GLuint i = 0; i < node->mNumChildren; i++) {
		TraverseNode(node->mChildren[i]);
		}
		*/
	}

	//��Assimp��aiMeshת��Ϊ�����Զ����Mesh
	Mesh ProcessMesh(aiMesh* aimesh) {
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;
		//��ȡaiMesh�е����ж������ݣ���������λ�á������������������꣩
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
		//��ȡaiMesh��������Ƭ��������������Σ��Ķ�������
		for (GLuint i = 0; i < aimesh->mNumFaces; i++) {
			aiFace aiface = aimesh->mFaces[i];
			for (GLuint j = 0; j < aiface.mNumIndices; j++) {
				indices.push_back(aiface.mIndices[j]);
			}
		}
		//��ȡaiMesh�Ĳ��ʵ����в�������
		if (aimesh->mMaterialIndex >= 0) {
			aiMaterial* aimat = scene->mMaterials[aimesh->mMaterialIndex];
			//��ȡ���е�����������
			for (GLuint i = 0; i < aimat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
				aiString str;
				//��ȡ��i�����������������
				aimat->GetTexture(aiTextureType_DIFFUSE, i, &str);
				string texName = str.C_Str(); //��������
				string texPath = directory + '/' + texName;  //����·��
				bool skip = false;  //�����Ƿ��Ѿ����ع�
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
			//��ȡ���еľ�������
			for (GLuint i = 0; i < aimat->GetTextureCount(aiTextureType_SPECULAR); i++) {
				aiString str;
				//��ȡ��i�����淴������Ĵ洢·��
				aimat->GetTexture(aiTextureType_SPECULAR, i, &str);
				string texName = str.C_Str(); //��������
				string texPath = directory + '/' + texName;  //����·��
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

	//��int����ת��Ϊstring����
	string IntToString(int number) {
		ostringstream outs;
		outs << number;
		return outs.str();
	}
};