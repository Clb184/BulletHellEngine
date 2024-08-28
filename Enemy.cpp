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
	if (m_VM) {
		sq_pushroottable(m_VM);
		m_ArrayObj = SQCreateArray(m_VM, _SC("____enm"), ENEMY_MAX);
		sq_pop(m_VM, 1);
	}
}


void EnemyManager::Move() {
	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	while ((pInst = it.GetData()) && pInst->active) {
		//Enemy test = *pInst;
		MoveTask(m_VM, pInst, &m_TaskList);

		it.MoveFront();
	}
}

void EnemyManager::Draw() {
	static Clb184::CD3DSprite spr;
	Clb184::CTexture* tex = nullptr;

	if (!m_TaskList.GetSize() || !m_VM) return;

	Node<Enemy>* pInst;
	Iterator<Enemy> it = *m_TaskList.GetBackIterator();
	it.MoveFront();
	//printf("begin\n");
	//PrintStack(m_VM);
	sq_pushobject(m_VM, m_ArrayObj);
	//assert(m_TaskList.GetSize() < 10);
	while ((pInst = it.GetData()) && pInst->active) {
		if (pInst->is_delete) continue;
		Enemy test = *pInst;
		spr.SetPos(test.pos);
		spr.SetRotation(test.rotation);
		spr.SetSize(test.size);
		spr.SetScale(test.scale);
		spr.SetUV(test.uv);
		//PrintStack(m_VM);
		sq_pushinteger(m_VM, pInst->id);
		//PrintStack(m_VM);
		sq_get(m_VM, -2);
		//PrintStack(m_VM);
		sq_pushobject(m_VM, m_TextureObj);
		//PrintStack(m_VM);

		sq_pushstring(m_VM, _SC("texture"), -1);
		sq_get(m_VM, -3);
		PrintStack(m_VM);
		if (SQTrue == sq_instanceof(m_VM)) {
			sq_getinstanceup(m_VM, -1, (SQUserPointer*)(&tex), NULL);
			tex->BindToContext(0, SHADER_RESOURCE_BIND_PS);
			spr.Draw();
		}
		it.MoveFront();
		//PrintStack(m_VM);
		//printf("mid\n");
		sq_pop(m_VM, 3);
	}
	//PrintStack(m_VM);
	sq_pop(m_VM, 1);
}

Node<Enemy>* EnemyManager::CreateEnemy() {
	return m_TaskList.AddFront();
}
