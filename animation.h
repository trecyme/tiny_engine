#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "common.h"

class Animation
{
public:
	enum AnimType
	{
		eBlendShape,
		eSkin,
		eVertex
	};
	AnimType atype;
	
	
	Animation(AnimType atype) : atype(atype) {}
	
	virtual void play() = 0;
	virtual bool isEnd() = 0;
	virtual void resetAndPlay() = 0;
	virtual Animation* newTransitionAnim(Animation* nextAnim, int nIntl) = 0;

};

/*
   ¹Ç÷À¶¯»­
*/
class SkinnedAnimation : Animation
{
private:
	int nowFrame = 0;
	bool firstFrame = true;
	bool toEnd = false;

public:
	int nFrames = 0;
	int nBones = 0;

	std::vector<std::string> boneNames;
	std::map<std::string, int> boneNameMapIdx;
	std::vector<std::vector<glm::mat4>> globalBoneTransforms;
	std::vector<std::vector<glm::mat4>> localBoneTransforms;
	std::vector<int> parent;

	SkinnedAnimation() : Animation(Animation::eSkin) {}
	SkinnedAnimation(int nFrames, int nBones, std::vector<std::vector<glm::mat4>>& globalBoneTransforms)
		: Animation(Animation::eSkin), nFrames(nFrames), nBones(nBones), globalBoneTransforms(globalBoneTransforms) {}
	~SkinnedAnimation() = default;
	
	void play();
	void reset();
	void resetAndPlay();
	bool isEnd() { return toEnd; };
	Animation* newTransitionAnim(Animation* nextAnim, int nIntl);


	glm::mat4* getCurBones();
	glm::mat4* getCurLocalBones();

};

FbxQuaternion slerp(FbxQuaternion v0, FbxQuaternion v1, float t);

#endif // !#define _ANIMATION_H
