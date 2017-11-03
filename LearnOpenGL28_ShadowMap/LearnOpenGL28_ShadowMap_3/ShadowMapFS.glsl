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
	if(Temp.z > 1.0)
		Temp.z = 0.0;
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

	//PCF阴影测试
	float Offset = max(0.002, 0.0025 * (1.0 - dot(Normal, LightDir)));
	float Shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_DepthMapSampler2, 0);
	
	if(LightClipSpacePos.z != 0.0)
	{
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				if(LightClipSpacePos.z >= texture(u_DepthMapSampler2, LightClipSpacePos.xy + vec2(x, y) * texelSize).r + Offset)
					Shadow += 1.0;
			}
		}
	}
	Shadow /= 9.0;
	
	//Diffuse Lighting
	float DiffuseFactor = max(dot(Normal, LightDir), 0.0);
	vec3 DiffuseColor = DiffuseFactor * u_LightColor;

	//Specular Lighting
	vec3 ViewDir = normalize(u_ViewPos - VS_WorldPos);
	vec3 HalfDir = normalize(LightDir + ViewDir);
	float SpecularFactor = pow(max(dot(HalfDir, Normal), 0.0f), 32);
	vec3 SpecularColor = SpecularFactor * u_LightColor;

	Color = vec4((AmbientColor + (1.0 - Shadow) * (DiffuseColor + SpecularColor)) * ObjectColor, 1.0);
}