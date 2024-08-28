#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED
#include "Task.h"
#include "HitCalc.h"

#define ENEMY_MAX 1024

struct Enemy : TaskCollideableCircle {
};

extern CMemoryPool<Node<Enemy>> g_TaskEnemyPool;

class EnemyManager : public CDrawableManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize();
	void Move();
	void Draw();
	Node<Enemy>* CreateEnemy();
private:
	CDoubleLinkedArrayList<Enemy> m_TaskList;
	Clb184::CTexture** m_ppTextures;
};

extern EnemyManager g_EnmManager;

static Node<Enemy>* CreateEnemy() {
	return g_EnmManager.CreateEnemy();
}

inline void EnmMemberSetup(HSQUIRRELVM v) {
	Task2DMemberSetup(v);
}

static void EnmListSetup(HSQUIRRELVM v, Node<Enemy>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____enm"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}

static void EnmInitialize(HSQUIRRELVM v, Node<Enemy>* pTask) {
	Task2DInitialize(v, (Node<TaskDrawable2D>*)pTask);
}

inline bool RegisterEnemyClass(HSQUIRRELVM v) {
	bool create_class = false;
	sq_pushstring(v, "Enemy", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	EnmMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<Enemy, CreateEnemy, EnmListSetup, EnmInitialize>, "constructor");
	RegisterSQClassFunc(v, Task_Kill, "Kill");
	RegisterSQClassFunc(v, Task2D_SetPos, "SetPos");
	RegisterSQClassFunc(v, Task2D_SetSize, "SetSize");
	RegisterSQClassFunc(v, Task2D_SetScale, "SetScale");
	RegisterSQClassFunc(v, Task2D_SetUV, "SetUV");
	RegisterSQClassFunc(v, Task2D_SetRotation, "SetRotation");
	RegisterSQClassFunc(v, Collision_SetRadius, "SetRadius");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif