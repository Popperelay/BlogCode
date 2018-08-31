#version 430 core

in vec2 v2f_TexCoord;
out vec2 Color_;

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

float GeometrySchlickGGX(float vNdotV, float vRoughness)
{
    //注意对于IBL来说，k的计算公式是不一样的
    float a = vRoughness;
    float k = (a * a) / 2.0;

    float nom   = vNdotV;
    float denom = vNdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 vN, vec3 vV, vec3 vL, float vRoughness)
{
    float NdotV = max(dot(vN, vV), 0.0);
    float NdotL = max(dot(vN, vL), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, vRoughness);
    float ggx1 = GeometrySchlickGGX(NdotL, vRoughness);

    return ggx1 * ggx2;
}

vec2 IntegrateBRDF(float vNdotV, float vRoughness)
{
    vec3 V;
    V.x = sqrt(1.0 - vNdotV*vNdotV);
    V.y = 0.0;
    V.z = vNdotV;

    float A = 0.0;
    float B = 0.0; 

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, vRoughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, vRoughness);
            float G_Vis = (G * VdotH) / (NdotH * vNdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main()
{
	vec2 integratedBRDF = IntegrateBRDF(v2f_TexCoord.x, v2f_TexCoord.y);
	Color_ = integratedBRDF;
}