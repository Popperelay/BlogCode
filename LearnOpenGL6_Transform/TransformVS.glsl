//������ɫ��
#version 330 core
layout (location=0) in vec3 position;  //����λ�ñ���������λ��ֵΪ0
layout (location=1) in vec3 color;     //������ɫ����������λ��ֵΪ1
layout (location=2) in vec2 texCoord;  //�����������������λ��ֵΪ2
out vec3 ourColor;
out vec2 TexCoord;
//����任����
uniform mat4 transform;
void main(){
	gl_Position = transform * vec4(position,1.0f);
	ourColor = color;
	TexCoord = vec2(texCoord.x, 1 - texCoord.y);
}