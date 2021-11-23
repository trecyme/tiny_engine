#include "fbx_util.h"

using std::cout;
using std::endl;

void printFbxVector4(FbxDouble4 v4)
{
	
	cout << v4[0] << ","
		<< v4[1] << ","
		<< v4[2] << ","
		<< v4[3] << endl;
}

void printFbxAMatrix(FbxAMatrix pTrans)
{
	FbxDouble4x4 m = (FbxDouble4x4)pTrans;
	for (int k = 0; k < 4; k++)
	{
		for (int l = 0; l < 4; l++)
		{
			cout << m[k][l] << ",";
		}
		cout << endl;
	}
}
/*
	输出场景中NODE的信息
*/
void printFbxNodes(FbxScene *scene)
{
	int nNodes = scene->GetSrcObjectCount<FbxNode>();
	cout << "num of nodes in this scene: " << nNodes << endl;

	for (int i = 0; i < nNodes; i++)
	{
		FbxNode *node = scene->GetSrcObject<FbxNode>(i);
		cout << "name of node [" << i << "]: " << node->GetName() << endl;
	}
}


/*
	输出场景中MESH的信息
*/
void printFbxMeshes(FbxScene *pScene)
{

	int nMeshes = pScene->GetSrcObjectCount<FbxMesh>();
	cout << "num of meshes in this scene: " << nMeshes << endl;

	for (int i = 0; i < nMeshes; i++)
	{
		FbxMesh *mesh = pScene->GetSrcObject<FbxMesh>(i);

		// MESH相关NODE 信息
		int nNodes = mesh->GetNodeCount();
		cout << "num of nodes in mesh [" << i << "]: " << nNodes << endl;
		for (int j = 0; j < nNodes; j++)
		{
			FbxNode *node = mesh->GetNode(j);
			cout << "name of node[" << j << "] in mesh [" << i << "]: " << node->GetName() << endl;
			cout << "globla transform matrix of node[" << j << "] in mesh [" << i << "]: " << endl;
			FbxAMatrix& tramsM = node->EvaluateGlobalTransform();
			printFbxAMatrix(tramsM);
		}

		// MESH相关控制点信息
		int nPoints = mesh->GetControlPointsCount();
		cout << "num of control points in mesh [" << i << "]: " << nPoints << endl;
		/*for (int j = 0; j < nPoints; j++)
		{
			FbxVector4 point = mesh->GetControlPointAt(j);
			cout << point[0] << ","
				<< point[1] << ","
				<< point[2] << ","
				<< point[3] << endl;
		}*/


		// MESH相关三角形顶点信息
		int nPolygons = mesh->GetPolygonCount();
		cout << "num of polygons in mesh [" << i << "]: " << nPolygons << endl;
		/*for (int j = 0; j < nPolygons; j++)
		{
			int size = mesh->GetPolygonSize(j);
			assert(size == 3);
			for (int k = 0; k < 3; k++)
			{
				cout << mesh->GetPolygonVertex(j, k) << " ,";
			}
			cout << endl;
		}*/

		int *vindices = mesh->GetPolygonVertices();
		int imin = 0xFFFFF;
		int imax = 0;
		for (int j = 0; j < nPolygons * 3; j++)
		{
			if (vindices[j] > imax)
				imax = vindices[j];

			if (vindices[j] < imin)
				imin = vindices[j];
		}
		cout << "min index in mesh[" << i << "]: " << imin << endl;
		cout << "max index in mesh[" << i << "]: " << imax << endl;


		// MESH相关材质和纹理信息
		int nMaterial = mesh->GetElementMaterialCount();
		cout << "num of material layer in mesh [" << i << "]: " << nMaterial << endl;
		cout << "num of material in mesh [" << i << "]: " << mesh->GetNode()->GetMaterialCount() << endl;
		for (int j = 0; j < nMaterial; j++)
		{
			FbxGeometryElementMaterial* material = mesh->GetElementMaterial(j);
			int nSurfaces = material->GetIndexArray().GetCount();
			if (material->GetMappingMode() == FbxLayerElement::eAllSame)
			{
				cout << "all the same mode!" << endl;
			}
			else if (material->GetMappingMode() == FbxLayerElement::eByPolygon)
			{
				assert(nSurfaces == nPolygons);
			}

			for (int k = 0; k < nSurfaces; k++)
			{
				FbxSurfaceMaterial* sm = mesh->GetNode()->GetMaterial(material->GetIndexArray().GetAt(k));

				if (sm->GetClassId().Is(FbxSurfacePhong::ClassId))
				{
					FbxSurfacePhong *sp = (FbxSurfacePhong*)(sm);
					/*FbxFileTexture* lTexture;

					cout << "name of surface[" << k << "] material[" << j << "] in mesh [" << i
						<< "]: " << lTexture->GetFileName() << endl;*/
				}
				else if (sm->GetClassId().Is(FbxSurfaceLambert::ClassId))
				{
					FbxSurfaceLambert *lambertM = (FbxSurfaceLambert *)(sm);
				}

			}
		}
		cout << "------------------------------------------" << endl;


	}
}

/*
	输出场景中姿势的信息
*/
void printFbxPoses(FbxScene *pScene)
{
	int nPoses = pScene->GetSrcObjectCount<FbxPose>();
	cout << "num of poses in this scene: " << nPoses << endl;
	for (int i = 0; i < nPoses; i++)
	{
		FbxPose *pose = pScene->GetSrcObject<FbxPose>(i);
		cout << "Is pose[" << i << "] bind pose?: " << pose->IsBindPose() << endl;

		int nLinks = pose->GetCount();
		for (int j = 0; j < nLinks; j++)
		{
			cout << "node[" << j << "] of pose[" << i << "]: " << pose->GetNode(j)->GetName() << endl;
			cout << "Is local matrix? :" << pose->IsLocalMatrix(j) << endl;
			printFbxMatrix(pose->GetMatrix(j));
			cout << endl;
			printFbxMatrix(FbxMatrix(pose->GetNode(j)->EvaluateGlobalTransform()));
			cout <<  endl;

		}
	}
}


/*
	输出场景中骨骼的信息
*/
void printFbxSkeletons(FbxScene *pScene)
{
	int nSkeletons = pScene->GetSrcObjectCount<FbxSkeleton>();
	cout << "num of skeletons in this scene: " << nSkeletons << endl;
	for (int i = 0; i < nSkeletons; i++)
	{
		FbxSkeleton* lSkeleton = pScene->GetSrcObject<FbxSkeleton>(i);
		cout << "type of bone[ " << i << "] " << lSkeleton->GetSkeletonType() << endl;
		cout << "name of bone[ " << i << "] " << lSkeleton->GetNode()->GetName() << endl;
		FbxAnimStack *stack = pScene->GetCurrentAnimationStack();
		FbxAnimLayer *layer = pScene->GetCurrentAnimationStack()->GetMember<FbxAnimLayer>(0);
		
		/**/
		cout << "------------local trans------------" << endl;
		FbxAMatrix ltrans = lSkeleton->GetNode()->EvaluateLocalTransform();
		printFbxAMatrix(ltrans);
		cout << "------------global trans------------" << endl;
		FbxAMatrix gtrans = lSkeleton->GetNode()->EvaluateGlobalTransform();
		printFbxAMatrix(gtrans);
	}
}

/*
	输出场景中动画的信息
*/
void printAnimationInfo(FbxScene *pScene)
{
	int nStacks = pScene->GetSrcObjectCount<FbxAnimStack>();
	cout << "num of anim stacks in this scene: " << nStacks << endl;
	for (int i = 0; i < nStacks; i++)
	{
		FbxAnimStack *stack = pScene->GetSrcObject<FbxAnimStack>(i);
		int nLayers = stack->GetMemberCount();
		cout << "num of anim layers in this stack[" << i << "]: " << nLayers << endl;
		for (int j = 0; j < nLayers; j++)
		{
			FbxAnimLayer *layer = stack->GetMember<FbxAnimLayer>(j);
			int nCurveNode = layer->GetMemberCount<FbxAnimCurveNode>();
			cout << "num of curve nodes in anim layers [" << j << "] in this stack[" << i << "]: " << nCurveNode << endl;
			for (int k = 0; k < nCurveNode; k++)
			{
				FbxAnimCurveNode *cnode = layer->GetMember<FbxAnimCurveNode>(j);
				cout << "num of channels in curve node[" << k << "] in anim layers [" << j << "] in this stack[" << i << "]: " << cnode->GetChannelsCount() << endl;
			}
		}
	}
}


void printSQTVectors(FbxAMatrix pTrans)
{
	cout << "S: " << endl;
	printFbxVector4(pTrans.GetS());
	cout << "Q: " << endl;
	printFbxVector4(pTrans.GetQ());
	cout << "T: " << endl;
	printFbxVector4(pTrans.GetT());

}

void printFbxMatrix(FbxMatrix mtx)
{
	FbxDouble4x4 data = (FbxDouble4x4)mtx;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << data[i][j] << ", ";
		}
		cout << endl;
	}
}


/*
	fbxsdk::FbxAMatrix转成glm::mat4的数据结构
*/
glm::mat4 FbxMatToGlm(const FbxAMatrix& mat) {

	// FbxAMatrix代表仿射矩阵，按列组织
	glm::mat4 res;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			res[i][j] = mat[i][j];
		}
	}
	return res;
}

FbxAMatrix GlmMatToFbx(const glm::mat4& mat)
{
	FbxAMatrix res;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			res[i][j] = mat[i][j];
		}
	}
	return res;
}


/*
	fbxsdk::FbxVector4转成glm::vec4的数据结构
*/
glm::vec4 FbxVec4ToGlm(const FbxVector4& v) {

	glm::dvec4 c0 = glm::make_vec4((double*)v.Buffer());
	return glm::vec4(c0.x, c0.y, c0.z, c0.w);
}