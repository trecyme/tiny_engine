#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTagent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 lightSpace;
uniform vec3 lightDir;
uniform vec3 viewPos;

out FRAG {
	vec2 uv;
	vec3 tangentPos;
	vec3 tangentViewPos;
	vec3 tangentLightDir;
	vec4 lightSpacePos;
} frag;


void main()
{
	vec4 PosL = model * vec4(aPos, 1.0);              // 世界坐标系中的顶点
//    gl_Position = lightSpace * PosL;
    gl_Position = projection * view * PosL;

	// 切线空间计算
	mat3 nMat = transpose(inverse(mat3(model)));
	vec3 T = normalize(nMat * aTagent);
    vec3 N = normalize(nMat *aNormal);
    vec3 B = cross(T, N);
    mat3 TBN = transpose(mat3(T, B, N));

	// 传递给fs
	frag.uv = aUV;
	frag.tangentPos = TBN * PosL.xyz;
	frag.tangentViewPos = TBN * viewPos;
	frag.tangentLightDir = TBN * lightDir;
	frag.lightSpacePos = lightSpace * PosL;

}