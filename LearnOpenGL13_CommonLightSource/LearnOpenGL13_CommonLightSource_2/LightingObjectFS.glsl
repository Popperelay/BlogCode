#version 330 core
//材质结构体
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
//聚光灯结构体
struct Light{
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
uniform Material material; //材质变量
uniform Light light;       //光源变量
uniform vec3 viewPos;      //观察位置
void main(){
	//计算聚光灯下片元的偏轴角（余弦值）
	float theta=dot(normalize(positionInWorld-light.position),normalize(light.lightAxis));
	//计算片元在聚光灯下的光强
	float intensity=clamp((theta-light.outCutOff)/(light.inCutOff-light.outCutOff),0.0,1.0);
	//if(theta > light.outCutOff){
		//计算最终的环境光
		vec3 ambientColor=light.ambient * vec3(texture(material.diffuse,TexCoords));  //环境光
		//计算最终的漫反射光
		vec3 normal=normalize(Normal);  //单位法向量
		vec3 lightDir=normalize(light.position-positionInWorld); //光线入射方向的单位向量
		float diffuseFactor=max(dot(normal,lightDir),0.0); //漫反射系数（两个向量夹角的余弦）
		vec3 diffuseColor=diffuseFactor * light.diffuse * vec3(texture(material.diffuse,TexCoords));   //漫反射光
		//计算最终的镜面反射光
		vec3 reflectDir=normalize(reflect(-lightDir,normal));   //反射光方向
		vec3 viewDir=normalize(viewPos-positionInWorld);  //观察方向
		float specularFactor=pow(max(dot(reflectDir,viewDir),0.0),material.shininess); //镜面反射系数
		vec3 specularColor=specularFactor * light.specular * vec3(texture(material.specular,TexCoords)); //镜面反射光
	
		//计算聚光的衰减系数
		float distance=length(light.position-positionInWorld);
		float attenuation=1.0f/(light.constant+light.linear*distance+light.quadric*distance*distance);
		//聚光的每种成分光都执行衰减和亮度插值
		ambientColor*=attenuation*intensity;
		diffuseColor*=attenuation*intensity;
		specularColor*=attenuation*intensity;
		color=vec4(ambientColor+diffuseColor+specularColor,1.0f);
	//}
	//else{  //片元在聚光范围外时使用环境光，使其不至于完全黑暗
		//color=vec4(light.ambient*vec3(texture(material.diffuse,TexCoords)),1.0f);
		//color=vec4(0.0f,0.0f,0.0f,1.0f);
	//}
}