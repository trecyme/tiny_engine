#ifndef _FBX_UTIL_H
#define _FBX_UTIL_H

#include "common.h"	


/*
	输出场景中MESH的信息
*/
void printFbxMeshes(FbxScene *scene);

/*
	输出场景中NODE的信息
*/
void printFbxNodes(FbxScene *scene);

/*
	输出场景中姿势的信息
*/
void printFbxPoses(FbxScene *scene);

/*
	输出场景中动画的信息
*/
void printAnimationInfo(FbxScene *scene);

/*
	输出场景中骨骼的信息
*/
void printFbxSkeletons(FbxScene *pScene);

void printFbxVector4(FbxDouble4 v4);

void printFbxAMatrix(FbxAMatrix pTrans);

void printSQTVectors(FbxAMatrix pTrans);

void printFbxMatrix(FbxMatrix mtx);

glm::mat4 FbxMatToGlm(const FbxAMatrix& mat);

FbxAMatrix GlmMatToFbx(const glm::mat4& mat);

#endif // !_FBX_UTIL_H
