#include "Bullet.h"
#include "Player.h"

static CMemoryPool<Node<Bullet>> g_TaskBulletPool;
BulletManager g_BulletManager;

BulletManager::BulletManager() :
	m_TaskList(BULLET_MAX, &g_TaskBulletPool)
{
	m_bDebugDrawEnable = true;
	m_Count = 0;
}

BulletManager::~BulletManager() {

}

void BulletManager::Initialize() {
	if (m_VM) {
		m_TaskList.Reset();
		m_VBuffer.Initialize(nullptr, sizeof(Clb184::Vertex2D) * BULLET_MAX * 4, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		UINT* pIndex = new UINT[BULLET_MAX * 6 * sizeof(UINT)];
		for (int i = 0; i < BULLET_MAX; i++) {

			pIndex[0 + i * 6] = 0 + (i << 2);
			pIndex[1 + i * 6] = 1 + (i << 2);
			pIndex[2 + i * 6] = 2 + (i << 2);

			pIndex[3 + i * 6] = 0 + (i << 2);
			pIndex[4 + i * 6] = 2 + (i << 2);
			pIndex[5 + i * 6] = 3 + (i << 2);

		}
		m_IBuffer.Initialize(pIndex, BULLET_MAX * 6 * sizeof(UINT), D3D11_USAGE_DEFAULT, 0);
		delete[] pIndex;
		m_BulletTexture.CreateEmptyTexture(256, 256, 0xffffffff);
		sq_pushroottable(m_VM);
		m_ArrayObj = SQCreateArray(m_VM, _SC("____blt"), BULLET_MAX);
		sq_pop(m_VM, 1);

		m_PrimBuffer.SetStrideInfo(sizeof(Clb184::Point2D));
		m_PrimBuffer.Initialize(nullptr, sizeof(Clb184::Point2D) * 17 * BULLET_MAX, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		constexpr float delta = 3.14159 / 8.0;
		float ang = 0.0f;
		for (int i = 0; i < 16; i++) {
			m_Points[i].Pos = { cos(ang), sin(ang) };
			m_Points[i].color = 0x5c1414ff;
			ang += delta;
		}
		m_Points[16] = m_Points[0];
		m_Count = 0;
	}
}

void BulletManager::Move() {
	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Bullet>* pInst;
	Iterator<Bullet> it = *m_TaskList.GetBackIterator();
	CollissionCircle phit = g_Player.co_shape;
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		//Enemy test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
		pInst->co_shape.pos = pInst->pos;
		if (hitCheckCC(phit, pInst->co_shape)) {
			pInst->is_delete = true;
			g_Player.Kill();
			//sq_getprintfunc(m_VM)(m_VM, "hit!\n");
		}
		it.MoveFront();
	}
}

void BulletManager::Draw() {
	int cnt = 0;

	if (!m_TaskList.GetSize() || !m_VM) {
		m_Count = 0;
		return;
	}

	Node<Bullet>* pInst;
	Iterator<Bullet> it = *m_TaskList.GetBackIterator();

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	if (!pVertices) return;

	Clb184::Point2D* pPoints = nullptr;
	if (m_bDebugDrawEnable) {
		pPoints = (Clb184::Point2D*)(m_PrimBuffer.Lock());
		if (!pPoints) return;
	}

	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		if (pInst->is_delete) { it.MoveFront(); continue; }
		Bullet test = *pInst;
		float c = cos(test.rotation);
		float s = sin(test.rotation);
		glm::vec4 rc = test.uv;

		glm::vec2 fsize = test.size * test.scale;


		glm::vec2 pos = {};

		pos.x = -test.size.x * 0.5f;
		pos.y = -test.size.x * 0.5f;

		float l = pos.x;
		float t = pos.y;

		UINT color = test.color;

		pVertices[(cnt << 2)] = { pos.x * c - pos.y * s + test.pos.x, pos.x * s + pos.y * c + test.pos.y, color, rc.x, rc.y };
		pVertices[(cnt << 2) + 1] = { -pos.x * c - pos.y * s + test.pos.x, -pos.x * s + pos.y * c + test.pos.y, color, rc.z, rc.y };
		pVertices[(cnt << 2) + 2] = { -pos.x * c - -pos.y * s + test.pos.x, -pos.x * s + -pos.y * c + test.pos.y, color, rc.z, rc.w };
		pVertices[(cnt << 2) + 3] = { pos.x * c - -pos.y * s + test.pos.x, pos.x * s + -pos.y * c + test.pos.y, color, rc.x, rc.w };

		if (m_bDebugDrawEnable) {
			pPoints[(cnt * 17)] = { m_Points[0].Pos * test.co_shape.r + test.pos,       0x8c1414ff };
			pPoints[(cnt * 17) + 1] = { m_Points[1].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 2] = { m_Points[2].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 3] = { m_Points[3].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 4] = { m_Points[4].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 5] = { m_Points[5].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 6] = { m_Points[6].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 7] = { m_Points[7].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 8] = { m_Points[8].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 9] = { m_Points[9].Pos * test.co_shape.r + test.pos,   0x8c1414ff };
			pPoints[(cnt * 17) + 10] = { m_Points[10].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 11] = { m_Points[11].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 12] = { m_Points[12].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 13] = { m_Points[13].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 14] = { m_Points[14].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 15] = { m_Points[15].Pos * test.co_shape.r + test.pos, 0x8c1414ff };
			pPoints[(cnt * 17) + 16] = { m_Points[16].Pos * test.co_shape.r + test.pos, 0x8c1414ff };

		}
		cnt++;
		it.MoveFront();
	}
	m_VBuffer.Unlock(sizeof(Clb184::Vertex2D) * 4 * cnt);
	if (m_bDebugDrawEnable)
		m_PrimBuffer.Unlock(sizeof(Clb184::Point2D) * 17 * cnt);

	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
	Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
	m_BulletTexture.BindToContext(0, SHADER_RESOURCE_BIND_PS);
	m_VBuffer.BindToContext(0, 0);
	m_IBuffer.BindToContext(0, 0);
	Clb184::g_pContext->DrawIndexed(cnt * 6, 0, 0);
	m_Count = cnt;
}

void BulletManager::DrawHitbox() {
	if (m_bDebugDrawEnable) {
		Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessVertexShader();
		Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessPixelShader();
		m_PrimBuffer.BindToContext(0, 0);
		for (int i = 0; i < m_Count; i++) {
			Clb184::g_pContext->Draw(17, i * 17);
		}
	}

}

void BulletManager::SetDebugDraw(bool state) {
	m_bDebugDrawEnable = state;
}

void BulletManager::SetTexture(const SQChar* name) {
	if(false == m_BulletTexture.LoadTextureFromFile(name))
		m_BulletTexture.CreateEmptyTexture(256, 256, 0xffffffff);
}

Node<Bullet>* BulletManager::CreateBullet()
{
    return m_TaskList.AddFront();
}
