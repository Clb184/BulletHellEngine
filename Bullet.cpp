#include "Bullet.h"
#include "Player.h"

static CMemoryPool<Node<Bullet>> g_TaskBulletPool;
BulletManager g_BulletManager;

BulletManager::BulletManager() :
	m_TaskList(BULLET_MAX, &g_TaskBulletPool)
{
}

BulletManager::~BulletManager() {

}

void BulletManager::Initialize() {
	if (m_VM) {
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
	}
}

void BulletManager::Move() {
	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Bullet>* pInst;
	Iterator<Bullet> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		//Enemy test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
		pInst->co_shape.pos = pInst->pos;
		if (hitCheckCC(g_Player.GetCollisionShape(), pInst->co_shape)) {
			pInst->color = 0xff0000ff;
			//sq_getprintfunc(m_VM)(m_VM, "hit!\n");
		}
		it.MoveFront();
	}
}

void BulletManager::Draw() {
	int cnt = 0;

	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Bullet>* pInst;
	Iterator<Bullet> it = *m_TaskList.GetBackIterator();

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	if (!pVertices) return;

	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		if (pInst->is_delete) continue;
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
		cnt++;
		it.MoveFront();
	}
	m_VBuffer.Unlock(sizeof(Clb184::Vertex2D) * 4 * cnt);
	Clb184::CTexture* past = nullptr;

	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_BulletTexture.BindToContext(0, SHADER_RESOURCE_BIND_PS);
	m_VBuffer.BindToContext(0, 0);
	m_IBuffer.BindToContext(0, 0);
	Clb184::g_pContext->DrawIndexed(cnt * 6, 0, 0);
}

void BulletManager::SetTexture(const SQChar* name) {
	if(false == m_BulletTexture.LoadTextureFromFile(name))
		m_BulletTexture.CreateEmptyTexture(256, 256, 0xffffffff);
}

Node<Bullet>* BulletManager::CreateBullet()
{
    return m_TaskList.AddFront();
}
