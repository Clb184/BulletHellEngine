#pragma once
#include <string>
#include <map>
#include <list>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

/*
*TODO:
*class 3DModel{
	int sign;
	int vertexGroupCnt;
	int materialCnt;
	std::string materialPath;
}

struct __Material{
	char* matName;
	char* textureName; // If empty, load with no texture
	char* vertexShader;
	char* fragmentShader;
}

struct __VertexGroup {
	char* matName;
	int vertexCnt;
	GLfloat vertexInfo[12];
}

class Material {
private:
	std::string matName;
	Texture* texture;
	Shader shader;
public:
	void createMaterial(std::string[4]);//Material name, Texture name, VertexShader, FragmentShader
	void useMaterial();
	void deleteMaterial();
}

class VertexGroup {
	std::string vertexGroupName;
	int vertexCnt;
	GLfloat* pVertexData; // vec3 pos, vec3 color, vec2 uv, vec3 normals
	Material* material;
}
*/


struct modelUniformInfo {
	std::map<std::string, float> floatData;
	std::map<std::string, glm::vec2> vec2Data;
	std::map<std::string, glm::vec3> vec3Data;
	std::map<std::string, glm::vec4> vec4Data;

	std::map<std::string, int> intData;
	std::map<std::string, glm::ivec2> ivec2Data;
	std::map<std::string, glm::ivec3> ivec3Data;
	std::map<std::string, glm::ivec4> ivec4Data;

	std::map<std::string, glm::mat2> mat2Data;
	std::map<std::string, glm::mat3> mat3Data;
	std::map<std::string, glm::mat4> mat4Data;
};

struct modelBatchPart {
	int modelID = -1;
	glm::vec3 position = {};
	glm::vec3 rotation = {};
	glm::vec3 scale = {};
	modelUniformInfo uinfo = {};
};

typedef std::list<modelBatchPart> ModelBatch;
class TriDiModelManager {
public:
	void loadModels(const std::vector<std::string>&);
	void draw(ModelBatch&);
private:
	inline glm::mat4 createModelMatrix(const glm::vec3&, const glm::vec3&, const glm::vec3&);
private:
	glm::mat4* m_pCamMat;
};

glm::mat4 TriDiModelManager::createModelMatrix(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& sca) {
	glm::mat4 modelMat = glm::mat4(1.0);
	modelMat = glm::translate(modelMat, pos);
	modelMat = glm::rotate(modelMat, rot.x, { 1.0, 0.0, 0.0f });
	modelMat = glm::rotate(modelMat, rot.y, { 0.0, 1.0, 0.0f });
	modelMat = glm::rotate(modelMat, rot.z, { 0.0, 0.0, 1.0f });
	modelMat = glm::scale(modelMat, { sca });
	return modelMat;
}