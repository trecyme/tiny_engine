#include "context.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "scene_manager.h"
#include <unordered_map>

// ------------------------------------- GLOBAL VARIABLES ----------------------------------- //

double xp = 0;
double yp = 0;
double xo = 0;
double yo = 0;

// ------------------------------------- CLASS : RenderContext ----------------------------------- //

RenderContext::RenderContext(RenderContext::RenderType rtype, int width, int height, const char* name)
{

	switch (rtype)
	{
	case RenderContext::eOpenGL:
		mRender = (GraphicsRender *)new OpenGLGraphicsRender(width, height, name);
		mSceneManager = new SceneManager(mRender);
		break;

	default:
		break;
	}
}

RenderContext::~RenderContext()
{
	delete mSceneManager;
	delete mRender;
}

// -----------------------------------OpenGLGraphicsRender------------------------------------- //

OpenGLGraphicsRender::OpenGLGraphicsRender(int width, int height, const char* name)
{

	// 初始化GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW! \n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VMAJ);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VMIN);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);;
	if (!window)
	{
		printf("ERROR: Failed to create GLFW window!\n");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	this->width = width;
	this->height = height;


	bool glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW! \n");
		exit(-1);
	}
	const GLubyte* ver = glGetString(GL_VERSION);
	const GLubyte* ren = glGetString(GL_RENDERER);
	glGetError();					// 消除glewInit产生的错误(GLEW的BUG)
	glCheckError();

	// init shader， 必须在GLEW后进行
	skyShadar = new OpenGLShadarProgram(
		read_string_of_file(SKY_VS_PATH).data(),
		read_string_of_file(SKY_FS_PATH).data());
	dynamicRenderShadar = new OpenGLShadarProgram(
		read_string_of_file("glsl/dy_bp_norm_shadow_vs.glsl").data(),
		read_string_of_file("glsl/pbr_norm_shadow_fs.glsl").data());
	staticRenderShadar = new OpenGLShadarProgram(
		read_string_of_file("glsl/st_bp_norm_shadow_vs.glsl").data(),
		read_string_of_file("glsl/bp_norm_shadow_fs.glsl").data());
	dynamicPbrRenderShadar = new OpenGLShadarProgram(
		read_string_of_file("glsl/dy_bp_norm_shadow_vs.glsl").data(),
		read_string_of_file("glsl/pbr_norm_shadow_fs.glsl").data());
	dynamicShadowShadar = new OpenGLShadarProgram(
		read_string_of_file("glsl/dy_shadow_map_vs.glsl").data(),
		read_string_of_file("glsl/shadow_map_fs.glsl").data());
	staticShadowShadar = new OpenGLShadarProgram(
		read_string_of_file("glsl/st_shadow_map_vs.glsl").data(),
		read_string_of_file("glsl/shadow_map_fs.glsl").data());
	glEnable(GL_DEPTH_TEST);
}

OpenGLGraphicsRender::~OpenGLGraphicsRender()
{
	if (window)
		glfwDestroyWindow(window);

	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);

	for (size_t i = 0; i < primitives.size(); i++)
	{
		glDeleteVertexArrays(1, &(primitives[i]->vao));
		for (size_t j = 0; j < primitives[i]->texes.size(); j++)
			glDeleteTextures(1, &(primitives[i]->texes[j]));
		delete primitives[i];

	}
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
	glDeleteTextures(1, &depthCubeMap);

	delete skyShadar;
	delete dynamicRenderShadar;
	delete dynamicShadowShadar;
	delete staticRenderShadar;
	delete staticShadowShadar;
	delete dynamicPbrRenderShadar;

	glfwTerminate();
}


void OpenGLGraphicsRender::setEventListener(EventListener* el)
{
	this->el = el;
}

void OpenGLGraphicsRender::beginScene()
{
	Event e;
	// key input
	int kCount = 0;
	for (size_t i = 0; window && i < GLFW_KEY_MENU; i++)
	{
		if (glfwGetKey(window, i) == GLFW_PRESS)
			e.ke.Keys[i] = 1;
		else
			e.ke.Keys[i] = 0;
		kCount++;
	}
	if (kCount)
		e.etypes[Event::eKeyInput] = true;
	else
		e.etypes[Event::eKeyInput] = false;

	// mouse move
	if (xp != xpos || yp != ypos)
	{
		e.mme.x = xpos;
		e.mme.y = ypos;
		xpos = xp;
		ypos = yp;
		e.etypes[Event::eMouseMove] = true;
	}
	else
		e.etypes[Event::eMouseMove] = false;

	// mouse scroll
	if (xo != xoff || yo != yoff)
	{
		e.mse.xoffset = xo;
		e.mse.yoffset = yo;
		xoff = xo;
		yoff = yo;
		e.etypes[Event::eMouseScroll] = true;
	}
	else
		e.etypes[Event::eMouseScroll] = false;
	
	if (el)
		el->onEvent(e);
}

void OpenGLGraphicsRender::endScene()
{
	if (window)
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

bool OpenGLGraphicsRender::isRun()
{
	bool result = !glfwWindowShouldClose(window);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		result = false;
	}
	return result;
}

GLuint createTex(Texture2D& tex)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tex.filename.data(), &width, &height, &nrChannels, 0);
	GLenum glChnl = GL_RGB;
	switch (nrChannels)
	{
	case 1:				glChnl = GL_RED;  break;
	case 3:								  break;
	case 4:				glChnl = GL_RGBA; break;
	default:							  break;
	}

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, glChnl, width, height, 0, glChnl, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);/**/
		stbi_image_free(data);
	}
	else
	{
		std::cout << "ERROR: Failed to load texture - " << tex.filename.data() << std::endl;
		printf("%s\n", stbi_failure_reason());
	}

	// 为当前绑定的纹理对象设置过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	return texID;
}

void OpenGLGraphicsRender::createMaterial(Material* m, Primitives *p)
{
	if (m->mtype == Material::eClassic)
	{
		if (m->isValid(m->Diffuse))
			p->texes.push_back(createTex(m->Diffuse));
		if (m->isValid(m->NormalMap))
			p->texes.push_back(createTex(m->NormalMap));
	}
	else if (m->mtype == Material::ePBR)
	{
		if (m->isValid(m->Albedo))
			p->texes.push_back(createTex(m->Albedo));
		if (m->isValid(m->NormalMap))
			p->texes.push_back(createTex(m->NormalMap));
		if (m->isValid(m->AO))
			p->texes.push_back(createTex(m->AO));
		if (m->isValid(m->Roughness))
			p->texes.push_back(createTex(m->Roughness));

	}
}

ShadarProgram* OpenGLGraphicsRender::getShader(Light::EType ltype, Material::MaterialType mtype, bool isAnimated, bool isShadow)
{
	if (ltype == Light::eParallel && isAnimated && isShadow)
		return (ShadarProgram*)dynamicShadowShadar;
	else if (ltype == Light::eParallel && !isAnimated && isShadow)
		return (ShadarProgram*)staticShadowShadar;
	else if (ltype == Light::eParallel && mtype == Material::ePBR && isAnimated && !isShadow)
		return (ShadarProgram*)dynamicPbrRenderShadar;
	else if (ltype == Light::eParallel && mtype == Material::eClassic && !isAnimated && !isShadow)
		return (ShadarProgram*)staticRenderShadar;
	else
		return 0;
	// FUTURE more else if()
}

ShadarProgram* OpenGLGraphicsRender::getSkyboxShader()
{
	return (ShadarProgram*)skyShadar;
}

void OpenGLGraphicsRender::drawSkybox(SkyBox* skybox)
{
	if (!window)
		return;

	glDepthFunc(GL_LEQUAL);
	std::map<void*, Primitives*>::iterator it = ptrMapId.find((void*)skybox);
	if (it == ptrMapId.end())
	{
		Primitives* primt = new Primitives();
		primitives.push_back(primt);
		ptrMapId[(void*)skybox] = primt;

		glGenVertexArrays(1, &primt->vao);
		glBindVertexArray(primt->vao);
		GLuint vboID = 0;
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, skybox->vertices.size() * sizeof(float), skybox->vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		GLuint texID = this->loadCubemap(skybox->tex.filename);
		primt->texes.push_back(texID);
		it = ptrMapId.find((void*)skybox);
	}

	glBindVertexArray(it->second->vao);
	glCheckError();

	glActiveTexture(GL_TEXTURE0);
	glCheckError();

	glBindTexture(GL_TEXTURE_CUBE_MAP, it->second->texes[0]);
	glCheckError();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glCheckError();
	glDepthFunc(GL_LESS);
}

void OpenGLGraphicsRender::drawMesh(SubMesh* mesh)
{
	std::map<void*, Primitives*>::iterator it = ptrMapId.find((void*)mesh);
	if (it == ptrMapId.end())
	{
		Primitives* primt = new Primitives();
		primitives.push_back(primt);
		ptrMapId[(void*)mesh] = primt;

		glGenVertexArrays(1, &(primt->vao));
		glBindVertexArray(primt->vao);
		GLuint vboID = 0;
		GLuint eboID = 0;
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex),
			mesh->vertices.data(), GL_STATIC_DRAW);
		glGenBuffers(1, &eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(int), 
			mesh->indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		
		createMaterial(&mesh->material, primt);
		primt->n = mesh->indices.size();
		primt->hasEbo = true;
		it = ptrMapId.find((void*)mesh);
	}

	drawPrimitives(it->second);
	glCheckError();
}

void OpenGLGraphicsRender::drawCube(Cube* cube)
{
	std::map<void*, Primitives*>::iterator it = ptrMapId.find((void*)cube);
	if (it == ptrMapId.end())
	{
		Primitives* primt = new Primitives();
		primitives.push_back(primt);
		ptrMapId[(void*)cube] = primt;

		glGenVertexArrays(1, &(primt->vao));
		glBindVertexArray(primt->vao);
		GLuint vboID = 0;
		GLuint eboID = 0;
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, cube->vertices.size() * sizeof(float),
			cube->vertices.data(), GL_STATIC_DRAW);
		int stride = 11;
		if (!cube->hasTangent) stride -= 3;
		if (!cube->hasNormal) stride -= 3;
		if (cube->indices.size() > 0)
		{
			glGenBuffers(1, &eboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube->indices.size() * sizeof(int),
				cube->indices.data(), GL_STATIC_DRAW);
			primt->hasEbo = true;
			primt->n = cube->indices.size();

		}
		else {
			primt->hasEbo = false;
			primt->n = cube->vertices.size() / stride;

		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
		if (cube->hasNormal) glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(5 * sizeof(float)));
		if (cube->hasTangent) glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		if (cube->hasNormal)glEnableVertexAttribArray(2);
		if (cube->hasNormal)glEnableVertexAttribArray(3);
		
		createMaterial(&cube->m, primt);
		it = ptrMapId.find((void*)cube);
	}
	drawPrimitives(it->second);
	glCheckError();
}

void OpenGLGraphicsRender::drawPrimitives(Primitives* primt)
{
	// bind vao and tex
	glBindVertexArray(primt->vao);
	for (size_t i = 0; i < primt->texes.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, primt->texes[i]);
	}

	// draw
	if (primt->hasEbo)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, primt->n, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, primt->n);
	 
	
}

GLuint OpenGLGraphicsRender::loadCubemap(std::string* faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void OpenGLGraphicsRender::createDepthMapFrameBuffer()
{
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);/**/
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);                       // 设置不绘制颜色缓冲
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer is OK!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

}

void OpenGLGraphicsRender::createOmnDepthMapFrameBuffer()
{
	// 创建点光源深度贴图的立方体纹理
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// 创建帧缓冲
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void OpenGLGraphicsRender::bindAndActiveDepTex(int tunit, bool isParallel)
{
	glActiveTexture(GL_TEXTURE0 + tunit);
	if (isParallel)
	{
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}
	else
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	}
}

//------------------------- FUNC ---------------------------//


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	xp = xpos;
	yp = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	xo = xoffset;
	yo = yoffset;
}

GLenum OpenGLGraphicsRender::glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

