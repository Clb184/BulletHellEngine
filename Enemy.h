#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED
#include "TaskDrawable2D.h"
#include "HitCalc.h"

#define ENEMY_MAX 1024

struct Enemy : TaskCollideableCircle {
	int life = 100;
};

class EnemyManager : public CDrawableManager<Enemy> {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize();
	void Move();
	void Draw();
	Iterator<Enemy>* GetListIterator();

#ifdef DEBUG
	void SetDebugDraw(bool state);
	void DrawHitbox();
#endif

	int GetItemCnt() const;
	Node<Enemy>* CreateObject();
private:
	Clb184::CTexture** m_ppTextures;

#ifdef DEBUG
	Clb184::CVertexBuffer m_PrimBuffer;
	Clb184::Point2D m_Points[17];
	bool m_bDebugDrawEnable;
	int m_Count;
#endif
};

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
	pTask->co_shape.pos = pTask->pos;
}

SQInteger Enemy_SetLife(HSQUIRRELVM v);
SQInteger Enemy_GetLife(HSQUIRRELVM v);

template<EnemyManager* EnmList>
inline bool RegisterEnemyClass(HSQUIRRELVM v) {
	bool create_class = false;
	sq_pushstring(v, "Enemy", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	EnmMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<Enemy, EnemyManager, EnmList, EnmListSetup, EnmInitialize>, "constructor");
	RegisterSQClassFunc(v, Task_Kill, "Kill");
	RegisterSQClassFunc(v, Task2D_SetPos, "SetPos");
	RegisterSQClassFunc(v, Task2D_SetSize, "SetSize");
	RegisterSQClassFunc(v, Task2D_SetScale, "SetScale");
	RegisterSQClassFunc(v, Task2D_SetUV, "SetUV");
	RegisterSQClassFunc(v, Task2D_SetRotation, "SetRotation");
	RegisterSQClassFunc(v, Collision_SetRadius, "SetRadius");
	RegisterSQClassFunc(v, Enemy_SetLife, "SetLife");
	RegisterSQClassFunc(v, Enemy_GetLife, "GetLife");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif