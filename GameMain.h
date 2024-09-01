#ifndef GAMEMAIN_INCLUDED
#define GAMEMAIN_INCLUDED

#include "CManagerBase.h"
#include "Task.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "PlayerShot.h"
#include "Tex.h"
#include "SqHandlers.h"

class GameMain {
public:
	GameMain();
	~GameMain();

	void Initialize(const char* script);
	void SetPlayerScript(const SQChar* player);
	void Move();
	void Draw();

	void SetDebugDraw(bool state);

	bool IsInitialized() const;

private:
#ifdef DEBUG
	void MoveDebug();
	void DrawDebug();
	void Reset();
#endif
private:
	bool m_bInitialized;
	bool m_bRunOK;
	float m_DeltaTime;
	SQInteger m_GlobalTime;
	HSQUIRRELVM m_VM;

	Clb184::CMusicStream m_Music;
	//EnemyManager m_EnemyManager;
#ifdef DEBUG

	Clb184::CText m_DebugText;
	Clb184::CVertexBuffer m_SBuffer;

	bool m_bDebugDrawEnable;
	bool m_LastPressedInv;
	bool m_LastPressedR;
	bool m_LastPressedHitbox;

#endif
	const char* m_ScriptName;
	const char* m_PlayerScript;
};


#endif