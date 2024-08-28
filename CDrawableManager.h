#ifndef DRAWABLEMANAGER_INCLUDED
#define DRAWABLEMANAGER_INCLUDED
#include "Graphics.h"

class CDrawableManager {
public:
	CDrawableManager();
	virtual void Draw() = 0;
private:
	Clb184::CVertexBuffer m_VBuffer;
	Clb184::CIndexBuffer m_IBuffer;
};

#endif // !DRAWABLEMANAGER_INCLUDED
