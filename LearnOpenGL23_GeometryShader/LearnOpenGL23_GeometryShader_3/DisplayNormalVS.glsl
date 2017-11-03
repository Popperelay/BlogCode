//������ɫ��
#version 330 core
layout (location=0) in vec3 position;  //����λ�����Ե�λ��ֵΪ0
layout (location=1) in vec3 normal;    //���㷨�����Ե�λ��ֵΪ1
out	VS_OUT{
	vec3 Normal;
} vs_out;
//ģ�;���
uniform mat4 model;
//�۲����
uniform mat4 view;
//ͶӰ����
uniform mat4 projection;
void main(){
	gl_Position = projection * view * model * vec4(position,1.0f);
	vs_out.Normal = normalize(vec3(projection * view * vec4(mat3(transpose(inverse(model))) * normal, 1.0f)));
}