#version 430 core

in vec2 v2f_TexCoords;
in vec3 v2f_WorldPos;
in vec3 v2f_Normal;
out vec4 Color_;

//uniform vec3 u_ObjectColor;
//uniform float u_Metalness;
//uniform float u_Roughness;
//uniform float u_AO;

uniform sampler2D u_NormalMap;
uniform sampler2D u_ObjectColorMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_RoughtnessMap;
uniform sampler2D u_AOMap;


uniform vec3  u_LightPos[4];
uniform vec3  u_LightColors[4];
uniform vec3  u_ViewPos;

const float PI = 3.14159265359;

//��̬�ֲ����������ڹ���΢ƽ��ȡ��
float DistributionGGX(vec3 vN, vec3 vH, float vRoughness)
{
	float a = vRoughness * vRoughness;
	float a2 = a * a;
	float NdotH = max(dot(vN, vH), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom = a2;  //����
	float denom = NdotH2 * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return nom / max(denom, 0.001);		//��ֹvRoughness=0����NdotH=1ʱ���ַ�ĸ��0�����
}

float GeometrySchlickGGX(float vNdotV, float vRoughness)
{
	float r = vRoughness + 1.0;
	float k = r * r / 8.0;
	float nom = vNdotV;
	float denom = vNdotV * (1.0 - k) + k;
	return nom / denom;
}

//���κ�������������΢ƽ��ļ����ڱκͼ�����Ӱ
float GeometrySmith(vec3 vN, vec3 vV, vec3 vL, float vRoughness)
{
	float NdotV = max(dot(vN, vV), 0.0);
	float NdotL = max(dot(vN, vL), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, vRoughness);
	float ggx2 = GeometrySchlickGGX(NdotL, vRoughness);
	return ggx1 * ggx2;
}

//���������̣���������������߱���
vec3 FresnelSchlick(float vCosTheta, vec3 vF0)
{
	return vF0 + (1.0 - vF0) * pow(1.0 - vCosTheta, 5.0);
}

vec3 getNormalFromMap()
{
	vec3 TangentNormal = texture(u_NormalMap, v2f_TexCoords).xyz * 2.0 - 1.0;
	
	vec3 Q1 = dFdx(v2f_WorldPos);
	vec3 Q2 = dFdy(v2f_WorldPos);
	vec2 st1 = dFdx(v2f_TexCoords);
	vec2 st2 = dFdy(v2f_TexCoords);

	vec3 N = normalize(v2f_Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * TangentNormal);
}

void main()
{
	vec3  ObjectColor = pow(texture(u_ObjectColorMap, v2f_TexCoords).rgb, vec3(2.2));
	float Metalness = texture(u_MetalnessMap, v2f_TexCoords).r;
	float Roughness = texture(u_RoughtnessMap, v2f_TexCoords).r;
	float AO = texture(u_AOMap, v2f_TexCoords).r;
		
	vec3 N = getNormalFromMap();
	vec3 V = normalize(u_ViewPos - v2f_WorldPos);

	//�������������
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, ObjectColor, Metalness);
	
	//�����ʷ���
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; ++i)
	{
		//�����Դ����������Li
		vec3 L = normalize(u_LightPos[i] - v2f_WorldPos);
		vec3 H = normalize(V + L);
		float Distance = length(u_LightPos[i] - v2f_WorldPos);
		float Attenuation = 1.0 / (Distance * Distance);
		vec3 Radiance = u_LightColors[i] * Attenuation;

		//����Cook-Torrance BRDF
		float D = DistributionGGX(N, H, Roughness);
		vec3  F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
		float G = GeometrySmith(N, V, L, Roughness);
		vec3 nom = D * F * G;
		float denom = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 Specular = nom / max(denom, 0.001);	//��ֹNdotV=0.0����NdotL=0.0ʱ����ĸ��0
		
		//����������;��淴��ı���
		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd *= (1.0 - Metalness);	//��������û��������
		
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kd * ObjectColor / PI + Specular) * Radiance * NdotL;
	}
	
	vec3 Ambient = vec3(0.03) * ObjectColor * AO;
	vec3 Color = Ambient + Lo;

	//HDRɫ��ӳ��
	Color = Color / (Color + vec3(1.0));
	//GammaУ��
	Color = pow(Color, vec3(1.0 / 2.2));

	Color_ = vec4(Color, 1.0);
}