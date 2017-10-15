#version 330 core
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
//材质结构体
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
//光源
struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Material material; //材质变量
uniform Light light;       //光源变量
uniform vec3 viewPos;      //观察位置
void main(){
	//计算最终的环境光
	vec3 ambientColor = light.ambient*material.ambient;  //环境光
	//计算最终的漫反射光
	vec3 normal = normalize(Normal);  //单位法向量
	vec3 lightDir = normalize(light.position - positionInWorld); //光线入射方向的单位向量
	float diffuseFactor = max(dot(normal, lightDir),0.0); //漫反射系数（两个向量夹角的余弦）
	vec3 diffuseColor = diffuseFactor * light.diffuse * material.diffuse;   //漫反射光
	//计算最终的镜面反射光
	vec3 reflectDir = normalize(reflect(-lightDir, normal));   //反射光方向
	vec3 viewDir = normalize(viewPos - positionInWorld);  //观察方向
	float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess); //镜面反射系数
	vec3 specularColor = specularFactor * light.specular * material.specular; //镜面反射光
	color = vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}