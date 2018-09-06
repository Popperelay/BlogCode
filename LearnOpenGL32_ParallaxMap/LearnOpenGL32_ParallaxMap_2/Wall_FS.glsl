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

//陡峭视差贴图
vec2 ParallaxMapping(vec2 vTexCoords, vec3 vViewDir)
{
	const float MinLayers = 8;
	const float MaxLayers = 32;
	float NumLayers = mix(MaxLayers, MinLayers, abs(dot(vec3(0, 0, 1), vViewDir)));
	//const float NumLayers = 10;
	float LayerDepth = 1.0 / NumLayers;
	float CurrentLayerDepth = 0.0;
	vec2 DeltaTexCoords = vViewDir.xy * u_DepthScale / NumLayers;
	vec2 CurrentTexCoords = vTexCoords;
	float CurrentDepthFromDepthMap = texture(u_DepthMap, CurrentTexCoords).r;
	
	while(CurrentLayerDepth < CurrentDepthFromDepthMap)
	{
		CurrentTexCoords -= DeltaTexCoords;
		CurrentDepthFromDepthMap = texture(u_DepthMap, CurrentTexCoords).r;
		CurrentLayerDepth += LayerDepth;
	}

	return CurrentTexCoords;
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
	Normal = texture(u_WallNormalMap, TexCoords).rgb;	//切线空间下的法线向量
	Normal = normalize(Normal * 2.0 - 1.0);		//把法线纹理中[0,1]范围内的法线，变换到[-1,1]之间
	
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