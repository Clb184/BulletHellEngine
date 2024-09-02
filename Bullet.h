#ifndef BULLET_INCLUDED
#define BULLET_INCLUDED
#include "TaskDrawable2D.h"
#include "HitCalc.h"

#define BULLET_MAX (2048)

struct Bullet : TaskCollideableCircle {
};

extern CMemoryPool<Node<Bullet>> g_TaskBulletPool;

class BulletManager : public CDrawableManager<Bullet> {
public:
	BulletManager();
	~BulletManager();

	void Initialize();
	void Move();
	void Draw();
#ifdef DEBUG
	void DrawHitbox();
	void SetDebugDraw(bool state);
#endif
	void SetTexture(const SQChar* name);
	int GetItemCnt() const;
	Node<Bullet>* CreateObject();
private:
	Clb184::CIndexBuffer m_IBuffer;
	Clb184::CTexture m_BulletTexture;

#ifdef DEBUG
	Clb184::CVertexBuffer m_PrimBuffer;
	Clb184::Point2D m_Points[17];
	bool m_bDebugDrawEnable;
	int m_Count;
#endif
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
	return g_BulletManager.CreateObject();
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
	pTask->co_shape.pos = pTask->pos;
}

inline bool RegisterBulletClass(HSQUIRRELVM v) {
	RegisterSQFunc(v, SetBulletTexture, "SetBulletTexture");

	bool create_class = false;
	sq_pushstring(v, "Bullet", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	BulletMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<Bullet, BulletManager, &g_BulletManager, BulletListSetup, BulletInitialize>, "constructor");
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