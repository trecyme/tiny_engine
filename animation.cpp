#include <Windows.h>
#include "animation.h"
#include "fbx_util.h"

// ----------------------------------- CLASS: SkinnedAnimation ------------------------------------ //
void SkinnedAnimation::play()
{ 
	if (firstFrame)
	{
		firstFrame = false;
		return;
	}
	
	nowFrame = nowFrame == nFrames - 1 ? 0 : nowFrame + 1; 
	toEnd = nowFrame == nFrames - 1 ? true : false;
}

void SkinnedAnimation::resetAndPlay()
{
	if (firstFrame)
	{
		firstFrame = false;
	}

	nowFrame = 0;
	toEnd = false;
}

void SkinnedAnimation::reset()
{
	firstFrame = true;
	nowFrame = 0;
	toEnd = false;
}

glm::mat4* SkinnedAnimation::getCurBones()
{
	if (nowFrame >= 0 && nowFrame < nFrames)
	{
		return globalBoneTransforms.at(nowFrame).data();
	}
	else
	{
		return 0;
	}
}

glm::mat4* SkinnedAnimation::getCurLocalBones()
{
	if (nowFrame >= 0 && nowFrame < nFrames)
	{
		return localBoneTransforms.at(nowFrame).data();
	}
	else
	{
		return 0;
	}
}


Animation* SkinnedAnimation::newTransitionAnim(Animation* nextAnim, int nIntl)
{
	if (nIntl <= 0)
		return 0;

	// check if the same atype
	SkinnedAnimation* nextSAnim;
	SkinnedAnimation* transSAnim;
	try
	{
		//nextSAnim = dynamic_cast<SkinnedAnimation*>(nextAnim);
		nextSAnim = (SkinnedAnimation*)(nextAnim);
	}
	catch (const std::exception&)
	{
		return 0;
	}

	// check if nextAnim is the same skeleton
	if (!(nBones == nextSAnim->nBones 
		&& parent == nextSAnim->parent 
		&& boneNameMapIdx == nextSAnim->boneNameMapIdx
		&& boneNames == nextSAnim->boneNames
		))
	{
		return 0;
	}
	std::vector<std::vector<glm::mat4>> temp = std::vector<std::vector<glm::mat4>>(nIntl);
	transSAnim = new SkinnedAnimation(nBones, nIntl, temp);

	// transition
	float factor = 1.f / (nIntl + 2);
	std::vector<glm::mat4>& lastframeBones = localBoneTransforms[nowFrame];
	std::vector<glm::mat4>& nextframeBones = nextSAnim->localBoneTransforms[nowFrame];
	std::vector<std::vector<glm::mat4>>& framesBones = transSAnim->globalBoneTransforms;
	for (size_t i = 0; i < nIntl; i++)
	{
		float beta = (i + 1) * factor;
		std::vector<glm::mat4>& frameBones = framesBones[i];

		for (size_t bIdx = 0; bIdx < nBones; bIdx++)
		{
			FbxAMatrix lastBone = GlmMatToFbx(lastframeBones[bIdx]);
			FbxAMatrix nextBone = GlmMatToFbx(nextframeBones[bIdx]);

			// 线性插值 - 平移向量
			const double b = beta;
			FbxVector4 lastTranslate = lastBone.GetT();
			FbxVector4 nextTranslate = nextBone.GetT();
			FbxVector4 newTranslate = lastTranslate * (1 - b) + nextTranslate * b;

			// 线性插值 - 缩放向量
			FbxVector4 lastScale = lastBone.GetS();
			FbxVector4 nextScale = nextBone.GetS();
			FbxVector4 newScale = lastScale * (1 - b) + nextScale * b;

			// SLERP插值 - 旋转四元数
			FbxQuaternion lastRotation = lastBone.GetQ();
			FbxQuaternion nextRotation = nextBone.GetQ();
			FbxQuaternion newRotation = slerp(lastRotation, nextRotation, beta);

			glm::mat4 newLocalTrans = FbxMatToGlm(FbxAMatrix(newTranslate, newRotation, lastScale));
			if (parent[bIdx] == -1)
			{
				frameBones.push_back(newLocalTrans);
			}
			else
			{
				glm::mat4 parGlobalTrans = frameBones[parent[bIdx]];
				frameBones.push_back(parGlobalTrans * newLocalTrans);
			}
		}
	}

	return transSAnim;
}


// ----------------------------------- FUNC ------------------------------------ //

FbxQuaternion slerp(FbxQuaternion v0, FbxQuaternion v1, float t)
{
	// Only unit quaternions are valid rotations.
	// Normalize to avoid undefined behavior.
	v0.Normalize();
	v1.Normalize();

	// Compute the cosine of the angle between the two vectors.
	double dot = v0.DotProduct(v1);

	// If the dot product is negative, slerp won't take
	// the shorter path. Note that v1 and -v1 are equivalent when
	// the negation is applied to all four components. Fix by 
	// reversing one quaternion.
	if (dot < 0.0f) {
		v1 = -v1;
		dot = -dot;
	}

	const double DOT_THRESHOLD = 0.9995;
	if (dot > DOT_THRESHOLD) {
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.
		FbxQuaternion result = v0 + (v1 - v0) * t;
		result.Normalize();
		return result;
	}

	// Since dot is in range [0, DOT_THRESHOLD], acos is safe
	double theta_0 = acos(dot);        // theta_0 = angle between input vectors
	double theta = theta_0 * t;          // theta = angle between v0 and result
	double sin_theta = sin(theta);     // compute this value only once
	double sin_theta_0 = sin(theta_0); // compute this value only once

	double s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
	double s1 = sin_theta / sin_theta_0;

	return (v0 * s0) + (v1 * s1);
}

