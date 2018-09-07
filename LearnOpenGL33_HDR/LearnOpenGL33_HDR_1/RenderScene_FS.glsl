#version 430 core
out vec4 Color_;

in vec3 v2f_FragWorldPos;
in vec3 v2f_Normal;
in vec2 v2f_TexCoords;

struct SLight
{
	vec3 Position;
	vec3 Color;
};

uniform SLight u_Lights[4];
uniform vec3 u_ViewPos;
uniform sampler2D u_DiffuseTexture;

void main()
{
	vec3 ObjectColor = texture(u_DiffuseTexture, v2f_TexCoords).rgb;
	vec3 Normal = normalize(v2f_Normal);
	//Ambient
	vec3 Ambient = 0.0 * ObjectColor;
	vec3 Lighting = vec3(0.0);
	float Diff;
	for(int i = 0; i < 4; ++i)
	{
		//Diffuse
		vec3 LightDir = normalize(u_Lights[i].Position - v2f_FragWorldPos);
		Diff = max(dot(LightDir, Normal), 0.0);
		vec3 Diffuse = u_Lights[i].Color * Diff * ObjectColor;
		//attenuation
		float Distance = length(v2f_FragWorldPos - u_Lights[i].Position);
		//float Distance = 1;
		Lighting += Diffuse * 1.0 / (Distance * Distance);
	}
	Color_ = vec4(Ambient + Lighting, 1.0);
	//Color_ = vec4(v2f_Normal, 1.0);
	//Color_ = vec4(vec3(Diff), 1.0);
}