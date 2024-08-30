#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED
#include "Task.h"
#include "Tex.h"
#include "HitCalc.h"
#include <Xinput.h>

//The idea is to split both main script VMs, so we don't use certain function we shouldn't been using
class Player : protected TaskCollideableCircle{
public:
	Player();
	~Player();

	void Initialize(const SQChar* fname);
	void Move();
	void Draw();
	void SetTexture(const SQChar* name);
	void SetUV(const glm::vec4 uv);
	void Kill();
	const CollissionCircle& GetCollisionShape();

private:
	bool m_bRunOK;
	bool m_IsDead;
	XINPUT_STATE m_GamePadState;
	HSQUIRRELVM m_VM;
	HSQOBJECT m_TextureObj;
	Clb184::CVertexBuffer m_VBuffer;
	Clb184::CTexture m_PlayerTexture;
};

extern Player g_Player;

inline void PlayerSetup(HSQUIRRELVM v) {
	Task2DMemberSetup(v);
}

static SQInteger SetPlayerTexture(HSQUIRRELVM v) {
	const SQChar* pName;
	if ((sq_gettop(v) == 2) && (SQ_SUCCEEDED(sq_getstring(v, -1, &pName)))) {
		g_Player.SetTexture(pName);
	}
	return 0;
}

SQInteger Player_SetPos(HSQUIRRELVM v);
SQInteger Player_SetSize(HSQUIRRELVM v);
SQInteger Player_SetScale(HSQUIRRELVM v);
SQInteger Player_SetRotation(HSQUIRRELVM v);
SQInteger Player_SetUV(HSQUIRRELVM v);

SQInteger Player_SetRadius(HSQUIRRELVM v);

inline bool RegisterPlayerClass(HSQUIRRELVM v) {
	return true;
}

#endif