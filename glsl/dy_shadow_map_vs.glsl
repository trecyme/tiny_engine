#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTagent;
layout (location = 4) in vec4 aBone;
layout (location = 5) in vec4 aWeight;

uniform mat4 boneAnim[74];
uniform mat4 postMtx[74];
uniform mat4 preMtx;
uniform mat4 model;
uniform mat4 lightSpace;


void main()
{
	// 骨骼动画中位置的计算
	ivec4 boneID = ivec4(aBone);
    mat4 BoneTransform = (preMtx * boneAnim[boneID[0]]* postMtx[boneID[0]]) * aWeight[0];
    BoneTransform += (preMtx * boneAnim[boneID[1]] * postMtx[boneID[1]]) * aWeight[1];
    BoneTransform += (preMtx * boneAnim[boneID[2]] * postMtx[boneID[2]]) * aWeight[2];
    BoneTransform += (preMtx * boneAnim[boneID[3]] * postMtx[boneID[3]]) * aWeight[3];
	gl_Position = lightSpace * model * BoneTransform * vec4(aPos, 1.0);

}