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

struct GameMainBody {
	EnemyManager EnmMan;
	BulletManager BullMan;
	PlayerShotManager PlShtMan;
};


extern CMemoryPool<Node<TaskDrawable2D>> g_TaskDrawable2DPool;
extern CMemoryPool<Node<TaskDrawable3D>> g_TaskDrawable3DPool;
extern CMemoryPool<Node<PlayerShot>> g_TaskPlayerShotPool;
extern CMemoryPool<Node<Bullet>> g_TaskBulletPool;
extern CMemoryPool<Node<Enemy>> g_TaskEnemyPool;


extern Task2DManager g_FX0;
extern Task2DManager g_FX1;
extern Task2DManager g_FX2;
extern Task2DManager g_FX3;
extern Task2DManager g_FX4;
extern EnemyManager g_EnmManager;
extern Player g_Player;
extern PlayerShotManager g_PlayerShotManager;

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

	EnemyManager m_EnemyManager;
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