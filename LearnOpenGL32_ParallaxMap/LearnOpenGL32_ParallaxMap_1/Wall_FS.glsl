#version 430 core

in vec2 v2f_TexCoord;
in vec3 v2f_FragPosInTangentSpace;
in vec3 v2f_ViewPosInTangentSpace;
in vec3 v2f_LightPosInTangentSpace;
out vec4 Color_;

uniform sampler2D u_WallTexture;
uniform sampler2D u_WallNormalMap;
uniform sampler2D u_DepthMap;
uniform vec3 u_LightColor = vec3(1.0);
uniform float u_DepthScale;
uniform bool u_IsApplyParallaxMap;

vec2 ParallaxMapping(vec2 vTexCoords, vec3 vViewDir)
{
	float Depth = texture(u_DepthMap, vTexCoords).r;
	vec3 P = vViewDir * (Depth * u_DepthScale);
	vec2 TexCoordsOffset = P.xy / vViewDir.z;
	return vTexCoords - TexCoordsOffset;
}

void main()
{
	vec3 ViewDir = normalize(v2f_ViewPosInTangentSpace - v2f_FragPosInTangentSpace);
	vec2 TexCoords;
	if(u_IsApplyParallaxMap)
	{
		TexCoords = ParallaxMapping(v2f_TexCoord, ViewDir);
		if(TexCoords.x > 1.0 || TexCoords.y > 1.0 || TexCoords.x < 0.0 || TexCoords.y < 0.0)
			discard;
	}
	else
		TexCoords = v2f_TexCoord;

	vec3 ObjectColor = texture(u_WallTexture, TexCoords).rgb;
	vec3 Normal;
	Normal = texture(u_WallNormalMap, TexCoords).rgb;	//���߿ռ��µķ�������
	Normal = normalize(Normal * 2.0 - 1.0);		//�ѷ���������[0,1]��Χ�ڵķ��ߣ��任��[-1,1]֮��
	
	//Ambient
	vec3 Ambient = ObjectColor * 0.1;
	//Diffuse
	vec3 LightDir = normalize(v2f_LightPosInTangentSpace - v2f_FragPosInTangentSpace);
	float Diff = max(dot(LightDir, Normal), 0.0f);
	vec3 Diffuse = Diff * ObjectColor;
	//Specular
	vec3 HalfWayDir = normalize(ViewDir + LightDir);
	float Spec = pow(max(dot(Normal, HalfWayDir), 0.0), 32.0f);
	vec3 Specular = Spec * u_LightColor * ObjectColor;

	Color_ = vec4(Ambient + Diffuse + Specular, 1.0f);
}