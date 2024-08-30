#include "Enemy.h"
#include "Player.h"

static CMemoryPool<Node<Enemy>> g_TaskEnemyPool;
EnemyManager g_EnmManager;

EnemyManager::EnemyManager() :
	m_TaskList(ENEMY_MAX, &g_TaskEnemyPool)
{
	m_ppTextures = new Clb184::CTexture * [ENEMY_MAX];
}

EnemyManager::~EnemyManager() {
	if (m_ppTextures) delete[] m_ppTextures;
}

void EnemyManager::Initialize() {
	if (m_VM) {
		m_VBuffer.Initialize(nullptr, sizeof(Clb184::Vertex2D) * ENEMY_MAX * 4, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		sq_pushroottable(m_VM);
		m_ArrayObj = SQCreateArray(m_VM, _SC("____enm"), ENEMY_MAX);
		sq_pop(m_VM, 1);
	}
}


void EnemyManager::Move() {
	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	CollissionCircle phit = g_Player.co_shape;
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		//Enemy test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList, m_ArrayObj);
		pInst->co_shape.pos = pInst->pos;
		if (hitCheckCC(phit, pInst->co_shape)) {
			g_Player.Kill();
			//sq_getprintfunc(m_VM)(m_VM, "hit!\n");
		}
		it.MoveFront();
	}
}

void EnemyManager::Draw() {
	int cnt = 0;

	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	if (!pVertices) return;

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
			cnt++;
		}
		it.MoveFront();
		sq_pop(m_VM, 3);
	}
	m_VBuffer.Unlock(sizeof(Clb184::Vertex2D) * 4 * cnt);
	sq_pop(m_VM, 1);
	Clb184::CTexture* past = nullptr;

	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_VBuffer.BindToContext(0,0);
	for (int i = 0; i < cnt; i++) {
		if (m_ppTextures[i] != past) {
			m_ppTextures[i]->BindToContext(0, SHADER_RESOURCE_BIND_PS);
			past = m_ppTextures[i];
		}
		Clb184::g_pContext->Draw(4, i << 2);
	}
}

Node<Enemy>* EnemyManager::CreateEnemy() {
	return m_TaskList.AddFront();
}
