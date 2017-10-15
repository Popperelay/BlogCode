#include "camera.h"
//������๹�캯�������ڳ�ʼ����������йصı���
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
//���ݰ���״̬�ƶ������
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
//�������λ���ƶ������
void Camera::ProcessRotate(double xOffset, double yOffset) {
	xOffset *= sensitivity;
	yOffset *= sensitivity;
	//����ƫ���Ǻ͸����ǣ����ȣ�
	Yaw += radians(xOffset);
	Pitch += radians(yOffset);
	//������������-89��~89��֮��
	if (Pitch > radians(89.0f)) {
		Pitch = radians(89.0f);
	}
	else if (Pitch < radians(-89.0f)) {
		Pitch = radians(-89.0f);
	}
	//�������Ŀ�������
	cameraFront.x = cos(Pitch) * cos(Yaw);
	cameraFront.y = sin(Pitch);
	cameraFront.z = cos(Pitch) * sin(Yaw);
	cameraFront = normalize(cameraFront);
	cameraRight = normalize(cross(upVector, -cameraFront));  //������Ҫ
}
//�����ֻص�����
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
//����������۲����
mat4 Camera::GetViewMatrix() {
	return lookAt(cameraPos, cameraPos + cameraFront, upVector);
}