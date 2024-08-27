#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED
#include "Task.h"
#include "HitCalc.h"

#define ENEMY_MAX 1024

struct Enemy : TaskDrawable2D {
	CollissionCircle co_shape;
};

extern CMemoryPool<Node<Enemy>> g_TaskEnemyPool;

class EnemyManager : public CManagerBase {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize();
	void Move();
	void Draw();
	Node<Enemy>* CreateEnemy();
private:
	CDoubleLinkedArrayList<Enemy> m_TaskList;
};

extern EnemyManager g_EnmManager;

static Node<Enemy>* CreateEnemy() {
	return g_EnmManager.CreateEnemy();
}


inline bool RegisterEnemyClass(HSQUIRRELVM v) {
	RegisterSQFunc(v, TaskInit, "TaskInit");

	bool create_class = false;
	sq_pushstring(v, "Enemy", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	sq_pushstring(v, _SC("____task"), -1);
	sq_pushnull(v);
	sq_newslot(v, -3, SQFalse);
	sq_pushstring(v, _SC("texture"), -1);
	sq_pushnull(v);
	sq_newslot(v, -3, SQFalse);
	RegisterSQClassFunc(v, Task_Constructor<Enemy, CreateEnemy, ListRelease<Enemy>>, "constructor");
	RegisterSQClassFunc(v, Task_Kill, "Kill");
	RegisterSQClassFunc(v, Task2D_SetPos, "SetPos");
	RegisterSQClassFunc(v, Task2D_SetSize, "SetSize");
	RegisterSQClassFunc(v, Task2D_SetScale, "SetScale");
	RegisterSQClassFunc(v, Task2D_SetUV, "SetUV");
	RegisterSQClassFunc(v, Task2D_SetRotation, "SetRotation");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif