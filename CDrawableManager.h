#ifndef DRAWABLEMANAGER_INCLUDED
#define DRAWABLEMANAGER_INCLUDED
#include "Graphics.h"
#include "CManagerBase.h"

class CDrawableManager : public CManagerBase {
public:
	CDrawableManager();
	virtual void Draw() = 0;
protected:
	Clb184::CVertexBuffer m_VBuffer;
	Clb184::CIndexBuffer m_IBuffer;
};

#endif // !DRAWABLEMANAGER_INCLUDED
