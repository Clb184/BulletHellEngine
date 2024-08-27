#include "Tex.h"

SQInteger Texture_Constructor(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Clb184::CTexture* pTex;
	};
	const SQChar* pName;
	if (sq_gettop(v) == 2 && SQ_SUCCEEDED(sq_getstring(v, 2, &pName))) {
		pTex = new Clb184::CTexture();
		if (pTex->LoadTextureFromFile(pName)) {
			sq_setinstanceup(v, 1, pTex);
			sq_setreleasehook(v, 1, DefaultSQClassDestructor<Clb184::CTexture>);
		}
		else {
			delete pTex;
		}
	}

	return 0;
}

SQInteger Texture_Bind(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Clb184::CTexture* pTex;
	};
	SQUserPointer pType;
	//if (sq_gettop(v) == 1) {
	sq_getinstanceup(v, -1, &pData, NULL);
	pTex->BindToContext(0, SHADER_RESOURCE_BIND_PS);
	//}

	return 0;
}