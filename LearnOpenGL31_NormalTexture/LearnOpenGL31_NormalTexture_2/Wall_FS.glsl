#version 430 core

in vec2 v2f_TexCoord;
in vec3 v2f_FragWorldPos;
in mat3 v2f_TBN;
out vec4 Color_;

uniform sampler2D u_WallTexture;
uniform sampler2D u_WallNormalMap;
uniform vec3 u_ViewPos;
uniform vec3 u_LightPos = vec3(0.5, 0.5, 0.3);
uniform vec3 u_LightColor = vec3(1.0);

void main()
{
	vec3 ObjectColor = texture(u_WallTexture, v2f_TexCoord).rgb;
	vec3 Normal;
	Normal = texture(u_WallNormalMap, v2f_TexCoord).rgb;
	Normal = normalize(Normal * 2.0 - 1.0);		//�ѷ���������[0,1]��Χ�ڵķ��ߣ��任��[-1,1]֮��
	Normal = normalize(v2f_TBN * Normal);		//�����ߴ����߿ռ�ת��������ռ�
	

	//Ambient
	vec3 Ambient = ObjectColor * 0.1;
	//Diffuse
	vec3 LightDir = normalize(u_LightPos - v2f_FragWorldPos);
	float Diff = max(dot(LightDir, Normal), 0.0f);
	vec3 Diffuse = Diff * ObjectColor;
	//Specular
	vec3 ViewDir = normalize(u_ViewPos - v2f_FragWorldPos);
	vec3 HalfWayDir = normalize(ViewDir + LightDir);
	float Spec = pow(max(dot(Normal, HalfWayDir), 0.0), 32.0f);
	vec3 Specular = Spec * u_LightColor * ObjectColor;

	Color_ = vec4(Ambient + Diffuse + Specular, 1.0f);
}