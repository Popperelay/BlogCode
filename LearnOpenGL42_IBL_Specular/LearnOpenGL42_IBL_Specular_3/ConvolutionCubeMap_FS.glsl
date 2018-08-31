#version 430 core

in vec4 g2f_WorldPos;
out vec4 Color_;

uniform samplerCube u_EnvironmentCubeMap;

const float PI = 3.14159265359;

void main()
{
	vec3 Normal = normalize(vec3(g2f_WorldPos));
	vec3 Up = vec3(0, 1, 0);
	vec3 Right = cross(Up, Normal);
	Up = cross(Normal, Right);
	
	vec3 Irradiance = vec3(0.0);
	float SampleDelta = 0.025;
	int SampleCount = 0;
	float DoublePI = 2.0 * PI;
	float HalfPI = 0.5 * PI;
	for(float Phi = 0.0; Phi < DoublePI; Phi += SampleDelta)
	{
		for(float Theta = 0.0; Theta < HalfPI; Theta += SampleDelta)
		{
			vec3 SampleVectorInTangentSpace = vec3(sin(Theta) * cos(Phi), sin(Theta) * sin(Phi), cos(Theta));
			vec3 SampleVecrorInWorldSpace = SampleVectorInTangentSpace.x * Right + SampleVectorInTangentSpace.y * Up + SampleVectorInTangentSpace.z * Normal;
			Irradiance += texture(u_EnvironmentCubeMap, SampleVecrorInWorldSpace).rgb * cos(Theta) * sin(Theta);
			++SampleCount;
		}
	}
	Irradiance = PI * Irradiance * (1.0 / SampleCount);	//不知道为什么是乘以PI，而不是除以PI

	Color_ = vec4(Irradiance, 1.0);
}