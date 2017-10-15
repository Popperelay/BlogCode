#version 330 core
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
uniform vec3 lightColor;  //��Դ��ɫ
uniform vec3 objectColor; //���屾����ɫ
uniform vec3 lightPos;    //��Դλ��
uniform vec3 viewPos;     //�۲�λ��
void main(){
	//���㻷����
	float ambientStrength=0.3f;  //��������	
	vec3 ambientColor=ambientStrength*lightColor;  //������
	//�����������
	vec3 normal=normalize(Normal);  //��λ������
	vec3 lightDir=normalize(lightPos-positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor=max(dot(normal,lightDir),0.0); //�������ϵ�������������нǵ����ң�
	vec3 diffuseColor=diffuseFactor*lightColor;  //�������
	//���㾵�淴���
	vec3 reflectDir=normalize(reflect(-lightDir,normal));   //����ⷽ��
	vec3 viewDir=normalize(viewPos-positionInWorld);  //�۲췽��
	float specularFactor=pow(max(dot(reflectDir,viewDir),0.0),32); //���淴��ϵ��
	float specularStrength=0.5f; //���淴��ǿ��
	vec3 specularColor=specularStrength*specularFactor*lightColor; //���淴���
	color=vec4((ambientColor+diffuseColor+specularColor)*objectColor,1.0f);
}