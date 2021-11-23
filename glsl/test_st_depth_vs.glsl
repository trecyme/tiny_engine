#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 pos;

uniform mat4 model;
uniform mat4 lightSpace;

void main()
{
	vec4 p = model * vec4(aPos, 1.0);
	gl_Position = lightSpace * p;
	pos = gl_Position;
}