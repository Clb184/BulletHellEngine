#include "Enemy.h"

static CMemoryPool<Node<Enemy>> g_TaskEnemyPool;
EnemyManager g_EnmManager;

EnemyManager::EnemyManager() :
	m_TaskList(ENEMY_MAX, &g_TaskEnemyPool)
{

}

EnemyManager::~EnemyManager() {

}

void EnemyManager::Initialize() {

}


void EnemyManager::Move() {
	if (!m_TaskList.GetSize())
		return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		Enemy test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList);

		it.MoveFront();
	}
}

void EnemyManager::Draw() {
	static Clb184::CTexture* tex = nullptr;
	static Clb184::CD3DSprite spr;
	if (!m_TaskList.GetSize())
		return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	sq_pushroottable(m_VM);
	sq_pushstring(m_VM, _SC("Texture"), -1);
	sq_get(m_VM, -2);
	sq_pushstring(m_VM, _SC("____enm"), -1);
	sq_get(m_VM, -3);
	while ((pInst = it.GetData()) && pInst->active) {
		if (pInst->is_delete) continue;
		Enemy test = *pInst;
		spr.SetPos(test.pos);
		spr.SetRotation(test.rotation);
		spr.SetSize(test.size);
		spr.SetScale(test.scale);
		spr.SetUV(test.uv);
		sq_pushinteger(m_VM, pInst->id);
		sq_get(m_VM, -2);
		sq_move(m_VM, m_VM, -3);
		sq_pushstring(m_VM, _SC("texture"), -1);
		sq_get(m_VM, -3);
		if (SQTrue == sq_instanceof(m_VM)) {
			sq_getinstanceup(m_VM, -1, (SQUserPointer*)(&tex), NULL);
			tex->BindToContext(0, SHADER_RESOURCE_BIND_PS);
		}
		else {
			ID3D11ShaderResourceView* p = nullptr;
			Clb184::g_pContext->PSSetShaderResources(0, 1, &p);
		}
		spr.Draw();
		it.MoveFront();
		sq_pop(m_VM, 3);
	}
	sq_pop(m_VM, 3);
}

Node<Enemy>* EnemyManager::CreateEnemy() {
	return m_TaskList.AddFront();
}
