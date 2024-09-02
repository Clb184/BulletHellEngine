#include "Enemy.h"
#include "Player.h"

static CMemoryPool<Node<Enemy>> g_TaskEnemyPool;
EnemyManager g_EnmManager;

EnemyManager::EnemyManager() :
	CDrawableManager<Enemy>(ENEMY_MAX, &g_TaskEnemyPool)
{
	m_ppTextures = new Clb184::CTexture * [ENEMY_MAX];
#ifdef DEBUG
	m_bDebugDrawEnable = true;
	m_Count = 0;
#endif
}

EnemyManager::~EnemyManager() {
	if (m_ppTextures) delete[] m_ppTextures;
}

void EnemyManager::Initialize() {
	if (m_VM) {
		m_TaskList.Reset();
		m_VBuffer.Initialize(nullptr, sizeof(Clb184::Vertex2D) * ENEMY_MAX * 4, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		sq_pushroottable(m_VM);
		m_ArrayObj = SQCreateArray(m_VM, _SC("____enm"), ENEMY_MAX);
		sq_pop(m_VM, 1);
#ifdef DEBUG
		m_PrimBuffer.SetStrideInfo(sizeof(Clb184::Point2D));
		m_PrimBuffer.Initialize(nullptr, sizeof(Clb184::Point2D) * 17 * ENEMY_MAX, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		constexpr float delta = 3.14159 / 8.0;
		float ang = 0.0f;
		for (int i = 0; i < 16; i++) {
			m_Points[i].Pos = {cos(ang), sin(ang)};
			m_Points[i].color = 0x5c1414ff;
			ang += delta;
		}
		m_Points[16] = m_Points[0];
		m_Count = 0;
#endif
	}
}


void EnemyManager::Move() {
	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	CollissionCircle phit = g_Player.co_shape;
	it.MoveFront();
	int i = 0;
	int max = m_TaskList.GetSize();
	while ((pInst = it.GetData()) && pInst->active && i < max) {
		if (pInst->life <= 0)
			pInst->is_delete = true;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
		pInst->co_shape.pos = pInst->pos;
		if (hitCheckCC(phit, pInst->co_shape)) {
			g_Player.Kill();
		}
		it.MoveFront();
		i++;
	}
}

void EnemyManager::Draw() {
	int cnt = 0;

	if (!m_TaskList.GetSize() || !m_VM) {
#ifdef DEBUG
		m_Count = 0;	
#endif
		return;
	}

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	if (!pVertices) return;

#ifdef DEBUG
	Clb184::Point2D* pPoints = nullptr;
	if (m_bDebugDrawEnable) {
		pPoints = (Clb184::Point2D*)(m_PrimBuffer.Lock());
		if (!pPoints) return;
	}
#endif
	it.MoveFront();
	sq_pushobject(m_VM, m_ArrayObj);
	while ((pInst = it.GetData()) && pInst->active) {
		if (pInst->is_delete) { it.MoveFront(); continue; }
		Enemy test = *pInst;
		sq_pushinteger(m_VM, pInst->id);
		sq_get(m_VM, -2);
		sq_pushobject(m_VM, m_TextureObj);

		sq_pushstring(m_VM, _SC("texture"), -1);
		sq_get(m_VM, -3);
		if (SQTrue == sq_instanceof(m_VM)) {
			sq_getinstanceup(m_VM, -1, (SQUserPointer*)(&m_ppTextures[cnt]), NULL);

			float c = cosf(test.rotation);
			float s = sinf(test.rotation);
			glm::vec4 rc = test.uv;

			glm::vec2 fsize = test.size * test.scale;


			glm::vec2 pos = {};

			pos.x = -test.size.x * 0.5f;
			pos.y = -test.size.y * 0.5f;

			float l = pos.x;
			float t = pos.y;

			UINT color = test.color;

			pVertices[(cnt << 2)] = { pos.x * c - pos.y * s + test.pos.x, pos.x * s + pos.y * c + test.pos.y, color, rc.x, rc.y };
			pVertices[(cnt << 2) + 1] = { -pos.x * c - pos.y * s + test.pos.x, -pos.x * s + pos.y * c + test.pos.y, color, rc.z, rc.y };
			pVertices[(cnt << 2) + 3] = { -pos.x * c - -pos.y * s + test.pos.x, -pos.x * s + -pos.y * c + test.pos.y, color, rc.z, rc.w };
			pVertices[(cnt << 2) + 2] = { pos.x * c - -pos.y * s + test.pos.x, pos.x * s + -pos.y * c + test.pos.y, color, rc.x, rc.w };

#ifdef DEBUG
			if (m_bDebugDrawEnable) {
				pPoints[(cnt * 17)] =      { m_Points[0].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 1] =  { m_Points[1].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 2] =  { m_Points[2].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 3] =  { m_Points[3].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 4] =  { m_Points[4].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 5] =  { m_Points[5].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 6] =  { m_Points[6].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 7] =  { m_Points[7].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 8] =  { m_Points[8].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 9] =  { m_Points[9].Pos  * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 10] = { m_Points[10].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 11] = { m_Points[11].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 12] = { m_Points[12].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 13] = { m_Points[13].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 14] = { m_Points[14].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 15] = { m_Points[15].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };
				pPoints[(cnt * 17) + 16] = { m_Points[16].Pos * test.co_shape.r + test.pos, HITBOX_COLOR };

			}
#endif
			cnt++;

		}
		it.MoveFront();
		sq_pop(m_VM, 3);
	}
	m_VBuffer.Unlock(sizeof(Clb184::Vertex2D) * 4 * cnt);

#ifdef DEBUG
	if (m_bDebugDrawEnable)
		m_PrimBuffer.Unlock(sizeof(Clb184::Point2D) * 17 * cnt);
#endif

	sq_pop(m_VM, 1);
	Clb184::CTexture* past = nullptr;
	Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
	Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_VBuffer.BindToContext(0,0);
	for (int i = 0; i < cnt; i++) {
		if (m_ppTextures[i] != past) {
			m_ppTextures[i]->BindToContext(0, SHADER_RESOURCE_BIND_PS);
			past = m_ppTextures[i];
		}
		Clb184::g_pContext->Draw(4, i << 2);
	}
#ifdef DEBUG
	m_Count = cnt;
#endif
}

Iterator<Enemy>* EnemyManager::GetListIterator() {
	return m_TaskList.GetBackIterator();
}

#ifdef DEBUG
void EnemyManager::SetDebugDraw(bool state) {
	m_bDebugDrawEnable = state;
}

void EnemyManager::DrawHitbox() {
	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessVertexShader();
	Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessPixelShader();
	m_PrimBuffer.BindToContext(0, 0);
	for (int i = 0; i < m_Count; i++) {
		Clb184::g_pContext->Draw(17, i * 17);
	}
}
#endif

int EnemyManager::GetItemCnt() const {
	return m_TaskList.GetSize();
}

Node<Enemy>* EnemyManager::CreateObject() {
	return m_TaskList.AddFront();
}

SQInteger Enemy_SetLife(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Node<Enemy>* pEnm;
	};
	SQInteger dmg;
	sq_getinstanceup(v, 1, &pData, NULL);
	sq_getinteger(v, 2, &dmg);
	pEnm->life = static_cast<int>(dmg);
	return 0;
}

SQInteger Enemy_GetLife(HSQUIRRELVM v) {
	union {
		SQUserPointer pData;
		Node<Enemy>* pEnm;
	};
	sq_getinstanceup(v, 1, &pData, NULL);
	sq_pushinteger(v, pEnm->life);
	return 1;
}
