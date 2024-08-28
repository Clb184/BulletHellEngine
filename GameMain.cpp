#include "GameMain.h"

GameMain::GameMain() {
	m_GlobalTime = 0.0;
	m_VM = nullptr;
	m_DeltaTime = 1.0f;
	m_bRunOK = false;
}

GameMain::~GameMain() {
	if (m_VM) { 
		sq_collectgarbage(m_VM);
		sq_close(m_VM);
	}
}

void GameMain::Initialize(const char* filename) {
	m_VM = sq_open(2048);
	sq_pushroottable(m_VM);
	bool math_init = SQ_SUCCEEDED(sqstd_register_mathlib(m_VM));
	bool sys_init = SQ_SUCCEEDED(sqstd_register_systemlib(m_VM));
	sq_setcompilererrorhandler(m_VM, SQCompilerErrorHandler);
	sq_newclosure(m_VM, &SQErrorFunction, 0);
	sq_seterrorhandler(m_VM);
	sq_setprintfunc(m_VM, PrintFn, PrintFn);
	/*
	sq_pushstring(m_VM, _SC("____enm"), -1);
	sq_newarray(m_VM, ENEMY_MAX);
	sq_newslot(m_VM, -3, SQFalse);*/
	RegisterSQFunc(m_VM, PrintCallStack, "PrintCallStack");
	RegisterEnemyClass(m_VM);
	RegisterLinearAlgebraClass(m_VM);
	RegisterTextureClass(m_VM);
	sq_pop(m_VM, 1);
	CompileMemSQScript(
		" function wait(t) {              "
		"local c = (t >= 0) ? t : 0;	  "
		"								  "
		"while (c--) { ::suspend(); };	  "
		"}", m_VM);
	CompileMemSQScript(
		" function waitd(t) {              "
		"local c = (t >= 0) ? t : 0;	  "
		"								  "
		"while (c--) { print(c + \"frames\\n\"); ::suspend(); };	  "
		"}", m_VM);

	if (m_bRunOK = CompileSQScript(filename, m_VM)) {
		CallNPSQFunc(m_VM, "main");
		m_Music.Load("music/ex_3.ogg");
		m_Music.SetLoop(true);
		m_Music.Play(0.0f);

		//Initialize managers
		sq_pushroottable(m_VM);
		CManagerBase::SetVM(m_VM);
		CManagerBase::SetTextureClass(SQGetObjectByName(m_VM, _SC("Texture")));
		g_EnmManager.Initialize();
		sq_pop(m_VM, 1);
	}
}

void GameMain::Move() {
	if (m_bRunOK) {
		sq_collectgarbage(m_VM);
		if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
			sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		m_TaskManager.Move();
		m_Task2DManager.Move();
		g_EnmManager.Move();
	}
}

void GameMain::Draw() {
	if (m_bRunOK) {
		m_Task2DManager.Draw();
		g_EnmManager.Draw();
	}
	DrawDebug();
}

void GameMain::DrawDebug() {

}
