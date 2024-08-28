#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED
#include "Task.h"
#include "HitCalc.h"

//The idea is to split both main script VMs, so we don't use certain function we shouldn't been using
class Player : protected TaskCollideableCircle{
public:
	Player();
	~Player();

	void SetVM(HSQUIRRELVM v);
	void Initialize();
	void Move();
	void Draw();
private:
	HSQUIRRELVM m_VM;
};

extern Player g_Player;

inline void PlayerSetup(HSQUIRRELVM v) {
	Task2DMemberSetup(v);
}

inline bool RegisterPlayerClass(HSQUIRRELVM v) {
	return true;
}

#endif