#include "camera.h"
//摄像机类构造函数，用于初始化与摄像机有关的变量
Camera::Camera(vec3 CameraPos, vec3 cameraFront, vec3 UpVector, GLfloat speed, GLfloat fov, GLfloat Sensitivity, GLfloat whPercent) {
	this->cameraSpeed = speed;
	this->cameraFov = fov;
	this->cameraPos = CameraPos;
	this->cameraFront = cameraFront;
	this->upVector = UpVector;
	this->cameraFront = normalize(cameraFront);
	this->cameraRight = normalize(cross(upVector, -cameraFront));
	this->Pitch = asin(cameraFront.y);
	this->Yaw = asin(cameraFront.z / cos(Pitch));
	this->sensitivity = Sensitivity;
	this->WHPercent = whPercent;
}
//根据按键状态移动摄像机
void Camera::ProcessMovemnet(Camera_Movement direction, GLfloat deltaTime) {
	float CameraSpeed = this->cameraSpeed * deltaTime;
	if (direction == FORWARD) {
		cameraPos += CameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
	}
	else if (direction == BACKWARD) {
		cameraPos -= CameraSpeed * vec3(cameraFront.x, 0, cameraFront.z);
	}
	else if (direction == RIGHT) {
		cameraPos += CameraSpeed * cameraRight;
	}
	else if (direction == LEFT) {
		cameraPos -= CameraSpeed * cameraRight;
	}
	else if (direction == UP) {
		cameraPos += CameraSpeed * vec3(0, 1, 0);
	}
	else if (direction == DOWN) {
		cameraPos -= CameraSpeed * vec3(0, 1, 0);
	}
}
//根据鼠标位置移动摄像机
void Camera::ProcessRotate(double xOffset, double yOffset) {
	xOffset *= sensitivity;
	yOffset *= sensitivity;
	//计算偏航角和俯仰角（弧度）
	Yaw += radians(xOffset);
	Pitch += radians(yOffset);
	//俯仰角限制在-89度~89度之间
	if (Pitch > radians(89.0f)) {
		Pitch = radians(89.0f);
	}
	else if (Pitch < radians(-89.0f)) {
		Pitch = radians(-89.0f);
	}
	//摄像机到目标的向量
	cameraFront.x = cos(Pitch) * cos(Yaw);
	cameraFront.y = sin(Pitch);
	cameraFront.z = cos(Pitch) * sin(Yaw);
	cameraFront = normalize(cameraFront);
	cameraRight = normalize(cross(upVector, -cameraFront));  //这句很重要
}
//鼠标滚轮回调函数
void Camera::ProcessScroll(double xoffset, double yoffset) {
	if (cameraFov >= 1.0f&& cameraFov <= 45.0f) {
		cameraFov -= yoffset;
	}
	if (cameraFov < 1.0f) {
		cameraFov = 1.0f;
	}
	else if (cameraFov > 45.0f) {
		cameraFov = 45.0f;
	}
}
//生成摄像机观察矩阵
mat4 Camera::GetViewMatrix() {
	return lookAt(cameraPos, cameraPos + cameraFront, upVector);
}