//������ɫ��
#version 330 core
layout (location=0) in vec3 position;  //����λ�����Ե�λ��ֵΪ0
layout (location=1) in vec3 normal;    //���㷨�����Ե�λ��ֵΪ1
layout (location=2) in vec2 texCoords; //���������������Ե�λ��ֵΪ2
out vec2 VS_TexCoords;
//ģ�;���
uniform mat4 model;
//�۲����
uniform mat4 view;
//ͶӰ����
uniform mat4 projection;
void main(){
	gl_Position = projection * view * model * vec4(position,1.0f);
	VS_TexCoords = texCoords;
}