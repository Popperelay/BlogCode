#version 330 core
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
uniform vec3 lightColor;  //光源颜色
uniform vec3 objectColor; //物体本身颜色
uniform vec3 lightPos;    //光源位置
uniform vec3 viewPos;     //观察位置
void main(){
	//计算环境光
	float ambientStrength=0.3f;  //环境因子	
	vec3 ambientColor=ambientStrength*lightColor;  //环境光
	//计算漫反射光
	vec3 normal=normalize(Normal);  //单位法向量
	vec3 lightDir=normalize(lightPos-positionInWorld); //光线入射方向的单位向量
	float diffuseFactor=max(dot(normal,lightDir),0.0); //漫反射光系数（两个向量夹角的余弦）
	vec3 diffuseColor=diffuseFactor*lightColor;  //漫反射光
	//计算镜面反射光
	vec3 reflectDir=normalize(reflect(-lightDir,normal));   //反射光方向
	vec3 viewDir=normalize(viewPos-positionInWorld);  //观察方向
	float specularFactor=pow(max(dot(reflectDir,viewDir),0.0),32); //镜面反射系数
	float specularStrength=0.5f; //镜面反射强度
	vec3 specularColor=specularStrength*specularFactor*lightColor; //镜面反射光
	color=vec4((ambientColor+diffuseColor+specularColor)*objectColor,1.0f);
}