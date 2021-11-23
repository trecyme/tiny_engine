#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H

#include "common.h"
#include "meshvao.h"
#include "fbx_parser.h"
#include "scene_node.h"
#include "animation.h"
#include "light.h"

class GraphicsRender;

// ---------------------------------- CLASS - SceneManager --------------------------------- //

class SceneManager
{
private:
	std::vector<SceneNode * > drawables; // for mesh and cube
	std::vector<SceneNode* > lights;
	std::vector<SceneNode* > cameras;
	std::vector<SceneNode* > skyboxes;
	std::vector<Animation*> anims;
	std::vector<Mesh*> meshes;
	CameraSceneNode* curCamera = 0;
	LightSceneNode* curLight = 0;
	SkyboxSceneNode* curSkybox = 0;
	GraphicsRender* mRender = 0;
	FbxLoader fbxloader;
	bool hasShadow;
	glm::mat4 lightSpaceMatrix;

	void getShader(SceneNode* node, ShadarProgram*& shader, bool isShadow);
	void bindDepthTex(SceneNode* node);

public:
	SceneManager(GraphicsRender* render) : mRender(render){}
	~SceneManager();

	void setCurCamera(CameraSceneNode* cameraNode) { curCamera = cameraNode; }
	void setCurLight(LightSceneNode* lightNode) { curLight = lightNode; }
	void setCurSkybox(SkyboxSceneNode*  skyNode) { curSkybox = skyNode; }
	void enableShadow() { hasShadow = true; }
	void disableShadow() { hasShadow = false; }
	
	void render();
	Mesh* loadMesh(std::string& path, Material::MaterialType mtype);

	MeshSceneNode* createMeshSceneNode(Mesh* mesh);
	SkyboxSceneNode* createSkyboxSceneNode(std::vector<std::string>& imagePaths);
	CubeSceneNode* createCubeSceneNode(Cube &cube);
	Animation* createAnimator(std::string& path, Animation::AnimType atype);
	CameraSceneNode* createCameraSceneNode(glm::vec3 &pos, glm::vec3& target);
	LightSceneNode* createLightSceneNode(Light& light);
};

#endif