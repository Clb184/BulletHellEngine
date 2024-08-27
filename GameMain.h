#ifndef GAMEMAIN_INCLUDED
#define GAMEMAIN_INCLUDED

#include "CManagerBase.h"
#include "Task.h"
#include "Enemy.h"
#include "Tex.h"
#include "SqHandlers.h"

class GameMain {
public:
	GameMain();
	~GameMain();

	void Initialize(const char* script);
	void Move();
	void Draw();
private:
	void DrawDebug();

private:
	bool m_bRunOK;
	float m_DeltaTime;
	SQInteger m_GlobalTime;
	HSQUIRRELVM m_VM;

	TaskManager m_TaskManager;
	Task2DManager m_Task2DManager;
	//EnemyManager m_EnemyManager;
};


#endif