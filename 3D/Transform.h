#ifndef TRANSFORM_INCLUDED
#define TRANSFORM_INCLUDED
#include "..\Graphics.h"
#include "..\SQHandlers.h"

class Transform {
public:
	Transform();
	~Transform();

	//Matrix calculation and manipulation
	void UpdateMatrix();

	//Transforms
	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();
	void SetRotation(const glm::vec3& rot);
	glm::vec3 GetRotation();
	void SetScale(const glm::vec3& scale);
	glm::vec3 GetScale();

	//Binding current transform
	void Bind(int slot);

private:
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;

	//Transform and normal matrix
	union {
		Clb184::BasicTransformBaked m_MatrixUniform;
		struct {
			glm::mat4 m_ModelMatrix;
			glm::mat3 m_NormalMatrix;
			float m_Dummy[7];
		};
	};
	Clb184::CConstantBuffer* m_pMatrices3D;
};

SQInteger Transform_SetPosition(HSQUIRRELVM v);
SQInteger Transform_SetRotation(HSQUIRRELVM v);
SQInteger Transform_SetScale(HSQUIRRELVM v);
SQInteger Transform_UpdateMatrix(HSQUIRRELVM v);
SQInteger Transform_Bind(HSQUIRRELVM v);

inline bool RegisterTransformClass(HSQUIRRELVM v) {
	SQClassFunctions funcs{
		{Transform_SetPosition,		"SetPosition"},
		{Transform_SetRotation,		"SetRotation"},
		{Transform_SetScale,		"SetScale"},
		{Transform_UpdateMatrix,	"UpdateMatrix"},
	};

	return CreateSQClassAndFunctions<Transform>(v, "Transform", funcs);
}

#endif