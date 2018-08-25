#version 430 core

in vec3 v2f_WorldPos;
out vec4 Color_;

//uniform samplerCube u_Cubemap;
uniform sampler2D u_SphereMap;		//������HDR��ͼ

const vec2 InvAtan = vec2(0.1591, 0.3183);  //0.1591 = 1 / (2*PI); 0.3183 = 1 / (PI)

vec2 sampleSphereMap(vec3 v)
{
	vec2 UV = vec2(atan(v.z, v.x), asin(v.y));
	UV *= InvAtan;
	UV += 0.5;
	return UV;
}

void main()
{
	vec2 UV = sampleSphereMap(normalize(v2f_WorldPos));	//����Ҫ��һ��
	Color_ = vec4(v2f_WorldPos.x, 0.0, 0.0, 1.0);
}