#version 430 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
uniform vec3 lightColor;  //��Դ��ɫ
uniform vec3 lightPos;    //��Դλ��
uniform vec3 viewPos;     //�۲�λ��
uniform sampler2D planeTexSampler;
void main(){
	vec3 objectColor = texture(planeTexSampler, TexCoords).rgb;
	//���㻷����
	float ambientStrength = 0.01f;  //��������	
	vec3 ambientColor = ambientStrength * objectColor;  //������
	//�����������
	vec3 normal = normalize(Normal);  //��λ������
	vec3 lightDir = normalize(lightPos - positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor = max(dot(normal,lightDir), 0.0); //�������ϵ�������������нǵ����ң�
	vec3 diffuseColor = diffuseFactor * lightColor;  //�������
	//���㾵�淴���
	/*//Phongģ�ͼ��㾵���
	vec3 reflectDir = normalize(reflect(-lightDir, normal));   //����ⷽ��
	vec3 viewDir = normalize(viewPos - positionInWorld);  //�۲췽��
	float specularFactor = pow(max(dot(reflectDir, viewDir), 0.0), 0.5); //���淴��ϵ��*/
	//Blinn-Phongģ�ͼ��㾵���
	vec3 viewDir = normalize(viewPos - positionInWorld);  //�۲췽��
	vec3 halfDir = normalize(lightDir + viewDir);
	float specularFactor = pow(max(dot(halfDir, normal), 0.0f), 0.5);
	vec3 specularColor =  specularFactor * lightColor; //���淴���
	
	color = vec4((diffuseColor + specularColor) * objectColor + ambientColor, 1.0f);
}