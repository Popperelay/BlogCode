#version 330 core
//���ʽṹ��
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
//���Դ�ṹ��
struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant; //������
	float linear;   //һ����
	float quadric;  //������
};
in vec3 Normal;
in vec3 positionInWorld;
in vec2 TexCoords;
out vec4 color;
uniform Material material; //���ʱ���
uniform Light light;       //��Դ����
uniform vec3 viewPos;      //�۲�λ��
void main(){
	//�������յĻ�����
	vec3 ambientColor=light.ambient * vec3(texture(material.diffuse,TexCoords));  //������
	//�������յ��������
	vec3 normal=normalize(Normal);  //��λ������
	vec3 lightDir=normalize(light.position-positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor=max(dot(normal,lightDir),0.0); //������ϵ�������������нǵ����ң�
	vec3 diffuseColor=diffuseFactor * light.diffuse * vec3(texture(material.diffuse,TexCoords));   //�������
	//�������յľ��淴���
	vec3 reflectDir=normalize(reflect(-lightDir,normal));   //����ⷽ��
	vec3 viewDir=normalize(viewPos-positionInWorld);  //�۲췽��
	float specularFactor=pow(max(dot(reflectDir,viewDir),0.0),material.shininess); //���淴��ϵ��
	vec3 specularColor=specularFactor * light.specular * vec3(texture(material.specular,TexCoords)); //���淴���
	
	//������Դ��˥��ϵ��
	float distance=length(light.position-positionInWorld);
	float attenuation=1.0f/(light.constant+light.linear*distance+light.quadric*distance*distance);
	//���Դ��ÿ�ֳɷֹⶼִ��˥��
	ambientColor*=attenuation;
	diffuseColor*=attenuation;
	specularColor*=attenuation;
	color=vec4(ambientColor+diffuseColor+specularColor,1.0f);
}