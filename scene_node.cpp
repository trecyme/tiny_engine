#include "scene_node.h"
#include "common.h"

void setMaterial(ShadarProgram* shader, Material* material)
{
	shader->use();
	if (material->mtype == Material::eClassic)
	{
		int tID = 0;
		if (material->isValid(material->Diffuse))
			shader->setUniformInt(TEX_DIFFUSE_NAME, tID++);
		if (material->isValid(material->NormalMap))
			shader->setUniformInt(TEX_NORM_NAME, tID++);
		shader->setUniformInt(TEX_DEPTH_NAME, tID++);

	}
	else if (material->mtype == Material::ePBR)
	{
		int tID = 0;
		if (material->isValid(material->Albedo))
			shader->setUniformInt(TEX_PBR_ALBEDO_NAME, tID++);
		if (material->isValid(material->NormalMap))
			shader->setUniformInt(TEX_PBR_NORM_NAME, tID++);
		if (material->isValid(material->AO))
			shader->setUniformInt(TEX_PBR_AO_NAME, tID++);
		if (material->isValid(material->Roughness))
			shader->setUniformInt(TEX_PBR_ROUGE_NAME, tID++);
		shader->setUniformInt(TEX_DEPTH_NAME, tID++);
	}
}

void MeshSceneNode::renderStaticMesh()
{
	for (size_t i = 0; i < mesh->getSubMeshCount(); i++)
	{
		SubMesh& submesh = mesh->submeshes[i];
		setMaterial(shader, &submesh.material);
		shader->setUniformMat4(MTX_PRE_NAME, submesh.bonePreMtx);
		mRender->drawMesh(&submesh);
	}
}

void MeshSceneNode::renderDynamicMeshFrame(Animation* anim)
{
	SkinnedAnimation* sAnim;
	for (size_t i = 0; i < mesh->getSubMeshCount(); i++)
	{
		SubMesh& submesh = mesh->submeshes[i];
		switch (anim->atype)
		{
		case Animation::eSkin:
			sAnim = (SkinnedAnimation*)(anim);
			setMaterial(shader, &submesh.material);
			shader->setUniformMat4Arr(MTX_BONE_NAME, sAnim->getCurBones(), sAnim->nBones);
			shader->setUniformMat4(MTX_PRE_NAME, submesh.bonePreMtx);
			shader->setUniformMat4Arr(MTX_POST_NAME, submesh.bonePostMtx.data(), sAnim->nBones);
			break;
		default:
			break;
		}
		mRender->drawMesh(&submesh);
	}
}

void MeshSceneNode::render()
{
	if (shadowMode)
		return;

	switch (aStatus)
	{
	case AnimatedSceneNode::ePause:
		renderDynamicMeshFrame(curAni);
		break;
	case AnimatedSceneNode::eStatic:
		renderStaticMesh();
		break;
	case AnimatedSceneNode::ePlayLoop:
		curAni->play();
		renderDynamicMeshFrame(curAni);
		break;
	case AnimatedSceneNode::ePlayOnce:
		if (!curAni->isEnd())
		{
			curAni->play();
			renderDynamicMeshFrame(curAni);
		}
		else
		{
			aStatus = AnimatedSceneNode::eStatic;
			renderStaticMesh();
		}
		break;
	case AnimatedSceneNode::eTransition:
		if (!transAni->isEnd())
		{
			transAni->play();
			renderDynamicMeshFrame(curAni);
		}
		else
		{
			curAni = nextAni;
			curAni->resetAndPlay();
			renderDynamicMeshFrame(curAni);
		}
		break;
	default:
		
		break;
	}
}

void MeshSceneNode::onPlayOnce(Animation* ani)
{
	aStatus = AnimatedSceneNode::ePlayOnce;
	this->curAni = ani;
}

void MeshSceneNode::onPlayLoop(Animation* ani)
{
	aStatus = AnimatedSceneNode::ePlayLoop;
	this->curAni = ani;

}
void MeshSceneNode::onTransition(Animation* nextAni, int nIntl)
{
	aStatus = AnimatedSceneNode::eTransition;
	if (transAni)
	{
		delete transAni;
		transAni = 0;
	}
	this->transAni = curAni->newTransitionAnim(nextAni, nIntl);
	this->nextAni = nextAni;

}
void MeshSceneNode::onPause()
{
	aStatus = AnimatedSceneNode::ePause;
}
void MeshSceneNode::onStop()
{
	aStatus = AnimatedSceneNode::eStatic;
}
