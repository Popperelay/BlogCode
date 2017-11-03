#version 330 core

in vec2 VS_TexCoords;
in vec3 VS_Normal;
in vec3 VS_WorldPos;
in vec4 VS_LightSpacePos;

out vec4 Color;

uniform sampler2D u_DiffuseMapSampler1;
uniform sampler2D u_DepthMapSampler2;
uniform vec3 u_LightPos;
uniform vec3 u_LightDirection;
uniform vec3 u_ViewPos;
uniform vec3 u_LightColor;

vec3 getDepthInLightSpace(vec4 vLightSpacePos)
{
	vec3 Temp = (vLightSpacePos / vLightSpacePos.w).xyz;
	Temp = Temp * 0.5 + 0.5;
	//远平面外的深度值更改为0.0
	//if(Temp.z > 1.0)
	//	Temp.z = 0.0;
	return Temp;
}

void main()
{
	vec3 ObjectColor = texture(u_DiffuseMapSampler1, VS_TexCoords).rgb;

	//Ambient Lighting
	float AmbientStrength = 0.3f;
	vec3  AmbientColor = AmbientStrength * ObjectColor;

	vec3 LightClipSpacePos = getDepthInLightSpace(VS_LightSpacePos);

	vec3 Normal = normalize(VS_Normal);
	vec3 LightDir = normalize(-u_LightDirection);

	//阴影测试
	//if(LightClipSpacePos.z >= texture(u_DepthMapSampler2, LightClipSpacePos.xy).r + 0.0009)
	//float Offset = max(0.0009, 0.0025 * (1.0 - dot(Normal, LightDir)));
	//if(LightClipSpacePos.z >= texture(u_DepthMapSampler2, LightClipSpacePos.xy).r + Offset)
	//if(LightClipSpacePos.z >= texture(u_DepthMapSampler2, LightClipSpacePos.xy).r + 0.01)
	if(LightClipSpacePos.z >= texture(u_DepthMapSampler2, LightClipSpacePos.xy).r)
	{
		Color = vec4(AmbientColor * ObjectColor, 1.0);
		return;
	}

	//Diffuse Lighting
	float DiffuseFactor = max(dot(Normal, LightDir), 0.0);
	vec3 DiffuseColor = DiffuseFactor * u_LightColor;

	//Specular Lighting
	vec3 ViewDir = normalize(u_ViewPos - VS_WorldPos);
	vec3 HalfDir = normalize(LightDir + ViewDir);
	float SpecularFactor = pow(max(dot(HalfDir, Normal), 0.0f), 32);
	vec3 SpecularColor = SpecularFactor * u_LightColor;

	Color = vec4((AmbientColor + DiffuseColor + SpecularColor) * ObjectColor, 1.0);
}