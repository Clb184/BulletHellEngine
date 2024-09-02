#ifndef CMANAGERBASE_INCLUDED
#define CMANAGERBASE_INCLUDED

#include <squirrel.h>
#include "LinkedArrayList.h"
#include "MemoryPool.h"

template <class T>
class CManagerBase {
public:
	CManagerBase(size_t num_elem, CMemoryPool<Node<T>>* pMem);
	virtual void Initialize() = 0;
	virtual void Move() = 0;
	static void SetDeltaTime(float* pTime) { m_pDeltaTime = pTime; }

	static void SetVM(HSQUIRRELVM v) { m_VM = v; }
	static HSQUIRRELVM GetVM() { return m_VM; }

	static void SetTextureClass(const HSQOBJECT& obj) { m_TextureObj = obj; }
	virtual int GetItemCnt() const = 0;
	virtual Node<T>* CreateObject() = 0;
protected:
	static float* m_pDeltaTime;
	static HSQUIRRELVM m_VM;
	static HSQOBJECT m_TextureObj;
	HSQOBJECT m_ArrayObj;
	CDoubleLinkedArrayList<T> m_TaskList;
};
template<class T>
float* CManagerBase<T>::m_pDeltaTime = nullptr;
template<class T>
HSQUIRRELVM CManagerBase<T>::m_VM = nullptr;
template<class T>
HSQOBJECT CManagerBase<T>::m_TextureObj = {};

template<class T>
CManagerBase<T>::CManagerBase(size_t num_elem, CMemoryPool<Node<T>>* pMem) :
	m_TaskList(num_elem, pMem)
{

}
#endif