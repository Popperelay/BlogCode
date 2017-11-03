#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 VS_TexCoords[];
out vec2 GS_TexCoords;
uniform float time;
vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[1].gl_Position) - vec3(gl_in[2].gl_Position);
	return normalize(cross(a, b));
}
vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0f;
	vec3 direction = normal * magnitude * ((sin(time) + 1.0f)/2.0f);
	return position + vec4(direction, 0.0f);
}
void main(){
	vec3 normal = GetNormal();
	gl_Position = explode(gl_in[0].gl_Position, normal);
	GS_TexCoords = VS_TexCoords[0];
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	GS_TexCoords = VS_TexCoords[1];
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	GS_TexCoords = VS_TexCoords[2];
	EmitVertex();
    EndPrimitive();
}