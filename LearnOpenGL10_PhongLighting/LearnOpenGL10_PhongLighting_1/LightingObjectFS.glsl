#version 330 core
in vec3 Normal;
in vec3 positionInWorld;
out vec4 color;
uniform vec3 lightColor;  //��Դ��ɫ
uniform vec3 objectColor; //���屾����ɫ
uniform vec3 lightPos;    //��Դλ��
void main(){
	//���㻷����
	float ambientStrength=0.3f;  //��������	
	vec3 ambientColor=ambientStrength*lightColor;  //������
	//�����������
	vec3 normal=normalize(Normal);  //��λ������
	vec3 lightDir=normalize(lightPos-positionInWorld); //�������䷽��ĵ�λ����
	float diffuseFactor=max(dot(normal,lightDir),0.0); //�������ϵ�������������нǵ����ң�
	vec3 diffuseColor=diffuseFactor*lightColor;  //�������
	color=vec4((ambientColor+diffuseColor)*objectColor,1.0f);
}