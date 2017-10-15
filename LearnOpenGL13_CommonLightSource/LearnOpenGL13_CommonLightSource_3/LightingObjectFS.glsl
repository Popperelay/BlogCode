#version 330 core
//材质结构体
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
//平行光结构体
struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//点光源结构体
struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;  //常数项
	float linear;    //一次项
	float quadric;   //二次项
};
//聚光灯结构体
struct SpotLight{
	vec3 position;   //聚光灯位置
	vec3 lightAxis;  //灯轴方向
	float inCutOff;  //内张角
	float outCutOff; //外张角  
	float constant; //常数项
	float linear;   //一次项
	float quadric;  //二次项
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
in vec3 Normal;
in vec3 positionInWorld;
in vec2 TexCoords;
out vec4 color;
uniform Material material;                        //材质变量
uniform DirectionalLight dirLight;                //定向光源变量
#define NUM_POINT_LIGHTS 4                        //点光源个数
uniform PointLight pointLights[NUM_POINT_LIGHTS];  //点光源变量
uniform SpotLight spotLight;                      //聚光灯变量
uniform vec3 viewPos;                             //观察位置
vec3 CalcDirLight(DirectionalLight dirLight);
vec3 CalcPointLight(PointLight pointLight);
vec3 CalcSpotLight(SpotLight spotLight);
void main(){
	vec3 result = CalcDirLight(dirLight);
	for(int i=0; i<NUM_POINT_LIGHTS; i++){
		result += CalcPointLight(pointLights[i]);
	}
	result += CalcSpotLight(spotLight);
	color=vec4(result,1.0f);
}
//计算定向光
vec3 CalcDirLight(DirectionalLight dirLight){
	//环境光
	vec3 ambientColor = dirLight.ambient * vec3(texture(material.diffuse,TexCoords));
	//漫反射光
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diffuseFactor = max(dot(normal,lightDir),0.0f);
	vec3 diffuseColor = diffuseFactor * dirLight.diffuse * vec3(texture(material.diffuse,TexCoords));
	//镜面反射光
	vec3 reflectDir = normalize(reflect(-lightDir,normal));
	vec3 viewDir = normalize(viewPos-positionInWorld);
	float specularFactor = pow(max(dot(reflectDir,viewDir),0.0f),material.shininess);
	vec3 specularColor = specularFactor * dirLight.specular * vec3(texture(material.specular,TexCoords));
	return ambientColor + diffuseColor + specularColor;
}
//计算点光源
vec3 CalcPointLight(PointLight pointLight){
	//计算最终的环境光
	vec3 ambientColor = pointLight.ambient * vec3(texture(material.diffuse,TexCoords));  //环境光
	//计算最终的漫反射光
	vec3 normal = normalize(Normal);  //单位法向量
	vec3 lightDir = normalize(pointLight.position - positionInWorld); //光线入射方向的单位向量
	float diffuseFactor = max(dot(normal,lightDir),0.0); //漫反射系数（两个向量夹角的余弦）
	vec3 diffuseColor = diffuseFactor * pointLight.diffuse * vec3(texture(material.diffuse,TexCoords));   //漫反射光
	//计算最终的镜面反射光
	vec3 reflectDir = normalize(reflect(-lightDir,normal));   //反射光方向
	vec3 viewDir = normalize(viewPos - positionInWorld);  //观察方向
	float specularFactor = pow(max(dot(reflectDir,viewDir),0.0),material.shininess); //镜面反射系数
	vec3 specularColor = specularFactor * pointLight.specular * vec3(texture(material.specular,TexCoords)); //镜面反射光
	
	//计算点光源的衰减系数
	float distance=length(pointLight.position-positionInWorld);
	float attenuation=1.0f/(pointLight.constant+pointLight.linear*distance+pointLight.quadric*distance*distance);
	//点光源的每种成分光都执行衰减
	ambientColor*=attenuation;
	diffuseColor*=attenuation;
	specularColor*=attenuation;
	
	return ambientColor+diffuseColor+specularColor;
}
//计算聚光
vec3 CalcSpotLight(SpotLight spotLight){
	//片元偏轴角（余弦值）
	float theta = dot(normalize(positionInWorld - spotLight.position),normalize(spotLight.lightAxis));
	//片元在聚光灯下的光强
	float intensity = clamp((theta - spotLight.outCutOff)/(spotLight.inCutOff - spotLight.outCutOff),0.0,1.0);
	//if(theta > spotLight.outCutOff){
		//环境光
		vec3 ambientColor = spotLight.ambient * vec3(texture(material.diffuse,TexCoords));
		//漫反射光
		vec3 normal = normalize(Normal);
		vec3 lightDir = normalize(spotLight.position - positionInWorld);
		float diffuseFactor = max(dot(normal,lightDir),0.0);
		vec3 diffuseColor = diffuseFactor * spotLight.diffuse * vec3(texture(material.diffuse,TexCoords));
		//镜面反射光
		vec3 reflectDir = normalize(reflect(-lightDir,normal));
		vec3 viewDir = normalize(viewPos - positionInWorld);
		float specularFactor = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
		vec3 specularColor = specularFactor * spotLight.specular * vec3(texture(material.specular,TexCoords));
		//聚光的衰减系数
		float distance = length(spotLight.position - positionInWorld);
		float attenuation = 1.0f/(spotLight.constant + spotLight.linear*distance + spotLight.quadric*distance*distance);
		//聚光的每种成分光都执行衰减和亮度插值
		ambientColor *= attenuation * intensity;
		diffuseColor *= attenuation * intensity;
		specularColor *= attenuation * intensity;
		return ambientColor + diffuseColor + specularColor;
	//}
	//else{  //片元在聚光范围外时使用环境光，使其不至于完全黑暗
		//return spotLight.ambient * vec3(texture(material.diffuse,TexCoords));
		//return vec3(0.0f,0.0f,0.0f);
	//}	
}