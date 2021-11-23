#include "fbx_parser.h"
#include "meshvao.h"
#include "fbx_util.h"
#include "context.h"
#include <unordered_map>



#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

/*
	初始化FBX SDK的FbxManager对象， 并创建FbxScene对象
*/
void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
	
}

/*
	从文件中导入FbxScene对象
*/
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);


	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (int i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);
	if (lStatus == true)
	{
		// Check the scene integrity!
		FbxStatus status;
		FbxArray< FbxString*> details;
		FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(pScene), &status, &details);
		lStatus = sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData);
		if (lStatus == false)
		{
			if (details.GetCount())
			{
				FBXSDK_printf("Scene integrity verification failed with the following errors:\n");
				for (int i = 0; i < details.GetCount(); i++)
					FBXSDK_printf("   %s\n", details[i]->Buffer());

				FbxArrayDelete<FbxString*>(details);
			}
		}
	}

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

bool loadFbxFileScene(FbxManager* fmanage, FbxScene* scene, const char* fbxpath)
{
	bool lResult;

	FbxString lFilePath(fbxpath);
	if (lFilePath.IsEmpty())
	{
		lResult = false;
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}
	else
	{
		FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
		lResult = LoadScene(fmanage, scene, lFilePath.Buffer());
	}

	if (!lResult)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	return lResult;
}


// ----------------------------------- CLASS - FbxLoader --------------------------------------//


FbxLoader::~FbxLoader()
{
	if (fmanager) fmanager->Destroy();
}

int FbxLoader::nScene = 0;

bool FbxLoader::loadSceneFile(FbxScene*& scene, const char* filePath)
{
	if (filePath)
	{
		InitializeSdkObjects(fmanager, scene);
		return loadFbxFileScene(fmanager, scene, filePath);
	}
	return false;
}

int FbxLoader::getAttrIndex(FbxLayerElement::EMappingMode mmode, FbxLayerElement::EReferenceMode rmode,
	FbxLayerElementArrayTemplate< int >& indexArray, int cIdx, int vIdx)
{
	/**
		FBX中法线/UV等顶点属性因为映射方式不同，所以在数组中的ID也不同。
		需要根据映射模式，获取它实际在数组中的IDX。
	*/
	switch (mmode)
	{
	case FbxGeometryElement::eByControlPoint:
		switch (rmode)
		{
		case FbxGeometryElement::eDirect:					return cIdx;
		case FbxGeometryElement::eIndexToDirect:			return indexArray.GetAt(cIdx);
		default:											throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (rmode)
		{
		case FbxGeometryElement::eDirect:					return vIdx;
		case FbxGeometryElement::eIndexToDirect:			return indexArray.GetAt(vIdx);
		default:											throw std::exception("Invalid Reference");
		}
		break;
	}
}

FbxAMatrix FbxLoader::GetGeometry(FbxNode* pNode)
{
	/**
		获取Node的geometry transform
	*/

	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	return FbxAMatrix(lT, lR, lS);

}

void FbxLoader::readVertices(FbxMesh* fmesh, std::vector<Vertex>& vertices)
{
	int nPolygonVertex = fmesh->GetPolygonVertexCount();
	int* cindices = fmesh->GetPolygonVertices();    // index of polygon vertex to index of control point
	FbxGeometryElementUV* uvlayer = fmesh->GetElementUV(0);
	FbxGeometryElementNormal* normallayer = fmesh->GetElementNormal(0);
	FbxGeometryElementTangent* tangentlayer = fmesh->GetElementTangent(0);

	for (int vIdx = 0; vIdx < nPolygonVertex; vIdx++)
	{
		// add new vertex
		vertices.push_back(Vertex());

		// POS
		FbxVector4 pos = fmesh->GetControlPointAt(cindices[vIdx]);
		vertices[vIdx].pos = glm::vec3(pos[0], pos[1], pos[2]);
	
		// UV
		FbxVector2 uv = uvlayer->GetDirectArray().GetAt(getAttrIndex(uvlayer->GetMappingMode(),
			uvlayer->GetReferenceMode(), uvlayer->GetIndexArray(), cindices[vIdx], vIdx));
		vertices[vIdx].uv = glm::vec2(uv[0], uv[1]);

		// NORMAL
		FbxVector4 normal = normallayer->GetDirectArray().GetAt(getAttrIndex(normallayer->GetMappingMode(),
			normallayer->GetReferenceMode(), normallayer->GetIndexArray(), cindices[vIdx], vIdx));
		vertices[vIdx].normal = glm::vec3(normal[0], normal[1], normal[2]);

		// TAGENT
		FbxVector4 tangent = tangentlayer->GetDirectArray().GetAt(getAttrIndex(tangentlayer->GetMappingMode(),
			tangentlayer->GetReferenceMode(), tangentlayer->GetIndexArray(), cindices[vIdx], vIdx));
		vertices[vIdx].tangent = glm::vec3(tangent[0], tangent[1], tangent[2]);
	}
}

void FbxLoader::removeDuplicateVertex(std::vector<Vertex>& oldVs, std::vector<Vertex>& newVs, 
	std::vector<int>& vIndices)
{
	using Vmap=std::unordered_map<Vertex, int, VertexHashFunc, VertexEqualKey>;
	int nowIdx = 0;
	Vmap vertexMapNewidx;
	
	for (int vIdx = 0; vIdx < oldVs.size(); vIdx++)
	{
		Vmap::iterator pos_it = vertexMapNewidx.find(oldVs[vIdx]);
		if (pos_it == vertexMapNewidx.end())
		{
			// jump to next index; put it to map; add it to newVs
			Vertex& vertex = oldVs[vIdx];
			newVs.push_back(vertex);
			vIndices.push_back(nowIdx);
			vertexMapNewidx[vertex] = nowIdx++;
		}
		else
		{
			// add to vIndices
			vIndices.push_back(pos_it->second);
		}
	}
}

void FbxLoader::parseMaterial(FbxMesh* fmesh, SubMesh& subMesh, Material::MaterialType materialType)
{
	FbxNode* node = fmesh->GetNode();
	FbxSurfaceMaterial* sm = node->GetMaterial(0);
	std::string fixedFilename;
	if (sm->FindProperty(sm->sDiffuse).GetSrcObject<FbxFileTexture>())
	{
		FbxFileTexture* lTexture = sm->FindProperty(sm->sDiffuse).GetSrcObject<FbxFileTexture>();
		fixedFilename = lTexture->GetRelativeFileName();
		fixedFilename = fixTextureFileName(fixedFilename.data());
	}

	// 读取其他贴图
	if (fixTextureFileName)
	{
		const char* filename = fixedFilename.data();
		int suffixIdx = strlen(filename);
		for (; suffixIdx > 0; suffixIdx--) {
			if (filename[suffixIdx] == '.') break;
		}

		int ulineIdx = strlen(filename);
		for (; ulineIdx > 0; ulineIdx--) {
			if (filename[ulineIdx] == '_') break;
		}

		std::string suffix(filename + suffixIdx);
		std::string prefix(filename, filename + ulineIdx);

		std::string normMappingFilename = prefix + "_ddna" + suffix;
		if (std::string(filename).find("tangyuannew") != std::string::npos) { normMappingFilename = prefix + "_normal" + suffix; }
		std::string aoFilename = prefix + "_ao" + suffix;
		std::string roughFilename = prefix + "_rough" + suffix;

		subMesh.material.mtype = materialType;
		if (materialType == Material::ePBR)
		{
			subMesh.material.Albedo = Texture2D(fixedFilename);
			subMesh.material.NormalMap = Texture2D(normMappingFilename);
			subMesh.material.AO = Texture2D(aoFilename);
			subMesh.material.Roughness = Texture2D(roughFilename);
		}
		else if (materialType == Material::eClassic)
		{
			subMesh.material.Diffuse = Texture2D(fixedFilename);
			subMesh.material.NormalMap = Texture2D(normMappingFilename);
		}
	}

}

void FbxLoader::parseSkinDeformer(FbxMesh* fmesh, glm::mat4& preMtx,
	std::vector<glm::mat4>& postMtx,
	std::vector<Vertex>& vertices,
	std::vector<int>& indices,
	std::map<std::string, int>& boneNameMapIdx)
{

	if (!fmesh->GetDeformer(0) || fmesh->GetDeformer(0)->GetDeformerType() != FbxDeformer::eSkin)
		return;
	FbxSkin* pSkin = (FbxSkin*)fmesh->GetDeformer(0);
	FbxNode* fnode = fmesh->GetNode();

	// preMtx
	FbxAMatrix geotransM = GetGeometry(fnode);
	FbxAMatrix meshCurTransM = fnode->EvaluateGlobalTransform() * geotransM;
	meshCurTransM = meshCurTransM.Inverse();
	preMtx = FbxMatToGlm(fnode->EvaluateGlobalTransform() * meshCurTransM);

	// postMtx 
	postMtx = std::vector<glm::mat4>(boneNameMapIdx.size(), glm::mat4(1.f));
	int nConPoints = fmesh->GetControlPointsCount();
	float defaultBone = -1.f;
	std::vector<glm::vec4> conWeights(nConPoints, glm::vec4(0));

	std::vector<glm::vec4> conBones(nConPoints, glm::vec4(defaultBone));
	for (int cIdx = 0; cIdx < pSkin->GetClusterCount(); cIdx++)
	{
		FbxCluster* pCluster = pSkin->GetCluster(cIdx);
		const char* bname = pCluster->GetLink()->GetName();
		int bIdx = boneNameMapIdx[std::string(bname)];
		FbxAMatrix transM, transLinkM, gloI;
		pCluster->GetTransformMatrix(transM);
		pCluster->GetTransformLinkMatrix(transLinkM);
		gloI = transLinkM.Inverse() * transM * geotransM;
		postMtx[bIdx] = FbxMatToGlm(gloI);


		int* conPointIndices = pCluster->GetControlPointIndices();
		double* weights = pCluster->GetControlPointWeights();
		for (int i = 0; i < pCluster->GetControlPointIndicesCount(); i++)	
		{
			int conPointIdx = conPointIndices[i];
			int j = 0;
			for (; j < 4 && conBones[conPointIdx][j] != defaultBone; j++);
			conWeights[conPointIdx][j] = weights[i];
			conBones[conPointIdx][j] = bIdx;
		}
	}

	// check boneID and weight
	for (int i = 0; i < fmesh->GetControlPointsCount(); i++)
	{
		int j = 0;
		for (; j < 4 && conBones[i][j] != defaultBone; j++);
		for (; j < 4 && conBones[i][j] != defaultBone; j++)
		{
			conWeights[i][j] = 0;
			conBones[i][j] = 0;
		}
		float total = conWeights[i].x + conWeights[i].y + conWeights[i].z + conWeights[i].w;
		assert(total < 1.01f && total > 0.99);
	}

	int* cids = fmesh->GetPolygonVertices();
	for (int vIdx = 0; vIdx < fmesh->GetPolygonVertexCount(); vIdx++)
	{
		int cIdx = cids[vIdx];
		int nIdx = indices[vIdx];

		vertices[nIdx].weights = conWeights[cIdx];
		vertices[nIdx].boneIDs = conBones[cIdx];
	}

	printf("");
	std::cout << std::endl;
}

void FbxLoader::parseSkeleton(FbxScene* fscene, std::vector<std::string> boneNames, std::map<std::string, int>& boneNameMapIdx)
{
	for (int i = 0; i < fscene->GetSrcObjectCount<FbxSkeleton>(); i++)
	{
		std::string bname = fscene->GetSrcObject<FbxSkeleton>(i)->GetNode()->GetName();
		boneNames.push_back(bname);
		boneNameMapIdx[bname] = i;
	}

}

void FbxLoader::loadMesh(Mesh* mesh, const char* filePath, Material::MaterialType materialType)
{
	FbxScene* scene;
	if (!loadSceneFile(scene, filePath))
	{
		throw "exception occurred when loading mesh in file file.";
	}
	FbxGeometryConverter triangulator = FbxGeometryConverter(fmanager);
	triangulator.Triangulate(scene, true);

	//  parse Skeletons
	parseSkeleton(scene, mesh->boneNames, mesh->boneNameMapIdx);
	mesh->nBones = mesh->boneNames.size();

	//  parse Meshs
	mesh->mtype = materialType;
	mesh->submeshes = std::vector<SubMesh>(scene->GetSrcObjectCount<FbxMesh>()); // 先分配空间避免之后复制
	for (int i = 0; i < scene->GetSrcObjectCount<FbxMesh>(); i++)
	{
		FbxMesh* fmesh = scene->GetSrcObject<FbxMesh>(i);
		SubMesh* submesh = &mesh->submeshes[i];

		// 取出所有顶点属性；去重顶点，计算新索引
		std::vector<Vertex> vertices;
		readVertices(fmesh, vertices);
		removeDuplicateVertex(vertices, submesh->vertices, submesh->indices);
		if (submesh->indices.size() != fmesh->GetPolygonVertexCount())
			throw "wrong duplicated func";
		// 处理材质			TODO 只目前针对现有模型的情况，因为纹理的路径不对
		parseMaterial(fmesh, *submesh, materialType);
		// 处理骨骼相关的数据
		submesh->meshMtx = FbxMatToGlm(fmesh->GetNode()->EvaluateGlobalTransform());
		parseSkinDeformer(fmesh, submesh->bonePreMtx, submesh->bonePostMtx, submesh->vertices, submesh->indices, mesh->boneNameMapIdx);
		// TODO AABB
		/*AABB aabb(pMesh->GetControlPoints(), pMesh->GetControlPointsCount());
		std::cout << "aabb.max: "<< vec32str(aabb.max) << std::endl;
		std::cout << "aabb.min: "<< vec32str(aabb.min) << std::endl;*/
	}
}

void FbxLoader::loadAnim(Animation*& anim, const char* filePath)
{

	FbxScene* scene;
	if (!loadSceneFile(scene, filePath))
	{
		throw "exception occurred when loading mesh in file file.";
	}
	
	std::vector<FbxTime> frameTimes;
	std::vector<std::vector<glm::mat4>> glbMatrices;
	std::vector<std::vector<glm::mat4>> lclMatrices;
	FbxTime frameGap, startTime, stopTime;
	SkinnedAnimation* sAnim;
	FbxAnimStack* astack;
	int nFrames;
	int nBones;
	switch (anim->atype)
	{
	case Animation::eSkin:
		//  parse Skeletons
		sAnim = (SkinnedAnimation*)anim;
		astack = scene->GetCurrentAnimationStack();
		startTime = astack->LocalStart;
		stopTime = astack->LocalStop;
		nFrames = stopTime.GetFrameCount();
		nBones = scene->GetSrcObjectCount<FbxSkeleton>();
		frameGap = (stopTime - startTime) / nFrames;
		for (FbxTime curTime = startTime; curTime < stopTime; curTime += frameGap)
			frameTimes.push_back(curTime);

		for (int bIdx = 0; bIdx < nBones; bIdx++)
		{
			FbxNode* node = scene->GetSrcObject<FbxSkeleton>(bIdx)->GetNode();
			std::string name = node->GetName();
			sAnim->boneNameMapIdx.insert(std::pair<std::string, int>(name, bIdx));
			sAnim->boneNames.push_back(name);
			std::string parName(node->GetParent()->GetName());
			auto it = sAnim->boneNameMapIdx.find(parName);
			if (it != sAnim->boneNameMapIdx.end())
				sAnim->parent.push_back(sAnim->boneNameMapIdx[parName]);
			else
				sAnim->parent.push_back(-1); // root
		}
		
		for (int fIdx = 0; fIdx < nFrames; fIdx++)
		{
			FbxTime curTime = frameTimes[fIdx];
			std::vector<glm::mat4> gms;
			std::vector<glm::mat4> lms;
			for (int bIdx = 0; bIdx < nBones; bIdx++)
			{
				gms.push_back(FbxMatToGlm(scene->GetSrcObject<FbxSkeleton>(bIdx)->GetNode()->EvaluateGlobalTransform(curTime)));
				lms.push_back(FbxMatToGlm(scene->GetSrcObject<FbxSkeleton>(bIdx)->GetNode()->EvaluateLocalTransform(curTime)));
			}
			glbMatrices.push_back(gms);
			lclMatrices.push_back(lms);
		}
		sAnim->globalBoneTransforms = glbMatrices;
		sAnim->localBoneTransforms = lclMatrices;
		sAnim->nFrames = nFrames;
		sAnim->nBones = nBones;
		break;

	default:
		break;
	}
}
