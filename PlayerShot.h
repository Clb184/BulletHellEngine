#ifndef PLAYERSHOT_INCLUDED
#define PLAYERSHOT_INCLUDED
#include "TaskDrawable2D.h"
#include "HitCalc.h"

#define PLAYERSHOT_MAX (512)

struct PlayerShot : TaskCollideableCircle {
	int damage = 1;
};


class PlayerShotManager : public CDrawableManager<PlayerShot> {
public:
	PlayerShotManager();
	~PlayerShotManager();

	void Initialize();
	void Move();
	void Draw();
#ifdef DEBUG
	void DrawHitbox();
	void SetDebugDraw(bool state);
#endif
	void SetTexture(const SQChar* name);
	void SetShotVM(HSQUIRRELVM v);
	int GetItemCnt() const;
	Node<PlayerShot>* CreateObject();
private:
	Clb184::CIndexBuffer m_IBuffer;
	Clb184::CTexture m_ShotTexture;

#ifdef DEBUG
	Clb184::CVertexBuffer m_PrimBuffer;
	glm::vec2 m_Points[17];
	bool m_bDebugDrawEnable;
	int m_Count;
#endif
};

template<PlayerShotManager* TaskList>
static SQInteger SetPlayerShotTexture(HSQUIRRELVM v) {
	const SQChar* pName;
	if ((sq_gettop(v) == 2) && (SQ_SUCCEEDED(sq_getstring(v, -1, &pName)))) {
		TaskList->SetTexture(pName);
	}
	return 0;
}

inline void PlayerShotMemberSetup(HSQUIRRELVM v) {
	TaskMemberSetup(v);
}

static void PlayerShotListSetup(HSQUIRRELVM v, Node<PlayerShot>* pTask) {

	sq_pushroottable(v);
	sq_pushstring(v, _SC("____sht"), -1);
	sq_get(v, -2);
	sq_pushinteger(v, pTask->id);
	sq_move(v, v, 1);
	sq_set(v, -3);
	sq_pop(v, 2);
}

static void PlayerShotInitialize(HSQUIRRELVM v, Node<PlayerShot>* pTask) {
	Task2DInitialize(v, (Node<TaskDrawable2D>*)pTask);
	pTask->co_shape.pos = pTask->pos;
}

SQInteger PlayerShot_SetDamage(HSQUIRRELVM v);

template <PlayerShotManager* PlayerShotList>
inline bool RegisterPlayerShotClass(HSQUIRRELVM v) {
	RegisterSQFunc(v, SetPlayerShotTexture<PlayerShotList>, "SetPlayerShotTexture");

	bool create_class = false;
	sq_pushstring(v, "PlayerShot", -1);
	create_class = SQ_SUCCEEDED(sq_newclass(v, FALSE));
	PlayerShotMemberSetup(v);
	RegisterSQClassFunc(v, Task_Constructor<PlayerShot, PlayerShotManager, PlayerShotList, PlayerShotListSetup, PlayerShotInitialize>, "constructor");
	RegisterSQClassFunc(v, Task_Kill, "Kill");
	RegisterSQClassFunc(v, Task2D_SetPos, "SetPos");
	RegisterSQClassFunc(v, Task2D_SetSize, "SetSize");
	RegisterSQClassFunc(v, Task2D_SetScale, "SetScale");
	RegisterSQClassFunc(v, Task2D_SetUV, "SetUV");
	RegisterSQClassFunc(v, Task2D_SetRotation, "SetRotation");
	RegisterSQClassFunc(v, Collision_SetRadius, "SetRadius");
	RegisterSQClassFunc(v, PlayerShot_SetDamage, "SetDamage");
	create_class &= SQ_SUCCEEDED(sq_newslot(v, -3, SQFalse));
	return create_class;
}

#endif