#include "GameMain.h"

GameMain::GameMain() {
	m_GlobalTime = 0.0;
	m_VM = nullptr;
	m_DeltaTime = 1.0f;
	m_bRunOK = false;
	m_bInitialized = false;
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
	RegisterBulletClass(m_VM);
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
		//Initialize managers
		sq_pushroottable(m_VM);
		CManagerBase::SetVM(m_VM);
		CManagerBase::SetTextureClass(SQGetObjectByName(m_VM, _SC("Texture")));
		g_EnmManager.Initialize();
		g_BulletManager.Initialize();
		PrintStack(m_VM);
		sq_pop(m_VM, 1);

		g_Player.Initialize(m_PlayerScript);

		//Finally, initialize script
		CallNPSQFunc(m_VM, "main");
		m_Music.Load("music/STAGE_00.ogg");
		m_Music.SetLoop(true);
		//m_Music.Play(0.0f);
		m_ScriptName = filename;
		m_bInitialized = true;
	}
}

void GameMain::SetPlayerScript(const SQChar* player) {
	m_PlayerScript = player;
}

void GameMain::Move() {
	if (m_bRunOK) {
		sq_collectgarbage(m_VM);
		if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
			sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		MoveDebug();
		m_TaskManager.Move();
		m_Task2DManager.Move();
		g_Player.Move();
		g_PlayerShotManager.Move();
		g_EnmManager.Move();
		g_BulletManager.Move();
	}
}

void GameMain::Draw() {
	if (m_bRunOK) {
		D3D11_RECT rc{ 
			120.0, 0.0,
			120.0 + 400.0, 480.0
		};
		Clb184::g_pContext->RSSetScissorRects(1, &rc);
		Clb184::CDefault2DShader::GetShaderInstance().SetCenter({ 320.0, 0.0 });
		Clb184::CDefault2DShader::GetShaderInstance().UpdateMatrix();
		m_Task2DManager.Draw();
		g_EnmManager.Draw();
		g_PlayerShotManager.Draw();
		g_Player.Draw();
		g_BulletManager.Draw();
		DrawDebug();
		Clb184::CDefault2DShader::GetShaderInstance().SetCenter({ 0.0, 0.0 });
		Clb184::CDefault2DShader::GetShaderInstance().UpdateMatrix();
	}
}

bool GameMain::IsInitialized() const {
	return m_bInitialized;
}

void GameMain::MoveDebug() {
	static bool draw_enable = true;
	bool this_hitbox = GetAsyncKeyState(VK_F2) & 0x8000;
	bool this_inv = GetAsyncKeyState(VK_F4) & 0x8000;
	bool this_reset = GetAsyncKeyState('R') & 0x8000;
	if (this_hitbox && !m_LastPressedHitbox) {
		draw_enable = !draw_enable;
		g_EnmManager.SetDebugDraw(draw_enable);
		g_BulletManager.SetDebugDraw(draw_enable);
		g_PlayerShotManager.SetDebugDraw(draw_enable);
		g_Player.SetDebugDraw(draw_enable);
	}
	if (this_inv && !m_LastPressedInv)
		g_Player.m_IsInvincible = !g_Player.m_IsInvincible;
	if (this_reset && !m_LastPressedR)
		Reset();
	m_LastPressedInv = this_inv;
	m_LastPressedR = this_reset;
	m_LastPressedHitbox = this_hitbox;

}

void GameMain::DrawDebug() {
	g_EnmManager.DrawHitbox();
	g_BulletManager.DrawHitbox();
	g_PlayerShotManager.DrawHitbox();
	g_Player.DrawHitbox();
}

void GameMain::Reset() {
	if (m_VM) {
		sq_close(m_VM);
		Initialize(m_ScriptName);
	}
}
