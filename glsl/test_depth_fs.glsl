#version 330 core
in vec4 pos;
out vec4 fcolor;

uniform sampler2D surfaceTex;
uniform sampler2D normTex;
uniform sampler2D depthMap;

void main()
{           
	vec3 lpos = pos.xyz / pos.w;
	vec2 uv = lpos.xy * 0.5 + 0.5;
	float r = texture(depthMap, uv).r;
	fcolor = vec4(vec3(r), 1.f);
}