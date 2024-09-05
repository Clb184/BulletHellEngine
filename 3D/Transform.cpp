#include "Transform.h"

Transform::Transform() {
	this->m_ModelMatrix = glm::mat4(1.0f);
	this->m_NormalMatrix = glm::mat3(1.0f);
	this->m_Position = glm::vec3(0.0f);
	this->m_Rotation = glm::vec3(0.0f);
	this->m_Scale = glm::vec3(1.0f);
	ZeroMemory(this->m_Dummy, sizeof(this->m_Dummy));
	this->m_pMatrices3D = new Clb184::CConstantBuffer(&this->m_MatrixUniform, sizeof(Clb184::BasicTransformBaked), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

Transform::~Transform() {
	SAFE_DELETE(this->m_pMatrices3D);
}

void Transform::UpdateMatrix() {
	glm::vec3 rot = this->m_Rotation;
	glm::quat q = glm::quat();
	glm::quat qx = glm::quat(sinf(rot.x * 0.5f), 0.0f, 0.0f, cosf(rot.x * 0.5f));
	glm::quat qy = glm::quat(0.0f, sinf(rot.y * 0.5f), 0.0f, cosf(rot.y * 0.5f));
	glm::quat qz = glm::quat(0.0f, 0.0f, sinf(rot.z * 0.5f), cosf(rot.z * 0.5f));
	q = qz * qy * qx;

	this->m_ModelMatrix = glm::mat4(1.0f);
	this->m_ModelMatrix = glm::scale(this->m_ModelMatrix, this->m_Scale);
	this->m_ModelMatrix = glm::translate(this->m_ModelMatrix, this->m_Position) * glm::mat4(q);

	this->m_NormalMatrix = glm::inverse(this->m_ModelMatrix);
	this->m_MatrixUniform.model = glm::transpose(this->m_ModelMatrix);
	this->m_pMatrices3D->QuickLockUnlock(&this->m_MatrixUniform, sizeof(Clb184::BasicTransformBaked));
}

void Transform::SetPosition(const glm::vec3& pos) {
	this->m_Position = pos;
}

glm::vec3 Transform::GetPosition() {
	return this->m_Position;
}

void Transform::SetRotation(const glm::vec3& rot) {
	this->m_Rotation = rot;
}

glm::vec3 Transform::GetRotation() {
	return this->m_Rotation;
}

void Transform::SetScale(const glm::vec3& scale) {
	this->m_Scale = scale;
}

glm::vec3 Transform::GetScale() {
	return this->m_Scale;
}

void Transform::Bind(int slot) {
	this->m_pMatrices3D->BindToContext(slot, SHADER_RESOURCE_BIND_VS);
}

SQInteger Transform_SetPosition(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Transform* pObj;
	};
	if (sq_gettop(v) == 4 &&
		((sq_gettype(v, -1) | sq_gettype(v, -2) | sq_gettype(v, -3)) == OT_FLOAT)) {
		glm::vec3 pos;
		sq_getinstanceup(v, -4, &pData, NULL);
		sq_getfloat(v, -3, &pos.x);
		sq_getfloat(v, -2, &pos.y);
		sq_getfloat(v, -1, &pos.z);
		pObj->SetPosition(pos);
	}

	return 0;
}

SQInteger Transform_SetRotation(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Transform* pObj;
	};
	if (sq_gettop(v) == 4 &&
		((sq_gettype(v, -1) | sq_gettype(v, -2) | sq_gettype(v, -3)) == OT_FLOAT)) {
		glm::vec3 rot;
		sq_getinstanceup(v, -4, &pData, NULL);
		sq_getfloat(v, -3, &rot.x);
		sq_getfloat(v, -2, &rot.y);
		sq_getfloat(v, -1, &rot.z);
		pObj->SetRotation(rot);
	}

	return 0;
}

SQInteger Transform_SetScale(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Transform* pObj;
	};
	if (sq_gettop(v) == 4 &&
		((sq_gettype(v, -1) | sq_gettype(v, -2) | sq_gettype(v, -3)) == OT_FLOAT)) {
		glm::vec3 scale;
		sq_getinstanceup(v, -4, &pData, NULL);
		sq_getfloat(v, -3, &scale.x);
		sq_getfloat(v, -2, &scale.y);
		sq_getfloat(v, -1, &scale.z);
		pObj->SetScale(scale);
	}

	return 0;
}

SQInteger Transform_UpdateMatrix(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Transform* pObj;
	};
	if (sq_gettop(v) == 1) {
		sq_getinstanceup(v, -1, &pData, NULL);
		pObj->UpdateMatrix();
	}

	return 0;
}

SQInteger Transform_Bind(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Transform* pObj;
	};
	if (sq_gettop(v) == 1) {
		sq_getinstanceup(v, -1, &pData, NULL);
		pObj->Bind(1);
	}

	return 0;
}
