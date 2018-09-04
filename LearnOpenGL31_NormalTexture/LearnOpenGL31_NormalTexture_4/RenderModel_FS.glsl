#version 430 core

in vec2 v2f_TexCoord;
in vec3 v2f_FragPosInTangentSpace;
in vec3 v2f_ViewPosInTangentSpace;
in vec3 v2f_LightPosInTangentSpace;
in vec3 v2f_Normal;
//in flat int v2f_IsApplyNormalMap;
out vec4 Color_;

uniform sampler2D u_DiffuseTexture0;
uniform sampler2D u_SpecularTexture0;
uniform sampler2D u_NormalMap0;
uniform vec3 u_LightColor = vec3(1.0);
uniform bool u_IsApplyNormalMap;

void main()
{
	vec3 ObjectDiffuseColor = texture(u_DiffuseTexture0, v2f_TexCoord).rgb;
	vec3 ObjectSpecularColor = texture(u_SpecularTexture0, v2f_TexCoord).rgb;
	vec3 Normal;
	if(u_IsApplyNormalMap)
	{
		Normal = texture(u_NormalMap0, v2f_TexCoord).rgb;	//切线空间下的法线向量
		Normal = normalize(Normal * 2.0 - 1.0);		//把法线纹理中[0,1]范围内的法线，变换到[-1,1]之间
	}
	else
	{
		Normal = v2f_Normal;
	}
	
	//Ambient
	vec3 Ambient = ObjectDiffuseColor * 0.3;
	//Diffuse
	vec3 LightDir = normalize(v2f_LightPosInTangentSpace - v2f_FragPosInTangentSpace);
	float Diff = max(dot(LightDir, Normal), 0.0f);
	vec3 Diffuse = Diff * ObjectDiffuseColor;
	//Specular
	vec3 ViewDir = normalize(v2f_ViewPosInTangentSpace - v2f_FragPosInTangentSpace);
	vec3 HalfWayDir = normalize(ViewDir + LightDir);
	float Spec = pow(max(dot(Normal, HalfWayDir), 0.0), 32.0f);
	vec3 Specular = Spec * u_LightColor * ObjectSpecularColor;

	Color_ = vec4(Ambient + Diffuse + Specular, 1.0f);
}