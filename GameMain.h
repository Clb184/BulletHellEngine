#ifndef GAMEMAIN_INCLUDED
#define GAMEMAIN_INCLUDED

#include "CManagerBase.h"
#include "Task.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "PlayerShot.h"
#include "Tex.h"
#include "Audio.h"
#include "SqHandlers.h"

class GameMain {
public:
	GameMain();
	~GameMain();

	void Initialize(const char* script);
	void SetPlayerScript(const SQChar* player);
	void Move();
	void Draw();

	bool IsInitialized() const;

#ifdef DEBUG
	void SetDebugDraw(bool state);

private:
	void MoveDebug();
	void DrawDebug();
#endif
	void Reset();

private:
	bool m_bInitialized;
	bool m_bRunOK;
	float m_DeltaTime;
	SQInteger m_GlobalTime;
	HSQUIRRELVM m_VM;

	//EnemyManager m_EnemyManager;
#ifdef DEBUG

	Clb184::CText m_DebugText;
	Clb184::CVertexBuffer m_SBuffer;

	bool m_bDebugDrawEnable;
	bool m_LastPressedInv;
	bool m_LastPressedR;
	bool m_LastPressedHitbox;
	bool m_LastPressedStats;

	bool m_bStatDraw;

#endif
	const char* m_ScriptName;
	const char* m_PlayerScript;
};


#endif