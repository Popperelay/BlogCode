//������ɫ��
#version 330 core
layout (location=0) in vec3 position;  //λ�ñ���������λ��ֵΪ0
layout (location=1) in vec3 color;     //��ɫ����������λ��ֵΪ1
out vec3 ourColor;
void main(){
	gl_Position=vec4(position,1.0f);
	ourColor=color;
}