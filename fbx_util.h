#ifndef _FBX_UTIL_H
#define _FBX_UTIL_H

#include "common.h"	


/*
	���������MESH����Ϣ
*/
void printFbxMeshes(FbxScene *scene);

/*
	���������NODE����Ϣ
*/
void printFbxNodes(FbxScene *scene);

/*
	������������Ƶ���Ϣ
*/
void printFbxPoses(FbxScene *scene);

/*
	��������ж�������Ϣ
*/
void printAnimationInfo(FbxScene *scene);

/*
	��������й�������Ϣ
*/
void printFbxSkeletons(FbxScene *pScene);

void printFbxVector4(FbxDouble4 v4);

void printFbxAMatrix(FbxAMatrix pTrans);

void printSQTVectors(FbxAMatrix pTrans);

void printFbxMatrix(FbxMatrix mtx);

glm::mat4 FbxMatToGlm(const FbxAMatrix& mat);

FbxAMatrix GlmMatToFbx(const glm::mat4& mat);

#endif // !_FBX_UTIL_H
