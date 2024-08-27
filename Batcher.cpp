#include "Batcher.h"
#include "MemoryPool.h"

CMemoryPool<Node<SpriteInstance>> g_SIPool;

Batcher::Batcher(int cnt) :
	m_VBuffer(nullptr, cnt * 4 * sizeof(Clb184::Vertex2D), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
	m_lInstances(cnt, &g_SIPool)
{
	this->m_bOnUpdate = true;
	//this->m_pInstances = new SpriteInstance*[cnt];
	this->m_MaxCount = cnt;

	UINT* pIndex = (UINT*)malloc(cnt * 6 * sizeof(UINT));
	for (int i = 0; i < cnt; i++) {

		pIndex[0 + i * 6] = 0 + (i << 2);
		pIndex[1 + i * 6] = 1 + (i << 2);
		pIndex[2 + i * 6] = 2 + (i << 2);

		pIndex[3 + i * 6] = 0 + (i << 2);
		pIndex[4 + i * 6] = 2 + (i << 2);
		pIndex[5 + i * 6] = 3 + (i << 2);

	}
	this->m_pIndices = pIndex;
	this->m_IBuffer = Clb184::CIndexBuffer(pIndex, cnt * 6 * sizeof(UINT), D3D11_USAGE_DEFAULT, 0);
	//this->m_IBuffer.Unlock(cnt * sizeof(UINT) * 6);
	this->m_VBuffer.SetStrideInfo(sizeof(Clb184::Vertex2D));
}

Batcher::~Batcher() {
	//SAFE_DELETE(this->m_pInstances);
	SAFE_FREE(this->m_pIndices);
}

SpriteInstance** Batcher::GetInstance(int id){
	return (id < this->m_MaxCount) ? &this->m_pInstances[id] : nullptr;
}

Node<SpriteInstance>* Batcher::GetFreeInstance() {
	return m_lInstances.AddFront();
}

bool Batcher::DeleteInstance(size_t idx) {
	return m_lInstances.DeleteFromIndex(idx);
}

void Batcher::SetInstance(int id, SpriteInstance* ref) {
	if(id < this->m_MaxCount)
		this->m_pInstances[id] = ref;
}

void Batcher::Draw(int cnt) {
	if (!cnt)
		return;

	Node<SpriteInstance> pInst;
	FRect rc;
	FRect pos = {};
	float w;
	float h;
	float x;
	float y;
	float l;
	float r;
	float t;
	float b;
	UINT color;
	Clb184::Vertex2D* vertex = (Clb184::Vertex2D*)this->m_VBuffer.Lock();
	Iterator<SpriteInstance> it = *m_lInstances.GetBackIterator();
	it.MoveFront();
	for (int i = 0; i < cnt; i++) {
		pInst = *it.GetData();
		if (!pInst.Active) {
			i--;
			it.MoveFront();
			continue;
		}

		float c = cos(pInst.Rot);
		float s = sin(pInst.Rot);
		//FTA::sincos(pInst.Rot, &s, &c);
		rc = pInst.TexCoord;

		w = pInst.Width *  pInst.ScaleX;
		h = pInst.Height * pInst.ScaleY;


		// = CenterRect(0.0f, 0.0f, w * this->m_ScaleX, h * this->m_ScaleY);

		switch (pInst.Center >> 2) {
		default:
		case 0:			//Left
			pos.left = 0.0f;
			pos.right = w;
			break;
		case 1:			//Center
			pos.left = -w * 0.5f;
			pos.right = -pos.left;
			break;
		case 2:			//Right
			pos.left = -w;
			pos.right = 0.0f;
			break;
		}

		switch (pInst.Center & 0b11) {
		default:
		case 0:			//Top
			pos.top = 0.0f;
			pos.bottom = h;
			break;
		case 1:			//Center
			pos.top = -h * 0.5f;
			pos.bottom = -pos.top;
			break;
		case 2:			//Bottom
			pos.top = -h;
			pos.bottom = 0.0f;
			break;
		}

		x = pInst.PosX;
		y = pInst.PosY;
		l = pos.left;
		r = pos.right;
		t = pos.top;
		b = pos.bottom;

		color = pInst.Color;

		vertex[0 + (i << 2)] = { l * c - t * s + x, l * s + t * c + y, color, rc.left, rc.top };
		vertex[1 + (i << 2)] = { r * c - t * s + x, r * s + t * c + y, color, rc.right, rc.top };
		vertex[2 + (i << 2)] = { r * c - b * s + x, r * s + b * c + y, color, rc.right, rc.bottom };
		vertex[3 + (i << 2)] = { l * c - b * s + x, l * s + b * c + y, color, rc.left, rc.bottom };
		it.MoveFront();

	}
	this->m_VBuffer.Unlock((cnt << 2) * sizeof(Clb184::Vertex2D));
	this->m_IBuffer.BindToContext(0, SHADER_RESOURCE_BIND_VS);
	this->m_VBuffer.BindToContext(0, SHADER_RESOURCE_BIND_VS);
	Clb184::g_pContext->DrawIndexed(6 * cnt, 0, 0);
}
