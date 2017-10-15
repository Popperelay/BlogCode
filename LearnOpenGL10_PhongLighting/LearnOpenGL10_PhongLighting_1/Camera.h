#pragma once
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLM
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;
class Camera {
public:
	//�����λ��
	vec3 cameraPos;
	//������
	vec3 upVector;
	//��������䷽�򣨹۲�����ϵ-z�ᣩ
	vec3 cameraFront;
	//������ҷ��򣨹۲�����ϵ+x�ᣩ
	vec3 cameraRight;
	//����������ǣ����ȣ�
	GLfloat Pitch;
	//�����ƫ���ǣ����ȣ�
	GLfloat Yaw;
	//�������Ұ
	GLfloat cameraFov;
	//������ƶ��ٶ�
	GLfloat cameraSpeed;
	//�������ת������
	GLfloat sensitivity;
	//ͶӰ�����߱�
	GLfloat WHPercent;
	//�����ǰ������ö��
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	//������๹�캯�������ڳ�ʼ����������йصı���
	Camera(vec3 CameraPos = vec3(0.0f, 0.0f, 3.0f), vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f), vec3 UpVector = vec3(0.0f, 1.0f, 0.0f), GLfloat speed = 5.0f, GLfloat fov = 45.0f, GLfloat Sensitivity = 0.03f, GLfloat whPercent = 800 / 600);
	//���ݰ���״̬�ƶ������
	void ProcessMovemnet(Camera_Movement direction, GLfloat deltaTime);
	//�������λ���ƶ������
	void ProcessRotate(double xOffset, double yOffset);
	//�����ֻص�����
	void ProcessScroll(double xoffset, double yoffset);
	//����������۲����
	mat4 GetViewMatrix();
};