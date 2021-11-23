#ifndef _CONFIG_H
#define _CONFIG_H

#define GL_VMAJ 4
#define GL_VMIN 2

#define SKY_FS_PATH "glsl/skybox_fs.glsl"
#define SKY_VS_PATH "glsl/skybox_vs.glsl"

#define MTX_MODEL_NAME "model"
#define MTX_VIEW_NAME "view"
#define MTX_PROJ_NAME "projection"
#define MTX_PRE_NAME "preMtx"
#define MTX_POST_NAME "postMtx"
#define MTX_BONE_NAME "boneAnim"

#define TEX_DIFFUSE_NAME "surfaceTex"
#define TEX_NORM_NAME "normTex"
#define TEX_DEPTH_NAME "depthTex"
#define TEX_PBR_ALBEDO_NAME "albedoMap"
#define TEX_PBR_NORM_NAME "normalMap"
#define TEX_PBR_AO_NAME "aoMap"
#define TEX_PBR_ROUGE_NAME "roughMap"

#ifdef _DEBUG
	//#define DEBUG_CAMERA
#endif



#endif // !#define _CONFIG_H
