#version 430 core

in vec4 g2f_WorldPos;
out vec4 Color_;

uniform samplerCube u_EnvironmentCubeMap;
uniform float u_Roughness;

const float PI = 3.14159265359;

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint uBits) 
{
     uBits = (uBits << 16u) | (uBits >> 16u);
     uBits = ((uBits & 0x55555555u) << 1u) | ((uBits & 0xAAAAAAAAu) >> 1u);
     uBits = ((uBits & 0x33333333u) << 2u) | ((uBits & 0xCCCCCCCCu) >> 2u);
     uBits = ((uBits & 0x0F0F0F0Fu) << 4u) | ((uBits & 0xF0F0F0F0u) >> 4u);
     uBits = ((uBits & 0x00FF00FFu) << 8u) | ((uBits & 0xFF00FF00u) >> 8u);
     return float(uBits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint vIndex, uint vN)
{
	return vec2(float(vIndex)/float(vN), RadicalInverse_VdC(vIndex));
}

vec3 ImportanceSampleGGX(vec2 vHammersleyVec, vec3 vNormal, float vRoughness)
{
	float a = vRoughness * vRoughness;
	//Theta和Phi是半程向量的球面坐标
	float Phi = 2.0 * PI * vHammersleyVec.x;
	float CosTheta = sqrt((1.0 - vHammersleyVec.y) / (1.0 + (a * a - 1.0) * vHammersleyVec.y));	//可以由上面arctan的公式推出来
	float SinTheta = sqrt(1.0 - CosTheta * CosTheta);
	//H是半程向量在其切线空间里的坐标
	vec3 H;
	H.x = cos(Phi) * SinTheta;
	H.y = sin(Phi) * SinTheta;
	H.z = CosTheta;
	//从切线空间转换到世界空间
	vec3 Up        = abs(vNormal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 Tangent   = normalize(cross(Up, vNormal));
	vec3 Bitangent = cross(vNormal, Tangent);
	vec3 SampleVec = Tangent * H.x + Bitangent * H.y + vNormal * H.z;

	return normalize(SampleVec);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

void main()
{
	vec3 Normal = normalize(vec3(g2f_WorldPos));
	//假设V=R=Normal
	vec3 R = Normal;
	vec3 V = R;
	
	const int SAMPLE_COUNT = 1024;
	vec3 PrefilteredColor = vec3(0);
	float TotalWeight = 0.0;
	
	for(int i = 0; i < SAMPLE_COUNT; ++i)
	{
		vec2 HammersleyVec = Hammersley(i, SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(HammersleyVec, Normal, u_Roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);

		float NdotL = max(dot(Normal, L), 0.0);
		if(NdotL > 0.0)
		{
			//根据粗糙度/pdf来从环境立方体贴图的mip层级中采样
			float D   = DistributionGGX(Normal, H, u_Roughness);
			float NdotH = max(dot(Normal, H), 0.0);
			float HdotV = max(dot(H, V), 0.0);
			float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

			float Resolution = 512.0; //环境立方体贴图每个面的分辨率
			float saTexel  = 4.0 * PI / (6.0 * Resolution * Resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float EnvMapMipLevel = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

			PrefilteredColor += textureLod(u_EnvironmentCubeMap, L, EnvMapMipLevel).rgb * NdotL;
            TotalWeight      += NdotL;
		}
	}
	PrefilteredColor = PrefilteredColor / TotalWeight;
	PrefilteredColor = PrefilteredColor;

	Color_ = vec4(PrefilteredColor, 1.0);
}