#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H

#include "animation.h"
#include "meshvao.h"
#include "fbx_parser.h"
#include "camera.h"
#include "light.h"
#include "shader_program.h"
#include "context.h"


// ---------------------------------- CLASS - SceneNode --------------------------------- //
class SceneNode
{
public:
	enum NodeType {
		eLight,
		eCamera,
		eSkybox,
		eCube,
		eMesh,
		eRoot
	};


	NodeType ntype;
	glm::mat4 model;

	SceneNode(NodeType ntype = SceneNode::eRoot) : ntype(ntype) {}
	virtual void setModel(glm::mat4 model) { this->model = model; }

	virtual void render() = 0;
};

class AnimatedSceneNode : public SceneNode
{
public:
	enum AnimStatus {
		ePlayOnce,
		ePlayLoop,
		eStatic,
		ePause,
		eTransition
	};


	AnimStatus aStatus;


	AnimatedSceneNode(SceneNode::NodeType ntype = SceneNode::eRoot, AnimStatus astatus = AnimatedSceneNode::eStatic) : SceneNode(ntype), aStatus(astatus) {}
	virtual void onPlayOnce(Animation* ani) = 0;
	virtual void onPlayLoop(Animation* ani) = 0;
	virtual void onTransition(Animation* nextAni, int nIntl) = 0;
	virtual void onPause() = 0;
	virtual void onStop() = 0;


};

// ---------------------------------- CLASS - CameraSceneNode --------------------------------- //
class CameraSceneNode : public SceneNode
{
	Camera camera;

public:
	CameraSceneNode(glm::vec3 pos, glm::vec3 target, float fov = 45.f,
		float near = 0.1f, float far = 250.0f, float ratio = 4.f / 3.f,
		float speed = 1.0f, float sensity = 0.1f) : camera(pos, target, fov, near, far, ratio, speed, sensity) {}

	virtual void render() {}
	Camera& getCamera() { return camera; }

};

// ---------------------------------- CLASS - LightSceneNode --------------------------------- //
class LightSceneNode : public SceneNode
{
public:
	Light light;


	LightSceneNode() = default;
	LightSceneNode(Light &lt) : light(lt) { }
	
	virtual void render() {}

};

// ---------------------------------- CLASS - MeshSceneNode --------------------------------- //

class MeshSceneNode : public AnimatedSceneNode
{
	Mesh* mesh = 0;
	Animation* curAni = 0;
	Animation* transAni = 0;
	Animation* nextAni = 0;
	GraphicsRender* mRender = 0;
	ShadarProgram* shader = 0;

	bool shadowMode = false;

	void renderStaticMesh();
	void renderDynamicMeshFrame(Animation*);

public:

	MeshSceneNode(GraphicsRender* render, Mesh* mesh) : AnimatedSceneNode(SceneNode::eMesh),  mRender(render), mesh(mesh){}
	~MeshSceneNode() = default;
	Mesh* getMesh() { return mesh; }
	bool isAnimated() { return aStatus != AnimatedSceneNode::eStatic; }
	virtual void setShader(ShadarProgram* shader) { this->shader = shader; }
	virtual void render();
	virtual void onPlayOnce(Animation* ani);
	virtual void onPlayLoop(Animation* ani);
	virtual void onTransition(Animation* nextAni, int nIntl);
	virtual void onPause();
	virtual void onStop();

	void setShadowMode() { shadowMode = true; }
	void disableShadowMode() { shadowMode = false; }


};

// ---------------------------------- CLASS - SkyboxSceneNode --------------------------------- //


class CubeSceneNode : public  SceneNode
{
	Cube cube;
	GraphicsRender* mRender;
	ShadarProgram* shader = 0;

public:

	CubeSceneNode(GraphicsRender* render, Cube& cube)
		: SceneNode(SceneNode::eCube), mRender(render), cube(cube) {}
	~CubeSceneNode() = default;

	virtual void setShader(ShadarProgram* shader) { this->shader = shader; }
	virtual void render() { mRender->drawCube(&cube); }
	
	Cube* getCube() { return &cube; }
};

// ---------------------------------- CLASS - SkyboxSceneNode --------------------------------- //

class SkyboxSceneNode : public SceneNode
{
	SkyBox skybox;
	GraphicsRender* mRender;

public:

	SkyboxSceneNode(GraphicsRender* render, std::vector<std::string>& paths) 
		: SceneNode(SceneNode::eSkybox), mRender(render) 
	{
		skybox.tex = Texture3D(paths.data(), false);
	}
	~SkyboxSceneNode() = default;
	
	virtual void render() { mRender->drawSkybox(&skybox); }
};

#endif