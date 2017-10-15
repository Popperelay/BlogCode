//������ɫ��
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
//���������1
uniform sampler2D ourTexture1;
//���������2
uniform sampler2D ourTexture2;
void main(){
	color = vec4(ourColor,1.0f) * mix(texture(ourTexture1,TexCoord),texture(ourTexture2,TexCoord),0.41);
}