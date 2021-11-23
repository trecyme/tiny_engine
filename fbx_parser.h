#ifndef _FBX_PARSER_H
#define _FBX_PARSER_H


#include "common.h"
#include "meshvao.h"
#include "animation.h"

// ---------------------------------- CLASS & STRUCT DEF --------------------------------- //



// ---------------------------------- CLASS - ModelLoader --------------------------------- //

class ModelLoader
{
	
public:
	virtual void loadMesh(Mesh* mesh, const char* filePath, Material::MaterialType materialType) = 0;
	virtual void loadAnim(Animation*& anim, const char* filePath) = 0;
};


// ---------------------------------- CLASS - FbxLoader --------------------------------- //
class FbxLoader: public ModelLoader
{
	

	FbxManager* fmanager;
	//FbxGeometryConverter triangulator;
	
	bool loadSceneFile(FbxScene*& scene, const char* filePath);
	int getAttrIndex(FbxLayerElement::EMappingMode mmode, FbxLayerElement::EReferenceMode rmode,
		FbxLayerElementArrayTemplate< int >& indexArray, int cIdx, int vIdx);
	FbxAMatrix GetGeometry(FbxNode* fnode);
	void readVertices(FbxMesh *fmesh, std::vector<Vertex>& vertices);
	void removeDuplicateVertex(std::vector<Vertex>& oldVs, std::vector<Vertex>& newVs, std::vector<int>& newIndices);
	void parseSkeleton(FbxScene *fscene, std::vector<std::string> boneNames, std::map<std::string, int>& boneNameMapIdx);
	void parseSkinDeformer(FbxMesh* fmesh, glm::mat4& preMtx,
		std::vector<glm::mat4>& postMtx,
		std::vector<Vertex>& vertices,
		std::vector<int>& indices,
		std::map<std::string, int>& boneNameMapIdx);
	void parseMaterial(FbxMesh* fmesh, SubMesh& subMesh, Material::MaterialType materialType);

public:
	static int nScene;

	FbxLoader() = default;
	~FbxLoader();
	void loadMesh(Mesh* mesh, const char* filePath, Material::MaterialType materialType);
	void loadAnim(Animation*& anim, const char* filePath);
};





#endif // !#define _FBX_PARSER_H
