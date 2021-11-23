#include "scene_manager.h"
#include "shader_program.h"
#include <typeinfo>
#include "context.h"

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < drawables.size(); i++)
		delete drawables[i];

	for (size_t i = 0; i < cameras.size(); i++)
		delete cameras[i];

	for (size_t i = 0; i < lights.size(); i++)
		delete lights[i];

	for (size_t i = 0; i < skyboxes.size(); i++)
		delete skyboxes[i];

	for (size_t i = 0; i < anims.size(); i++)
		delete anims[i];

	for (size_t i = 0; i < meshes.size(); i++)
		delete meshes[i];
}

Mesh* SceneManager::loadMesh(std::string& path, Material::MaterialType mtype)
{
	Mesh* mesh = new Mesh();
	if (path.find(".fbx") != std::string::npos || path.find(".FBX") != std::string::npos)
	{
		fbxloader.loadMesh(mesh, path.data(), mtype);
	}
	meshes.push_back(mesh);
	return mesh;

}

MeshSceneNode* SceneManager::createMeshSceneNode(Mesh *mesh)
{
	MeshSceneNode* mnode = new MeshSceneNode(mRender, mesh);
	SceneNode* node = (SceneNode*)mnode;
	drawables.push_back(node);
	return mnode;
}

SkyboxSceneNode* SceneManager::createSkyboxSceneNode(std::vector<std::string>& imagePaths)
{
	SkyboxSceneNode* snode = new SkyboxSceneNode(mRender, imagePaths);
	SceneNode* node = (SceneNode*)snode;
	skyboxes.push_back(node);
	return snode;
}

CubeSceneNode* SceneManager::createCubeSceneNode(Cube& cube)
{
	CubeSceneNode* snode = new CubeSceneNode(mRender, cube);
	SceneNode* node = (SceneNode*)snode;
	drawables.push_back(node);
	return snode;
}

Animation* SceneManager::createAnimator(std::string& path, Animation::AnimType atype)
{
	Animation* anim;

	if (path.find(".fbx") != std::string::npos || path.find(".FBX") != std::string::npos)
	{
		switch (atype)
		{
		case Animation::eSkin:
			anim = (Animation*)new SkinnedAnimation();
			fbxloader.loadAnim(anim, path.data());
			break;
		default:
			break;
		}
	}
	// FUNTURE else if(other filetype) ....
	anims.push_back(anim);
	return anim;
}

CameraSceneNode* SceneManager::createCameraSceneNode(glm::vec3& pos, glm::vec3& target)
{
	CameraSceneNode* node = new CameraSceneNode(pos, target);
	cameras.push_back((SceneNode*)node);
	curCamera = node;
	return node;
}

LightSceneNode* SceneManager::createLightSceneNode(Light& light)
{
	LightSceneNode* node = new LightSceneNode(light);
	lights.push_back((SceneNode*)node);
	curLight = node;
	return node;
}

void  SceneManager::getShader(SceneNode* node, ShadarProgram*& shader, bool isShadow)
{
	const char* classname = typeid(*node).name();
	if (!strcmp(classname, "class MeshSceneNode"))
	{
		MeshSceneNode* mnode = (MeshSceneNode*)(node);
		shader = mRender->getShader(curLight->light.etype, mnode->getMesh()->mtype, mnode->isAnimated(), isShadow);
		mnode->setShader(shader);
	}
	else if (!strcmp(classname, "class CubeSceneNode"))
	{
		CubeSceneNode* mnode = (CubeSceneNode*)(node);
		shader = mRender->getShader(curLight->light.etype, mnode->getCube()->m.mtype, false, isShadow);
		mnode->setShader(shader);
	}
	else
	{
		throw "NOT IMPELEMENT!";
	}
}

void SceneManager::bindDepthTex(SceneNode *node)
{
	Material::MaterialType mtype = Material::eClassic;
	bool hasMaterial = true;
	const char* classname = typeid(*node).name();
	if (strcmp(classname, "class MeshSceneNode"))
		mtype = ((MeshSceneNode*)node)->getMesh()->mtype;
	else if (strcmp(classname, "class CubeSceneNode"))
		mtype = ((CubeSceneNode*)node)->getCube()->m.mtype;
	else
		hasMaterial = false;

	// FUTURE 2/4 is wrong
	bool isParalLight = curLight->light.etype == Light::eParallel;
	if (!hasMaterial && mtype == Material::eClassic)
		mRender->bindAndActiveDepTex(2, isParalLight);
	else if (mtype == Material::ePBR)
		mRender->bindAndActiveDepTex(4, isParalLight);
}


void SceneManager::render()
{
	
	// if shadow is on, draw it
	if (hasShadow)
	{
		// create frame buffer for shadow; create light space
		if (!mRender->hasFrameBuffer())
		{
			if (curLight->light.etype == Light::eParallel)
			{ 
				GLfloat near_plane = 0.1f, far_plane = 200.f;
				GLfloat radius = 25;
				glm::mat4 lightProjection = glm::ortho(-radius, radius, -radius, radius, near_plane, far_plane);
				glm::mat4 lightView = glm::lookAt(curLight->light.pos, curLight->light.pos - curLight->light.dir, glm::vec3(0, 1.0, 0));
				lightSpaceMatrix = lightProjection * lightView;
				mRender->createDepthMapFrameBuffer();
			}
			else
			{
				mRender->createOmnDepthMapFrameBuffer();
				// FUTURE dot light shadow
			}
		}

		mRender->renderShadowPre();
		for (size_t i = 0; i < drawables.size(); i++)
		{
			// retrive shader according to sth.
			SceneNode* node = drawables[i];
			ShadarProgram* shader = 0;
			getShader(node, shader, true);

			// when different light type
			if (curLight->light.etype == Light::eParallel)
			{
				shader->use();
				shader->setUniformMat4("lightSpace", lightSpaceMatrix);
				shader->setUniformMat4(MTX_MODEL_NAME, node->model);
			}
			else
			{
				// FUTURE spot light space setting
				throw "NOT IMPELEMENT: spot light shader";
			}
			
			if (typeid(*node) == typeid(MeshSceneNode))
			{
				MeshSceneNode* mnode = (MeshSceneNode*)node;
				mnode->setShadowMode();
			}
			node->render();
		}
		mRender->renderShadowPost();
	}

	// draw sth.
	mRender->renderPre();
	// use camera' view and proj to set uniform
	glm::mat4 view = curCamera->getCamera().view_matrix();
	glm::mat4 proj = curCamera->getCamera().projection_maxtrix();
	for (size_t i = 0; i < drawables.size(); i++)
	{
		SceneNode* node = drawables[i];
		ShadarProgram* shader = 0;
		getShader(node, shader, false);

		// when different light type
		if (curLight->light.etype == Light::eParallel)
		{
			shader->use();
			shader->setUniformMat4(MTX_MODEL_NAME, node->model);
			shader->setUniformMat4(MTX_PROJ_NAME, proj);
			shader->setUniformMat4(MTX_VIEW_NAME, view);
			if (hasShadow) shader->setUniformMat4("lightSpace", lightSpaceMatrix);
			if (hasShadow) shader->setUniformVec3("viewPos", curCamera->getCamera().pos);
			if (hasShadow) shader->setUniformVec3("lightDir", curLight->light.dir);
		}
		else
		{
			// FUTURE spot light space setting
		}
		if (hasShadow) { bindDepthTex(node); }
		
		if (typeid(*node) == typeid(MeshSceneNode))
		{
			MeshSceneNode* mnode = (MeshSceneNode*)node;
			mnode->disableShadowMode();
		}
		node->render();
	}

	// draw skybox
	
	/*
	ShadarProgram* skyShadar = mRender->getSkyboxShader();
	skyShadar->use();
	glm::mat4 skyview = glm::mat4(glm::mat3(view));
	skyShadar->setUniformMat4(MTX_VIEW_NAME, skyview);
	skyShadar->setUniformMat4(MTX_PROJ_NAME, proj);
	skyShadar->setUniformInt("skybox", 0);
	curSkybox->render();*/


}
