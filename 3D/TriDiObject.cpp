#include "TriDiObject.h"

//_3DModelCache	TriDiObject::m_ModelCache;

TriDiObject::TriDiObject() {
	this->m_pTriDiModel = nullptr;
}

TriDiObject::~TriDiObject() {
	SAFE_DELETE(this->m_pTriDiModel);
}

void TriDiObject::LoadModel(std::filesystem::path file) {
	if (this->m_pTriDiModel)
		SAFE_DELETE(this->m_pTriDiModel);
	OBJModel* model = new OBJModel();
	model->LoadFromFile(file);
	this->m_pTriDiModel = model;
	//if (m_ModelCache.find(file) == m_ModelCache.end()) {
	//	OBJModel* model = new OBJModel();
	//	model->LoadFromFile(file);
	//	m_ModelCache.insert({ file, model });
	//	this->m_pTriDiModel = model;
	//}
	//else {
	//	this->m_pTriDiModel = m_ModelCache[file];
	//}
}

void TriDiObject::Draw() {
	if(this->m_pTriDiModel)
	for (auto& it : this->m_pTriDiModel->m_ModelVertex) {
		it.pVBuffer->BindToContext(0, 0);
		it.tex.BindToContext(0, SHADER_RESOURCE_BIND_PS);

		Clb184::g_pContext->Draw(it.size, 0);
	}
}

void TriDiObject::DrawInstanced(size_t cnt) {
	for (auto& it : this->m_pTriDiModel->m_ModelVertex) {
		it.pVBuffer->BindToContext(0, 0);
		it.tex.BindToContext(0, SHADER_RESOURCE_BIND_PS);

		Clb184::g_pContext->DrawInstanced(it.size, cnt, 0, 0);
	}
}

SQInteger Model_Load(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		TriDiObject* pObj;
	};
	if (sq_gettop(v) == 2 && (sq_gettype(v, -1) == OT_STRING)) {
		const SQChar* str;
		sq_getinstanceup(v, -2, &pData, NULL);
		sq_getstring(v, -1, &str);
		pObj->LoadModel(str);
	}

	return 0;
}

SQInteger Model_Draw(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		TriDiObject* pObj;
	};
	if (sq_gettop(v) == 1) {
		sq_getinstanceup(v, -1, &pData, NULL);
		pObj->Draw();
	}

	return 0;
}

SQInteger Model_DrawInstanced(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		TriDiObject* pObj;
	};
	if (sq_gettop(v) == 2 && (sq_gettype(v,-1) == OT_INTEGER)) {
		SQInteger cnt;
		sq_getinstanceup(v, -2, &pData, NULL);
		sq_getinteger(v, -1, &cnt);
		pObj->DrawInstanced(cnt);
	}

	return 0;
}
