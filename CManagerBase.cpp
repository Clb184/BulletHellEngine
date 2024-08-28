#include "CManagerBase.h"

float* CManagerBase::m_pDeltaTime = nullptr;
HSQUIRRELVM CManagerBase::m_VM = nullptr;
HSQOBJECT CManagerBase::m_TextureObj = {};