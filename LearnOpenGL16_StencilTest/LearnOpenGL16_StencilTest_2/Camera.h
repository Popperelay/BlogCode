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
	//摄像机位置
	vec3 cameraPos;
	//上向量
	vec3 upVector;
	//摄像机照射方向（观察坐标系-z轴）
	vec3 cameraFront;
	//摄像机右方向（观察坐标系+x轴）
	vec3 cameraRight;
	//摄像机俯仰角（弧度）
	GLfloat Pitch;
	//摄像机偏航角（弧度）
	GLfloat Yaw;
	//摄像机视野
	GLfloat cameraFov;
	//摄像机移动速度
	GLfloat cameraSpeed;
	//摄像机旋转灵敏度
	GLfloat sensitivity;
	//投影矩阵宽高比
	GLfloat WHPercent;
	//摄像机前后左右枚举
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	//摄像机类构造函数，用于初始化与摄像机有关的变量
	Camera(vec3 CameraPos = vec3(0.0f, 0.0f, 3.0f), vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f), vec3 UpVector = vec3(0.0f, 1.0f, 0.0f), GLfloat speed = 5.0f, GLfloat fov = 45.0f, GLfloat Sensitivity = 0.03f, GLfloat whPercent = 800 / 600);
	//根据按键状态移动摄像机
	void ProcessMovemnet(Camera_Movement direction, GLfloat deltaTime);
	//根据鼠标位置移动摄像机
	void ProcessRotate(double xOffset, double yOffset);
	//鼠标滚轮回调函数
	void ProcessScroll(double xoffset, double yoffset);
	//生成摄像机观察矩阵
	mat4 GetViewMatrix();
};