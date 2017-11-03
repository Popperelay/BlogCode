//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //顶点位置属性的位置值为0
layout (location=1) in vec3 normal;    //顶点法线属性的位置值为1
out	VS_OUT{
	vec3 Normal;
} vs_out;
//模型矩阵
uniform mat4 model;
//观察矩阵
uniform mat4 view;
//投影矩阵
uniform mat4 projection;
void main(){
	gl_Position = projection * view * model * vec4(position,1.0f);
	vs_out.Normal = normalize(vec3(projection * view * vec4(mat3(transpose(inverse(model))) * normal, 1.0f)));
}