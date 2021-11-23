#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTagent;
layout (location = 4) in vec4 aBone;
layout (location = 5) in vec4 aWeight;

out vec4 pos;

uniform mat4 boneAnim[59];
uniform mat4 lightSpace;
uniform mat4 model;


void main()
{
	// 骨骼动画中位置的计算
    ivec4 boneID = ivec4(aBone);
    mat4 BoneTransform = boneAnim[boneID[0]] * aWeight[0];
    BoneTransform += boneAnim[boneID[1]] * aWeight[1];
    BoneTransform += boneAnim[boneID[2]] * aWeight[2];
    BoneTransform += boneAnim[boneID[3]] * aWeight[3];
	vec4 p = model * BoneTransform * vec4(aPos, 1.0);
	gl_Position = lightSpace * p;
	pos = gl_Position;

}