#include "Camera.h"
#include <cmath>

Camera::Camera() {
	this->m_CamMat = glm::mat4(1.0f);
	this->m_ViewMat = glm::mat4(1.0f);
	this->m_ProjMat = glm::mat4(1.0f);
	this->m_Orientation = glm::vec3(0.0f, 1.0f, 0.0f);
	this->m_Up = glm::vec3(0.0, 0.0, -1.0);
	this->m_Position = glm::vec3(0.0);
	this->m_Rotation = glm::vec3(0.0);
	this->m_Ratio = 16.0f / 9.0f;
	this->m_FOV = glm::radians(90.0f);
	this->m_Fog.x = 50.0f;
	this->m_Fog.y = 60.0f;
}

Camera::~Camera() {

}

void Camera::Initialize() {
	this->m_CameraMatrix.Initialize(&this->m_CameraUniform, sizeof(Clb184::BasicTransforms), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void Camera::UpdateMatrix() {
	glm::vec3 rot = this->m_Rotation;
	glm::mat4 view = glm::lookAt(this->m_Position, this->m_Position + this->m_Orientation, this->m_Up);
	glm::quat q = glm::quat();
	glm::quat qx = glm::quat(sinf(rot.x * 0.5f), 0.0f, 0.0f, cosf(rot.x * 0.5f));
	glm::quat qy = glm::quat(0.0f, sinf(rot.y * 0.5f), 0.0f, cosf(rot.y * 0.5f));
	glm::quat qz = glm::quat(0.0f, 0.0f, sinf(rot.z * 0.5f), cosf(rot.z * 0.5f));

	q = qz * qy * qx;
	glm::mat4 proj = glm::perspective(this->m_FOV, this->m_Ratio, 1.0f, 1000.0f) * glm::mat4(q);

	this->m_ViewMat = glm::transpose(view);
	this->m_ProjMat = glm::transpose(proj);
	this->m_CamMat = glm::transpose(proj * view);
	this->m_CameraMatrix.QuickLockUnlock(&this->m_CameraUniform, sizeof(Clb184::BasicTransforms));

}

glm::mat4 Camera::GetCamMat() {
	return this->m_CamMat;
}


void Camera::SetFOV(float FOV) {
	this->m_FOV = FOV;
}

float Camera::GetFOV() const {
	return this->m_FOV;
}

void Camera::SetRatio(float rat) {
	this->m_Ratio = rat;
}

float Camera::GetRatio() const {
	return this->m_Ratio;
}

void Camera::SetPos(const glm::vec3& pos) {
	this->m_Position = pos;
}

glm::vec3 Camera::GetPos() {
	return this->m_Position;
}

void Camera::SetRotation(const glm::vec3& rot) {
	this->m_Rotation = rot;
}

glm::vec3 Camera::GetRotation() {
	return this->m_Rotation;
}

void Camera::SetFog(const glm::vec2& fog) {
	this->m_Fog = fog;
}


void Camera::Bind(int slot) {
	this->m_CameraMatrix.BindToContext(slot, SHADER_RESOURCE_BIND_VS);
}

SQInteger Camera_Destructor(SQUserPointer pc, SQInteger size) {
	Camera* pCam = (Camera*)pc;
	if (pCam) delete pCam;
	return SQ_OK;
}


SQInteger Camera_Constructor(HSQUIRRELVM v) {
	if (sq_gettop(v) == 1) {
		Camera* pCam = new Camera();
		sq_setinstanceup(v, -1, pCam);
		sq_setreleasehook(v, -1, Camera_Destructor);
	}

	return 0;
}

SQInteger Camera_SetPos(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Camera* pCam;
	};
	if (sq_gettop(v) == 4 &&
		((sq_gettype(v, -1) | sq_gettype(v, -2) | sq_gettype(v, -3)) == OT_FLOAT)) {
		glm::vec3 pos;
		sq_getinstanceup(v, -4, &pData, NULL);
		sq_getfloat(v, -3, &pos.x);
		sq_getfloat(v, -2, &pos.y);
		sq_getfloat(v, -1, &pos.z);
		pCam->SetPos(pos);
	}

	return 0;
}

SQInteger Camera_SetFOV(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Camera* pCam;
	};
	if (sq_gettop(v) == 2 &&
		(sq_gettype(v, -1) == OT_FLOAT)) {
		SQFloat fov;
		sq_getinstanceup(v, -2, &pData, NULL);
		sq_getfloat(v, -1, &fov);
		pCam->SetFOV(fov);
	}

	return 0;
}

SQInteger Camera_SetRatio(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Camera* pCam;
	};
	if (sq_gettop(v) == 2 &&
		(sq_gettype(v, -1) == OT_FLOAT)) {
		SQFloat ratio;
		sq_getinstanceup(v, -2, &pData, NULL);
		sq_getfloat(v, -1, &ratio);
		pCam->SetRatio(ratio);
	}

	return 0;
}

SQInteger Camera_SetRotation(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Camera* pCam;
	};
	if (sq_gettop(v) == 4 &&
		((sq_gettype(v, -1) | sq_gettype(v, -2) | sq_gettype(v, -3)) == OT_FLOAT)) {
		glm::vec3 rot;
		sq_getinstanceup(v, -4, &pData, NULL);
		sq_getfloat(v, -3, &rot.x);
		sq_getfloat(v, -2, &rot.y);
		sq_getfloat(v, -1, &rot.z);
		pCam->SetRotation(rot);
	}

	return 0;
}
