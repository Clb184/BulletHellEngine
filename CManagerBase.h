#ifndef CMANAGERBASE_INCLUDED
#define CMANAGERBASE_INCLUDED

#include <squirrel.h>

class CManagerBase {
public:
	virtual void Initialize() = 0;
	virtual void Move() = 0;
	static void SetDeltaTime(float* pTime) { m_pDeltaTime = pTime; }
	static void SetVM(HSQUIRRELVM v) { m_VM = v; }
	static HSQUIRRELVM GetVM() { return m_VM; }
	static void SetTextureClass(const HSQOBJECT& obj) { m_TextureObj = obj; }
protected:
	static float* m_pDeltaTime;
	static HSQUIRRELVM m_VM;
	static HSQOBJECT m_TextureObj;
	HSQOBJECT m_ArrayObj;
};


#endif