#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED
#include "ClbLibMain.h"
#include "..\graphics.h"
#include "..\SqHandlers.h"
class Camera {
public:
	//Constructor and destructor
	Camera();
	~Camera();

	//Initializer
	void Initialize();

	//Matrix operations
	void UpdateMatrix();
	glm::mat4 GetCamMat();

	//Camera transforms
	void SetFOV(float fov);
	float GetFOV() const;
	void SetRatio(float ratio);
	float GetRatio() const;
	void SetPos(const glm::vec3& pos);
	glm::vec3 GetPos();
	void SetRotation(const glm::vec3& rot);
	glm::vec3 GetRotation();

	//Fog setup
	void SetFog(const glm::vec2& fog);

	//Binding to context
	void Bind(int slot);

private:
	float m_Ratio;
	float m_FOV;

	glm::vec3 m_Orientation;
	glm::vec3 m_Up;
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;

private:
	union {
		Clb184::BasicTransforms m_CameraUniform;
		struct {
			glm::mat4 m_CamMat;
			glm::mat4 m_ViewMat;
			glm::mat4 m_ProjMat;
			glm::vec2 m_Fog;
		};
	};
	Clb184::CConstantBuffer m_CameraMatrix;
};

SQInteger Camera_Constructor(HSQUIRRELVM v);
SQInteger Camera_SetPos(HSQUIRRELVM v);
SQInteger Camera_SetRotation(HSQUIRRELVM v);
SQInteger Camera_SetFOV(HSQUIRRELVM v);
SQInteger Camera_Update(HSQUIRRELVM v);
SQInteger Camera_Bind(HSQUIRRELVM v);
SQInteger Camera_SetRatio(HSQUIRRELVM v);

inline bool RegisterCameraClass(HSQUIRRELVM v) {
	bool create_class;
	sq_pushstring(v, "Camera", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, SQFalse));
	RegisterSQClassFunc(v, Camera_Constructor, "constructor");
	RegisterSQClassFunc(v, Camera_SetPos, "SetPos");
	RegisterSQClassFunc(v, Camera_SetRotation, "SetRotation");
	RegisterSQClassFunc(v, Camera_SetFOV, "SetFOV");
	RegisterSQClassFunc(v, Camera_SetRatio, "SetRatio");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}



#endif