#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>


struct Texture2D
{
	std::string filename;
	bool flip;
	
	Texture2D() = default;
	~Texture2D() = default;
	Texture2D(std::string& filename, bool flip = true) : filename(filename), flip(flip){}
};

struct Texture3D
{
	std::string filename[6];
	bool flip;
	
	Texture3D() = default;
	~Texture3D() = default;
	Texture3D(std::string filename[6], bool flip = true) : flip(flip) 
	{
		for (size_t i = 0; i < 6; i++)
		{
			this->filename[i] = filename[i];
		}
	}
};

struct Material
{
public:
	enum MaterialType
	{
		ePBR,
		eClassic,
	};

	MaterialType mtype;

	// for classic
	Texture2D NormalMap;
	Texture2D Ambient;
	Texture2D Diffuse;
	Texture2D Specular;
	int			Shininess;
	float		AmbientFactor;
	float		SpecularFactor;
	float		DiffuseFactor;

	// for pbr
	Texture2D Albedo;
	Texture2D AO;
	Texture2D Metallic;
	Texture2D Roughness;

	bool isValid(Texture2D tex)
	{
		return tex.filename.size() != 0;
	}

};


#endif // !#define _MATERIAL_H
