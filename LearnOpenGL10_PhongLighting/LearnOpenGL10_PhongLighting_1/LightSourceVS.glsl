//������ɫ��
#version 330 core
layout (location=0) in vec3 position;  //����λ�ñ���������λ��ֵΪ0
//ģ�;���
uniform mat4 model;
//�۲����
uniform mat4 view;
//ͶӰ����
uniform mat4 projection;

void main(){
	gl_Position=projection * view * model * vec4(position,1.0f);
}