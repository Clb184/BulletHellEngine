#ifndef OBJMODEL_INCLUDED
#define OBJMODEL_INCLUDED
#include <cstdio>
#include <iostream>
#include <map>
#include <glm/matrix.hpp>
#include <filesystem>
//#define _CRTDBG_MAP_ALLOC
#include "3DModel.h"

class OBJModel {
public:
	void LoadFromFile(std::filesystem::path file);

	std::vector<vertGroup> m_ModelVertex;
	~OBJModel();
private:
	class __file {
	public:
		char* __load(std::filesystem::path);
		size_t size;
		char* buffer = nullptr;
		void __free();
	};

	void ParseObj();
	void ParseMat();

	void LoadObj(std::filesystem::path);
	void LoadMaterial(std::filesystem::path);

	//Store the XYZ of vertex
	std::vector<glm::vec3> m_Vec3Vec;
	//UV for texCoords
	std::vector<glm::vec2> m_UVVec;
	//Normals (For shading!)
	std::vector<glm::vec3> m_NormalVec;
	__file m_Obj;
	__file m_Mat;
	std::map<std::string, Material> m_MatArray;
};
#endif