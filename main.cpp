#include <vector>
#include "context.h"
#include "scene_manager.h"
#include "scene_node.h"
#include "material.h"
#include "collision.h"

// ------------------------------------------------------------------------------- //
#define FLOOR_TEX_PATH "texture/floor_stone.png"
#define FLOOR_NM_TEX_PATH "texture/floor_stone_nm.png"
#define BOX_TEX_PATH "texture/box.png"
#define BOX_NM_TEX_PATH "texture/box_nm.jpg"

// ------------------------------------------------------------------------------- //
const int window_width = 800;
const int window_height = 600;
std::string meshFbxPath = "tangyuannew/rig_tangyuannew.fbx";
std::vector<std::string> animPaths {
		"tangyuannew/animation/Body/tangyuannew@tangyuannew_body_bandunzou.fbx",
		"tangyuannew/animation/Body/tangyuannew@tangyuannew_body_dimianbao.fbx",
		"tangyuannew/animation/Body/tangyuannew@tangyuannew_body_fuwo.fbx",
		"tangyuannew/animation/Body/tangyuannew@tangyuannew_body_bandun.fbx",
		"tangyuannew/animation/Body/tangyuannew@tangyuannew_body_bihuajiaotan.fbx" 
};
std::vector<std::string> faces {
	"skybox/default/right.jpg",
	"skybox/default/left.jpg",
	"skybox/default/top.jpg",
	"skybox/default/bottom.jpg",
	"skybox/default/front.jpg",
	"skybox/default/back.jpg"
};
std::vector<float> cube_with_uv_nom_vertices{
	// positions           // texcoords     // normals         //tangent
	// back face
	-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // bottom-left
	 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // top-right
	 1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // bottom-right         
	 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // top-right
	-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // bottom-left
	-1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f, // top-left
	// front face
	-1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // bottom-left
	 1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // bottom-right
	 1.0f,  1.0f,  1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // top-right
	 1.0f,  1.0f,  1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // top-right
	-1.0f,  1.0f,  1.0f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // top-left
	-1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,   -1.0f, 0.0f, 0.0f, // bottom-left
	// left face
	-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // top-right
	-1.0f,  1.0f, -1.0f,  1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // top-left
	-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // bottom-left
	-1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f,  0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // bottom-right
	-1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, -1.0f, // top-right
	// right face
	 1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // top-left
	 1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // bottom-right
	 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // top-right         
	 1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // bottom-right
	 1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // top-left
	 1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 1.0f, // bottom-left     
	// bottom face
	-1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-right
	 1.0f, -1.0f, -1.0f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-left
	 1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // bottom-left
	 1.0f, -1.0f,  1.0f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // bottom-left
	-1.0f, -1.0f,  1.0f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // bottom-right
	-1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-right
	// top face
	-1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-left
	 1.0f,  1.0f , 1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // bottom-right
	 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-right     
	 1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // bottom-right
	-1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f, // top-left
	-1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f  // bottom-left        
};

// ------------------------------------------------------------------------------- //
class Application : EventListener
{
	
	RenderContext context;
	SceneManager* sMngr;
	GraphicsRender* gRdr;

	CameraSceneNode* cameraNode;
	LightSceneNode* lightNode;
	SkyboxSceneNode* skyNode;
	MeshSceneNode* charater;
	CubeSceneNode* floor;
	CubeSceneNode* box;
	std::vector<Animation*> anims;
	int curAnimID = 0;

	glm::mat4 catScaleModel;
	glm::mat4 floorModel;
	glm::mat4 boxModel;

	AABB boxAABB;
	AABB initcatAABB;
	Movement movement;

	const int MAX_FOV = 45;

public:
	Application() : 
		context(RenderContext::eOpenGL, window_width, window_height, "CAT application"), 
		movement(glm::vec3(0, 0, 1.f), glm::mat4(1), 5)
	{
		sMngr = context.getSceneManager();
		gRdr = context.getGraphicsRender();
		gRdr->setEventListener(this);
		sMngr->enableShadow();

		// create camera
		glm::vec3 cPos = glm::vec3(0, 1, 10);
		glm::vec3 cTarget = glm::vec3(0);
		cameraNode = sMngr->createCameraSceneNode(cPos, cTarget);
		sMngr->setCurCamera(cameraNode);

		// create light
		glm::vec3 lPos = glm::vec3(0, 100, 100);
		glm::vec3 lDir = lPos;
		glm::vec3 lColor = glm::vec3(0);
		Light lt(lPos, lDir, lColor);
		lightNode = sMngr->createLightSceneNode(lt);
		sMngr->setCurLight(lightNode);

		// create floor / cube / skybox
		skyNode = sMngr->createSkyboxSceneNode(faces);
		sMngr->setCurSkybox(skyNode);
		createFloor();
		createBox();/**/

		// create mesh and anim
		Mesh* mesh = sMngr->loadMesh(meshFbxPath, Material::ePBR);
		charater = sMngr->createMeshSceneNode(mesh);
		for (size_t i = 0; i < animPaths.size(); i++)
			anims.push_back(sMngr->createAnimator(animPaths[i], Animation::eSkin));
		charater->onPlayLoop(anims[curAnimID]);

		// box / cat move
		catScaleModel = glm::scale(catScaleModel, glm::vec3(0.1));
		boxModel = glm::scale(boxModel, glm::vec3(2, 2, 10));
		boxModel = glm::translate(boxModel, glm::vec3(5, 2, 0));
		boxAABB = AABB(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		initcatAABB = AABB(glm::vec3(-40.801, 0.011889, -33.5359), glm::vec3(40.801, 112.858, 25.6071));
		movement = Movement(glm::vec3(0, 0, 1.f), catScaleModel, 5);
		boxAABB.update(boxModel);

		charater->setModel(catScaleModel);
		box->setModel(boxModel);
	}

	void run()
	{
		while (gRdr->isRun())
		{
			gRdr->beginScene();
			sMngr->render();
			gRdr->endScene();
		}
	}
	
	void onEvent(Event& e)
	{
		if(e.etypes[Event::eKeyInput])
			onKeyInput(e.ke);
		if (e.etypes[Event::eMouseMove])
			onMouseMove(e.mme);
		if (e.etypes[Event::eMouseScroll])
			onMouseScroll(e.mse);
	
		glm::mat4 catModel = movement.model;
		charater->setModel(movement.model);
		box->setModel(boxModel);
	}

private:
	void createBox()
	{
		Cube cube(cube_with_uv_nom_vertices, true, true);
		std::string dp = BOX_TEX_PATH;
		std::string np = BOX_NM_TEX_PATH;
		cube.m.mtype = Material::eClassic;
		cube.m.Diffuse = Texture2D(dp);
		cube.m.NormalMap = Texture2D(np);
		box = sMngr->createCubeSceneNode(cube);
	}
	
	void createFloor()
	{
		const int len = 10;
		std::vector<float> vertice{
			// pos        // norm  //uv  //tangent     
			-len, 0, len, 0, 0, 0, 1, 0, 1, 0, 0,
			 len, 0, len, 1, 0, 0, 1, 0, 1, 0, 0,
			len, 0, -len, 1, 1, 0, 1, 0, 1, 0, 0,
			-len, 0, -len, 0, 1, 0, 1, 0, 1, 0, 0
		};
		std::vector<int> indices{
			3, 2, 1,
			3, 1, 0
		};
		Cube cube(vertice, indices, true, true);
		std::string dp = FLOOR_TEX_PATH;
		std::string np = FLOOR_NM_TEX_PATH;
		cube.m.mtype = Material::eClassic;
		cube.m.Diffuse = Texture2D(dp);
		cube.m.NormalMap = Texture2D(np);
		floor = sMngr->createCubeSceneNode(cube);

	}

	void onKeyInput(Event::KeyEvent& e)
	{
		// AWSD
		if (e.Keys[EKEY_CODE::KEY_A])
			cameraNode->getCamera().changeDirection(Camera::eLeft);
		if (e.Keys[EKEY_CODE::KEY_D])
			cameraNode->getCamera().changeDirection(Camera::eRight);
		if (e.Keys[EKEY_CODE::KEY_W])
			cameraNode->getCamera().changeDirection(Camera::eFront);
		if (e.Keys[EKEY_CODE::KEY_S])
			cameraNode->getCamera().changeDirection(Camera::eBack);
		
		// tab
		if (e.Keys[EKEY_CODE::KEY_TAB])
			charater->onTransition(anims[curAnimID < animPaths.size() - 1 ? ++curAnimID : 0], 10);

		// L/R/U/D
		Movement newMove = movement;
		if (e.Keys[EKEY_CODE::KEY_UP])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goFront(), movement, newMove);
		if (e.Keys[EKEY_CODE::KEY_DOWN])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goBack(), movement, newMove);
		if (e.Keys[EKEY_CODE::KEY_RIGHT])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goRight(), movement, newMove);
		if (e.Keys[EKEY_CODE::KEY_LEFT])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goLeft(), movement, newMove);
		if (e.Keys[EKEY_CODE::KEY_I])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goUp(), movement, newMove);
		if (e.Keys[EKEY_CODE::KEY_K])
			updateIfnInsect(initcatAABB, boxAABB, newMove.goDown(), movement, newMove);
		

	}

	void onMouseMove(Event::MouseMoveEvent& e)
	{
		double xpos = e.x; 
		double ypos = e.y;
		static bool firstMouse = true;
		static double lxpos = window_width / 2;
		static double lypos = window_height / 2;
		if (firstMouse) // 这个bool变量初始时是设定为true的
		{
			lxpos = xpos;
			lypos = ypos;
			firstMouse = false;
		}

		double x_ = xpos - lxpos;
		double y_ = lypos - ypos;

		lxpos = xpos;
		lypos = ypos;

		cameraNode->getCamera().rotateViewDir(x_, y_);
	}
	
	float fov = MAX_FOV;

	void onMouseScroll(Event::MouseScrollEvent& e)
	{
		double xoffset = e.xoffset;
		double yoffset = e.yoffset;

		if (fov >= 1.0f && fov <= MAX_FOV)
			fov -= yoffset;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= MAX_FOV)
			fov = MAX_FOV;
		cameraNode->getCamera().updateFov(fov);
	}
};

// ------------------------------------------------------------------------------- //

/*
	程序入口
*/

int main(int argc, char** argv)
{
	Application app;
	app.run();
}

