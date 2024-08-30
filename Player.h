#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED
#include "Task.h"
#include "Tex.h"
#include "HitCalc.h"
#include <Xinput.h>

//The idea is to split both main script VMs, so we don't use certain function we shouldn't been using
struct Player : TaskCollideableCircle{
	bool m_bEnableMove;
	bool m_IsDead;
	bool m_IsInvincible;
	SQInteger m_WaitNextLife;
	SQInteger m_MutekiTime;

public:
	Player();
	~Player();

	void Initialize(const SQChar* fname);
	void Move();
	void Draw();
	void Kill();
	void SetDebugDraw(bool state);
	void DrawHitbox();

private:
	bool m_bRunOK;
	HSQUIRRELVM m_VM;
	HSQOBJECT m_TextureObj;
	Clb184::CVertexBuffer m_VBuffer;
	Clb184::CTexture m_PlayerTexture;

	bool m_bDebugDrawEnable;
	glm::vec2 m_Points[17];
	Clb184::CVertexBuffer m_PrimBuffer;
};

extern Player g_Player;

SQInteger Player_SetPos(HSQUIRRELVM v);
SQInteger Player_SetSize(HSQUIRRELVM v);
SQInteger Player_SetScale(HSQUIRRELVM v);
SQInteger Player_SetRotation(HSQUIRRELVM v);
SQInteger Player_SetUV(HSQUIRRELVM v);
SQInteger Player_SetColor(HSQUIRRELVM v);
SQInteger Player_SetMutekiTime(HSQUIRRELVM v);
SQInteger Player_SetRadius(HSQUIRRELVM v);
SQInteger Player_EnableMove(HSQUIRRELVM v);

inline bool RegisterPlayerClass(HSQUIRRELVM v) {
	return true;
}

#endif