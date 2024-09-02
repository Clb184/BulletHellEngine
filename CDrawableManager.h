#ifndef DRAWABLEMANAGER_INCLUDED
#define DRAWABLEMANAGER_INCLUDED
#include "Graphics.h"
#include "CManagerBase.h"

template<class T>
class CDrawableManager : public CManagerBase<T> {
public:
	CDrawableManager(size_t num_elem, CMemoryPool<Node<T>>* pMem);
	virtual void Draw() = 0;
protected:
	Clb184::CVertexBuffer m_VBuffer;
};
template<class T>
CDrawableManager<T>::CDrawableManager(size_t num_elem, CMemoryPool<Node<T>>* pMem) : CManagerBase<T>(num_elem, pMem) {
	m_VBuffer.SetStrideInfo(sizeof(Clb184::Vertex2D));
}


#endif // !DRAWABLEMANAGER_INCLUDED
