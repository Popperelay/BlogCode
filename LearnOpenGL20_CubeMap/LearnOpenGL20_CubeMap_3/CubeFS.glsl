#version 330 core
in vec3 Normal;
in vec3 PositionInWorld;
out vec4 color;
uniform vec3 cameraPos;
uniform samplerCube cubemap;
void main(){
	//����
	/*
	vec3 I = normalize(PositionInWorld - cameraPos);
	vec3 R = reflect(I,normalize(Normal));
	color = texture(cubemap,R);*/
	//����
	float ratio = 1.00/1.52;
	vec3 I = normalize(PositionInWorld - cameraPos);
	vec3 R = refract(I,normalize(Normal),ratio);
	color = texture(cubemap,R);
}