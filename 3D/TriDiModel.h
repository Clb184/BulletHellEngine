#ifndef TRIDIMODEL_INCLUDED
#define TRIDIMODEL_INCLUDED
#include "Material.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

struct VertexModelInfo {
	//Vertex position
	float x;
	float y;
	float z;
	//Color
	float r;
	float g;
	float b;
	float a;
	//UV
	float u;
	float v;
	//Normals
	float i;
	float j;
	float k;
};

struct Mesh {
	VertexModelInfo* vertex;

};

class __TriDiModel {
public:
private:
	std::vector<Mesh> m_VertexGroups;

};

#endif