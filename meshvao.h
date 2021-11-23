#ifndef MESHVAO_H
#define MESHVAO_H

#include "common.h"
#include "collision.h"
#include "meshvao.h"
#include "material.h"
#include <exception>
#include <map>

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec4 boneIDs;
	glm::vec4 weights;

	Vertex() : boneIDs(glm::vec4(0)), weights(glm::vec4(0.f)) {}
};

struct VertexHashFunc
{
	std::size_t operator()(const Vertex& v) const
	{
		size_t result = 2166136261;
		result += 131 * (v.pos.x + v.pos.y + v.pos.z);
		result += 31 * (v.uv.x + v.uv.y);
		result += 91 * (v.normal.x + v.normal.y + v.normal.z);
		result += 71 * (v.tangent.x + v.tangent.y + v.tangent.z);
		return result;
	}
};

struct VertexEqualKey
{
	bool operator () (const Vertex& v1, const Vertex& v2) const
	{
		return v1.pos == v2.pos
			&& v1.uv == v2.uv
			&& v1.tangent == v2.tangent
			&& v1.normal == v2.normal;
	}
};




class SubMesh {

public:
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	Material material;
	AABB box;

	// for skin
	glm::mat4 meshMtx;
	glm::mat4 bonePreMtx;
	std::vector<glm::mat4> bonePostMtx;


	SubMesh() = default;
	~SubMesh() = default;
	SubMesh(std::vector<Vertex> v, std::vector<int> i) : vertices(v), indices(i) {}
	SubMesh(std::vector<Vertex> v, std::vector<int> i, Material m)
		: vertices(v), indices(i), material(m) {}


};


class Mesh {
public:
	std::vector<SubMesh> submeshes;
	std::vector<std::string> boneNames;
	std::map<std::string, int> boneNameMapIdx;
	int nBones;
	Material::MaterialType mtype;

	int getSubMeshCount() 
	{ 
		return submeshes.size(); 
	}

	SubMesh& getSubMesh(int i)
	{
		if (i > 0 && i < submeshes.size())
			throw "ERROR: index out of submeshes's range!";
		else
			return submeshes[i];
	}
};



struct Cube {
	std::vector<float> vertices;
	std::vector<int> indices;
	AABB box;

	bool hasNormal;
	bool hasTangent;

	Material m;

	Cube() = default;
	Cube(std::vector<float>& vertices, std::vector<int>& indices, bool hasNormal, bool hasTagent)
		:vertices(vertices), indices(indices), hasNormal(hasNormal), hasTangent(hasTagent) { }
	Cube(std::vector<float>& vertices, bool hasNormal, bool hasTagent)
		:vertices(vertices), indices(indices), hasNormal(hasNormal), hasTangent(hasTagent) { }
};

struct SkyBox
{
	std::vector<float> vertices{
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	Texture3D tex;
};

#endif // !MESHVAO_H

