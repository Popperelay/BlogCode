#version 330 core
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
//���ʽṹ��
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
//��Դ
struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Material material; //���ʱ���
uniform Light light;       //��Դ����
uniform vec3 viewPos;      //�۲�λ��
void main(){
	//�������յĻ�����
	vec3 ambientColor = light.ambient*material.ambient;  //������
	//�������յ��������
	vec3 normal = normalize(Normal);  //��λ������
	vec3 lightDir = normalize(light.position - positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor = max(dot(normal, lightDir),0.0); //������ϵ�������������нǵ����ң�
	vec3 diffuseColor = diffuseFactor * light.diffuse * material.diffuse;   //�������
	//�������յľ��淴���
	vec3 reflectDir = normalize(reflect(-lightDir, normal));   //����ⷽ��
	vec3 viewDir = normalize(viewPos - positionInWorld);  //�۲췽��
	float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess); //���淴��ϵ��
	vec3 specularColor = specularFactor * light.specular * material.specular; //���淴���
	color = vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}