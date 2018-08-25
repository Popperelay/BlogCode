#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_VPMatrices4LookAtSixFaces[6];

out vec4 g2f_WorldPos;

void main()
{
    for(int Face = 0; Face < 6; ++Face)
    {
        gl_Layer = Face;
        for(int i = 0; i < 3; ++i)
        {
            g2f_WorldPos = gl_in[i].gl_Position;
            gl_Position = u_VPMatrices4LookAtSixFaces[Face] * g2f_WorldPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}