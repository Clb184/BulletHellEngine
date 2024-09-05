#ifndef _3DMODEL_INCLUDED
#define _3DMODEL_INCLUDED
#include <vector>
#include "..\Graphics.h"

struct VertexGL {
	//Position
	float vx = 0.0f;
	float vy = 0.0f;
	float vz = 0.0f;
	//Colors
	float r = 1.0;
	float g = 1.0;
	float b = 1.0;
	float a = 1.0;
	//TexCoords
	float u = 0.0f;
	float v = 0.0f;
	//Normals
	float vni = 0.0f;
	float vnj = 0.0f;
	float vnk = 0.0f;
};

struct Material {
	std::string MatName;
	std::string MatFileName;
	Clb184::CTexture mat;

	//AmbientCol
	float ar = 1.0;
	float ag = 1.0;
	float ab = 1.0;
	//DiffuseCol
	float dr = 1.0;
	float dg = 1.0;
	float db = 1.0;
	//SpecularCol
	float sr = 1.0;
	float sg = 1.0;
	float sb = 1.0;
	//ECol???
	float er = 1.0;
	float eg = 1.0;
	float eb = 1.0;
	//Alpha
	float alpha = 1.0;
};

struct vertGroup {
	VertexGL* pVert = nullptr;
	std::string texname;
	Clb184::CTexture tex;
	size_t size;
	Clb184::CVertexBuffer* pVBuffer;
	//OpenGLObj obj;
};

struct VertexArray {
	Material* pMaterial = nullptr;
	std::vector<VertexGL> vertex;
};

struct PartEx {
	std::string PartName;
	std::string MatName;
	Clb184::CTexture texture;
	VertexGL* VxArray = nullptr;
	size_t size;
};

struct Part {
	std::string PartName;
	Material material;
	std::vector<VertexGL> VxArray;
};

struct Mesh {
	std::string MeshName;
	std::vector<Part> Parts;
};

struct _3DModel {
	std::string ModelName;
	std::vector<Mesh> Meshes;
};

#endif