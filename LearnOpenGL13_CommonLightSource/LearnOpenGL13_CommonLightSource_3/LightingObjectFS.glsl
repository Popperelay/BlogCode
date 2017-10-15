#version 330 core
//���ʽṹ��
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
//ƽ�й�ṹ��
struct DirectionalLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//���Դ�ṹ��
struct PointLight{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;  //������
	float linear;    //һ����
	float quadric;   //������
};
//�۹�ƽṹ��
struct SpotLight{
	vec3 position;   //�۹��λ��
	vec3 lightAxis;  //���᷽��
	float inCutOff;  //���Ž�
	float outCutOff; //���Ž�  
	float constant; //������
	float linear;   //һ����
	float quadric;  //������
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
in vec3 Normal;
in vec3 positionInWorld;
in vec2 TexCoords;
out vec4 color;
uniform Material material;                        //���ʱ���
uniform DirectionalLight dirLight;                //�����Դ����
#define NUM_POINT_LIGHTS 4                        //���Դ����
uniform PointLight pointLights[NUM_POINT_LIGHTS];  //���Դ����
uniform SpotLight spotLight;                      //�۹�Ʊ���
uniform vec3 viewPos;                             //�۲�λ��
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
//���㶨���
vec3 CalcDirLight(DirectionalLight dirLight){
	//������
	vec3 ambientColor = dirLight.ambient * vec3(texture(material.diffuse,TexCoords));
	//�������
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diffuseFactor = max(dot(normal,lightDir),0.0f);
	vec3 diffuseColor = diffuseFactor * dirLight.diffuse * vec3(texture(material.diffuse,TexCoords));
	//���淴���
	vec3 reflectDir = normalize(reflect(-lightDir,normal));
	vec3 viewDir = normalize(viewPos-positionInWorld);
	float specularFactor = pow(max(dot(reflectDir,viewDir),0.0f),material.shininess);
	vec3 specularColor = specularFactor * dirLight.specular * vec3(texture(material.specular,TexCoords));
	return ambientColor + diffuseColor + specularColor;
}
//������Դ
vec3 CalcPointLight(PointLight pointLight){
	//�������յĻ�����
	vec3 ambientColor = pointLight.ambient * vec3(texture(material.diffuse,TexCoords));  //������
	//�������յ��������
	vec3 normal = normalize(Normal);  //��λ������
	vec3 lightDir = normalize(pointLight.position - positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor = max(dot(normal,lightDir),0.0); //������ϵ�������������нǵ����ң�
	vec3 diffuseColor = diffuseFactor * pointLight.diffuse * vec3(texture(material.diffuse,TexCoords));   //�������
	//�������յľ��淴���
	vec3 reflectDir = normalize(reflect(-lightDir,normal));   //����ⷽ��
	vec3 viewDir = normalize(viewPos - positionInWorld);  //�۲췽��
	float specularFactor = pow(max(dot(reflectDir,viewDir),0.0),material.shininess); //���淴��ϵ��
	vec3 specularColor = specularFactor * pointLight.specular * vec3(texture(material.specular,TexCoords)); //���淴���
	
	//������Դ��˥��ϵ��
	float distance=length(pointLight.position-positionInWorld);
	float attenuation=1.0f/(pointLight.constant+pointLight.linear*distance+pointLight.quadric*distance*distance);
	//���Դ��ÿ�ֳɷֹⶼִ��˥��
	ambientColor*=attenuation;
	diffuseColor*=attenuation;
	specularColor*=attenuation;
	
	return ambientColor+diffuseColor+specularColor;
}
//����۹�
vec3 CalcSpotLight(SpotLight spotLight){
	//ƬԪƫ��ǣ�����ֵ��
	float theta = dot(normalize(positionInWorld - spotLight.position),normalize(spotLight.lightAxis));
	//ƬԪ�ھ۹���µĹ�ǿ
	float intensity = clamp((theta - spotLight.outCutOff)/(spotLight.inCutOff - spotLight.outCutOff),0.0,1.0);
	//if(theta > spotLight.outCutOff){
		//������
		vec3 ambientColor = spotLight.ambient * vec3(texture(material.diffuse,TexCoords));
		//�������
		vec3 normal = normalize(Normal);
		vec3 lightDir = normalize(spotLight.position - positionInWorld);
		float diffuseFactor = max(dot(normal,lightDir),0.0);
		vec3 diffuseColor = diffuseFactor * spotLight.diffuse * vec3(texture(material.diffuse,TexCoords));
		//���淴���
		vec3 reflectDir = normalize(reflect(-lightDir,normal));
		vec3 viewDir = normalize(viewPos - positionInWorld);
		float specularFactor = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);
		vec3 specularColor = specularFactor * spotLight.specular * vec3(texture(material.specular,TexCoords));
		//�۹��˥��ϵ��
		float distance = length(spotLight.position - positionInWorld);
		float attenuation = 1.0f/(spotLight.constant + spotLight.linear*distance + spotLight.quadric*distance*distance);
		//�۹��ÿ�ֳɷֹⶼִ��˥�������Ȳ�ֵ
		ambientColor *= attenuation * intensity;
		diffuseColor *= attenuation * intensity;
		specularColor *= attenuation * intensity;
		return ambientColor + diffuseColor + specularColor;
	//}
	//else{  //ƬԪ�ھ۹ⷶΧ��ʱʹ�û����⣬ʹ�䲻������ȫ�ڰ�
		//return spotLight.ambient * vec3(texture(material.diffuse,TexCoords));
		//return vec3(0.0f,0.0f,0.0f);
	//}	
}