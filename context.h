#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "common.h"
#include "config.h"
#include "shader_program.h"
#include "util.h"
#include "light.h"
#include "material.h"

class SceneManager;
struct SkyBox;
class Cube;
struct SubMesh;

enum EKEY_CODE
{
	/*KEY_A,
	KEY_W,
	KEY_S,
	KEY_D,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_TAB,*/

	KEY_I = GLFW_KEY_I,
	KEY_K = GLFW_KEY_K,
	KEY_A = GLFW_KEY_A,
	KEY_W = GLFW_KEY_W,
	KEY_S = GLFW_KEY_S,
	KEY_D = GLFW_KEY_D,
	KEY_UP = GLFW_KEY_UP,
	KEY_DOWN = GLFW_KEY_DOWN,
	KEY_LEFT = GLFW_KEY_LEFT,
	KEY_RIGHT = GLFW_KEY_RIGHT,
	KEY_TAB = GLFW_KEY_TAB,

	// FUTURE more key...

};

struct Event 
{
	enum EventType {
		eKeyInput = 0,
		eMouseMove = 1,
		eMouseScroll = 2
	};
	
	struct MouseMoveEvent
	{
		double x;
		double y;
	};

	struct MouseScrollEvent
	{
		double xoffset;
		double yoffset;
	};

	struct KeyEvent
	{
		int Keys[GLFW_KEY_MENU];
	};

	MouseMoveEvent mme;
	MouseScrollEvent mse;
	KeyEvent ke;
	bool etypes[3];

};

class EventListener
{
public:
	virtual void onEvent(Event& e) = 0;
};


class GraphicsRender
{
public:
	virtual void beginScene() = 0;
	virtual void endScene() = 0;
	virtual bool isRun() = 0;
	virtual void drawSkybox(SkyBox* skybox) = 0;
	virtual void drawCube(Cube* cube) = 0;
	virtual void drawMesh(SubMesh* smesh) = 0;
	virtual void setEventListener(EventListener* el) = 0;
	virtual ShadarProgram* getShader(Light::EType ltype, Material::MaterialType mtype, bool isAnimated, bool isShadow) = 0;
	virtual ShadarProgram* getSkyboxShader() = 0;
	virtual void createDepthMapFrameBuffer() = 0;
	virtual void createOmnDepthMapFrameBuffer() = 0;
	virtual void renderPre() = 0;
	virtual void renderShadowPre() = 0;
	virtual void renderShadowPost() = 0;
	virtual bool hasFrameBuffer() = 0;
	virtual void bindAndActiveDepTex(int tunit, bool isParallel) = 0;

};

class OpenGLGraphicsRender : GraphicsRender
{
	struct Primitives
	{
		GLuint vao;
		bool hasEbo;
		std::vector<GLuint> texes;
		int n;
	};

	EventListener* el = 0;
	GLFWwindow* window = 0;
	int width;
	int height;
	double xpos = 0;
	double ypos = 0;
	double xoff = 0;
	double yoff = 0;

	std::vector<Primitives*> primitives;
	std::map<void*, Primitives*> ptrMapId;
	
	GLuint depthMapFBO;
	GLuint depthCubeMap;
	GLuint depthMap;
	uint shadowWidth = 1024;
	uint shadowHeight = 1024;

	OpenGLShadarProgram* skyShadar;
	OpenGLShadarProgram* dynamicRenderShadar;
	OpenGLShadarProgram* staticRenderShadar;
	OpenGLShadarProgram* dynamicPbrRenderShadar;
	OpenGLShadarProgram* dynamicShadowShadar;
	OpenGLShadarProgram* staticShadowShadar;

public:
	OpenGLGraphicsRender(int width, int height, const char* name);
	~OpenGLGraphicsRender();

	virtual void setEventListener(EventListener *el);
	virtual void beginScene(); 
	virtual void endScene();
	virtual bool isRun();

	virtual ShadarProgram* getShader(Light::EType ltype, Material::MaterialType mtype, bool isAnimated, bool isShadow);
	virtual ShadarProgram* getSkyboxShader();
	virtual void drawSkybox(SkyBox* skybox);
	virtual void drawMesh(SubMesh* mesh);
	virtual void drawCube(Cube* mesh);
	virtual void createDepthMapFrameBuffer();
	virtual void createOmnDepthMapFrameBuffer();
	virtual void bindAndActiveDepTex(int tunit, bool isParallel);
	virtual void renderPre()
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	virtual void renderShadowPre()
	{
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
	}
	virtual void renderShadowPost()
	{
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	virtual bool hasFrameBuffer()
	{
		return depthMapFBO != 0;
	}

private:
	GLenum glCheckError_(const char* file, int line);
	GLuint loadCubemap(std::string *faces);
	void createMaterial(Material* m, Primitives* p);
	void drawPrimitives(Primitives* primt);


};

#define glCheckError() glCheckError_(__FILE__, __LINE__)
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class RenderContext
{

public:
	enum RenderType
	{
		eOpenGL,

		// FUTURE others Graphics API ...
	};

	RenderContext(RenderType rtype, int width, int height, const char* name);
	RenderContext(const RenderContext&) = delete;
	RenderContext& operator=(const RenderContext&) = delete;
	~RenderContext();

	SceneManager* getSceneManager() { return mSceneManager; }
	GraphicsRender* getGraphicsRender() { return mRender; }

private:
	SceneManager* mSceneManager;
	GraphicsRender* mRender;

};


#endif // !_CONTEXT_H



