#version 430 core

in vec2 v2f_TexCoord;
in vec3 v2f_FragPosInTangentSpace;
in vec3 v2f_ViewPosInTangentSpace;
in vec3 v2f_LightPosInTangentSpace;
out vec4 Color_;

uniform sampler2D u_WallTexture;
uniform sampler2D u_WallNormalMap;
uniform vec3 u_LightColor = vec3(1.0);

void main()
{
	vec3 ObjectColor = texture(u_WallTexture, v2f_TexCoord).rgb;
	vec3 Normal;
	Normal = texture(u_WallNormalMap, v2f_TexCoord).rgb;	//切线空间下的法线向量
	Normal = normalize(Normal * 2.0 - 1.0);		//把法线纹理中[0,1]范围内的法线，变换到[-1,1]之间
	
	//Ambient
	vec3 Ambient = ObjectColor * 0.1;
	//Diffuse
	vec3 LightDir = normalize(v2f_LightPosInTangentSpace - v2f_FragPosInTangentSpace);
	float Diff = max(dot(LightDir, Normal), 0.0f);
	vec3 Diffuse = Diff * ObjectColor;
	//Specular
	vec3 ViewDir = normalize(v2f_ViewPosInTangentSpace - v2f_FragPosInTangentSpace);
	vec3 HalfWayDir = normalize(ViewDir + LightDir);
	float Spec = pow(max(dot(Normal, HalfWayDir), 0.0), 32.0f);
	vec3 Specular = Spec * u_LightColor * ObjectColor;

	Color_ = vec4(Ambient + Diffuse + Specular, 1.0f);
}