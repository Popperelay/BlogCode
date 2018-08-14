#version 430 core

in vec2 v2f_TexCoord;
in vec3 v2f_Normal;
in vec3 v2f_WorldPos;

out vec4 Color_;

uniform sampler2D u_DiffuseTexture;
uniform samplerCube u_DepthCubeMap;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_FarPlane;
uniform vec3 u_LightColor = vec3(0.3);

float calculateShadow(vec3 vWorldPos)
{
	vec3 Light2Frag = vWorldPos - u_LightPos;
	float CurrentDepth = length(Light2Frag);
	float Bias = 0.05;
	float Shadow = 0.0;
	int Samples = 20;
	vec3 SampleOffsetDirections[20] = vec3[]
	(
		vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
		vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
		vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
		vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
		vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	//float DiskRadius = 0.05;
float ViewDistance = length(u_ViewPos - v2f_WorldPos);
float DiskRadius = (1.0 + (ViewDistance / u_FarPlane)) / 25.0;
for(int i = 0; i < Samples; ++i)
{
	float ClosestDepth = texture(u_DepthCubeMap, Light2Frag + SampleOffsetDirections[i] * DiskRadius).r;
	ClosestDepth *= u_FarPlane;
	if(CurrentDepth - Bias > ClosestDepth)
		Shadow += 1.0;
}
Shadow /= float(Samples);
	return Shadow;
}

void main()
{
	vec3 ObjectColor = texture(u_DiffuseTexture, v2f_TexCoord).rgb;
	vec3 Normal = normalize(v2f_Normal);
	//Ambient
	vec3 AmbientColor = 0.3 * ObjectColor;
	//Diffuse
	vec3 LightDirection = normalize(u_LightPos - v2f_WorldPos);
	float DiffuseFactor = max(dot(LightDirection, Normal), 0.0);
	vec3 DiffuseColor = DiffuseFactor * u_LightColor;
	//Specular
	vec3 ViewDirection = normalize(u_ViewPos - v2f_WorldPos);
	vec3 HalfDirection = normalize(LightDirection + ViewDirection);
	float SpecularFactor = pow(max(dot(Normal, HalfDirection), 0.0), 64.0);
	vec3 SpecularColor = SpecularFactor * u_LightColor;
	//Calculate Shadow
	float Shadow = calculateShadow(v2f_WorldPos);
	vec3 Result = AmbientColor + (1.0 - Shadow) * (DiffuseColor + SpecularColor) * ObjectColor;
	Color_ = vec4(Result, 1.0);
}