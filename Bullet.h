#ifndef BULLET_INCLUDED
#define BULLET_INCLUDED
#include "Task.h"
#include "HitCalc.h"

#define BULLET_MAX (2048 * 16)

struct Bullet : TaskCollideableCircle {
};

extern CMemoryPool<Node<Bullet>> g_TaskBulletPool;

class BulletManager : public CDrawableManager {
public:
	BulletManager();
	~BulletManager();

	void Initialize();
	void Move();
	void Draw();
	void SetTexture(const SQChar* name);
	Node<Bullet>* CreateBullet();
private:
	CDoubleLinkedArrayList<Bullet> m_TaskList;
	Clb184::CTexture m_BulletTexture;
};

extern BulletManager g_BulletManager;

static SQInteger SetBulletTexture(HSQUIRRELVM v) {
	const SQChar* pName;
	if ((sq_gettop(v) == 2) && (SQ_SUCCEEDED(sq_getstring(v, -1, &pName)))) {
		g_BulletManager.SetTexture(pName);
	}
	return 0;
}

static Node<Bullet>* CreateBullet() {
	return g_BulletManager.CreateBullet();
}

inline void BulletMemberSetup(HSQUIRRELVM v) {
	TaskMemberSetup(v);
}

static void BulletListSetup(HSQUIRRELVM v, Node<Bullet>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____blt"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}

static void BulletInitialize(HSQUIRRELVM v, Node<Bullet>* pTask) {
	Task2DInitialize(v, (Node<TaskDrawable2D>*)pTask);
}

inline bool RegisterBulletClass(HSQUIRRELVM v) {
	RegisterSQFunc(v, SetBulletTexture, "SetBulletTexture");

	bool create_class = false;
	sq_pushstring(v, "Bullet", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	BulletMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<Bullet, CreateBullet, BulletListSetup, BulletInitialize>, "constructor");
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