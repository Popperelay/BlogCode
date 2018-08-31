#version 430 core

layout(location = 0) in vec3 _Position;

void main()
{
    gl_Position = vec4(_Position, 1.0);
}