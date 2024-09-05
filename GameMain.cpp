#include "GameMain.h"
CMemoryPool<Node<TaskDrawable2D>> g_TaskDrawable2DPool;
CMemoryPool<Node<TaskDrawable3D>> g_TaskDrawable3DPool;
CMemoryPool<Node<Enemy>> g_TaskEnemyPool; 
CMemoryPool<Node<Bullet>> g_TaskBulletPool; 
CMemoryPool<Node<PlayerShot>> g_TaskPlayerShotPool;

Task2DManager g_FX0;
EnemyManager g_EnmManager;
Task2DManager g_FX1;
Player g_Player;
Task2DManager g_FX2;
PlayerShotManager g_PlayerShotManager;
Task2DManager g_FX3;
BulletManager g_BulletManager;
Task2DManager g_FX4;

GameMain::GameMain() {
	m_GlobalTime = 0.0;
	m_VM = nullptr;
	m_DeltaTime = 1.0f;
	m_bRunOK = false;
	m_bInitialized = false;
#ifdef DEBUG
	m_bDebugDrawEnable = true;
	m_LastPressedHitbox = false;
	m_LastPressedInv = false;
	m_LastPressedR = false;
	m_LastPressedStats = false;
	m_bStatDraw = true;
#endif
}

GameMain::~GameMain() {
	if (m_VM) { 
		sq_collectgarbage(m_VM);
		sq_close(m_VM);
	}
}

static const char* fx0 = _SC("FX0");
static const char* ____fx0 = _SC("____f0");
static const char* fx1 = _SC("FX1");
static const char* ____fx1 = _SC("____f1");
static const char* fx2 = _SC("FX2");
static const char* ____fx2 = _SC("____f2");
static const char* fx3 = _SC("FX3");
static const char* ____fx3 = _SC("____f3");
static const char* fx4 = _SC("FX4");
static const char* ____fx4 = _SC("____f4");

void GameMain::Initialize(const char* filename) {
	m_bInitialized = false;
	m_VM = sq_open(2048);
	sq_pushroottable(m_VM);
	bool math_init = SQ_SUCCEEDED(sqstd_register_mathlib(m_VM));
	bool sys_init = SQ_SUCCEEDED(sqstd_register_systemlib(m_VM));
	sq_setcompilererrorhandler(m_VM, SQCompilerErrorHandler);
	sq_newclosure(m_VM, &SQErrorFunction, 0);
	sq_seterrorhandler(m_VM);
	sq_setprintfunc(m_VM, PrintFn, PrintFn);
	RegisterSQFunc(m_VM, PrintCallStack, "PrintCallStack");
	RegisterEnemyClass<&g_EnmManager>(m_VM);
	RegisterBulletClass<&g_BulletManager>(m_VM);
	RegisterLinearAlgebraClass(m_VM);
	RegisterTextureClass(m_VM);
	RegisterEffectClass<&g_FX0, Task2DListSetup0>(m_VM, _SC("FX0"));
	RegisterEffectClass<&g_FX1, Task2DListSetup1>(m_VM, _SC("FX1"));
	RegisterEffectClass<&g_FX2, Task2DListSetup2>(m_VM, _SC("FX2"));
	RegisterEffectClass<&g_FX3, Task2DListSetup3>(m_VM, _SC("FX3"));
	RegisterEffectClass<&g_FX4, Task2DListSetup4>(m_VM, _SC("FX4"));
	RegisterAudio(m_VM);
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
		CManagerBase<Bullet>::SetVM(m_VM);
		CManagerBase<Bullet>::SetTextureClass(SQGetObjectByName(m_VM, _SC("Texture")));
		CManagerBase<Enemy>::SetVM(m_VM);
		CManagerBase<Enemy>::SetTextureClass(SQGetObjectByName(m_VM, _SC("Texture")));
		CManagerBase<TaskDrawable2D>::SetVM(m_VM);
		CManagerBase<TaskDrawable2D>::SetTextureClass(SQGetObjectByName(m_VM, _SC("Texture")));
		g_FX0.Initialize(_SC("____fx0"));
		g_FX1.Initialize(_SC("____fx1"));
		g_FX2.Initialize(_SC("____fx2"));
		g_FX3.Initialize(_SC("____fx3"));
		g_FX4.Initialize(_SC("____fx4"));
		g_EnmManager.Initialize();
		g_BulletManager.Initialize();
		PrintStack(m_VM);
		sq_pop(m_VM, 1);

		g_Player.Initialize(m_PlayerScript);
		//Finally, initialize script
		CallNPSQFunc(m_VM, "main");
		//m_Music.Load("music/STAGE_00.ogg");
		//m_Music.SetLoop(true);
		//m_Music.Play(0.0f);
		m_ScriptName = filename;
#ifdef DEBUG
		m_DebugText.Setup(512, 512);
		m_DebugText.SetSize(16);
		m_DebugText.SetBold(false);
		uint32_t color = 0xffffffff;
		const float debx = 100.0f;
		const float deby = 20.0f;
		const float debw = 200.0f;
		const float debh = 480.0f;
		Clb184::Vertex2D points[4];
		points[0] = { debx, deby, color, 0.0, 0.0 };
		points[1] = { debx + debw, deby, color, debw / 512.0f, 0.0 };
		points[2] = { debx, deby + debh, color, 0.0, debh / 512.0f };
		points[3] = { debx + debw, deby + debh, color, debw / 512.0f, debh / 512.0f };
		m_SBuffer.Initialize(points, sizeof(points), D3D11_USAGE_DEFAULT, 0);
		m_SBuffer.SetStrideInfo(sizeof(Clb184::Vertex2D));
		MoveDebug();
#endif
		m_bInitialized = true;
	}
}

void GameMain::SetPlayerScript(const SQChar* player) {
	m_PlayerScript = player;
}

void GameMain::Move() {
	if (m_bRunOK) {
		static bool wind = false;
		bool this_wind = GetAsyncKeyState(VK_TAB) & 0x8000;
		if (this_wind && !wind && Clb184::CD3DWindow::GetWindowActive())
			Clb184::CD3DWindow::ToggleFullscreenMode();
		sq_collectgarbage(m_VM);
		if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
			sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		g_FX0.Move();
		g_Player.Move();
		g_FX1.Move();
		g_PlayerShotManager.Move();
		g_FX2.Move();
		g_EnmManager.Move();
		g_FX3.Move();
		g_BulletManager.Move();
		g_FX4.Move();
		wind = this_wind;
#ifdef DEBUG
		MoveDebug();
#endif
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
		g_FX0.Draw();
		g_EnmManager.Draw();
		g_FX1.Draw();
		g_PlayerShotManager.Draw();
		g_FX2.Draw();
		g_Player.Draw();
		g_FX3.Draw();
		g_BulletManager.Draw();
		g_FX4.Draw();
#ifdef DEBUG
		DrawDebug();
#endif
		Clb184::CDefault2DShader::GetShaderInstance().SetCenter({ 0.0, 0.0 });
		Clb184::CDefault2DShader::GetShaderInstance().UpdateMatrix();
	}
}


bool GameMain::IsInitialized() const {
	return m_bInitialized;
}

#ifdef DEBUG
void GameMain::SetDebugDraw(bool state) {
	m_bDebugDrawEnable = state;
}

void GameMain::MoveDebug() {
	bool this_hitbox = GetAsyncKeyState(VK_F2) & 0x8000;
	bool this_stats = GetAsyncKeyState(VK_F1) & 0x8000;
	bool this_inv = GetAsyncKeyState(VK_F4) & 0x8000;
	bool this_reset = GetAsyncKeyState('R') & 0x8000;
	if (this_hitbox && !m_LastPressedHitbox) {
		m_bDebugDrawEnable = !m_bDebugDrawEnable;
		g_EnmManager.SetDebugDraw(m_bDebugDrawEnable);
		g_BulletManager.SetDebugDraw(m_bDebugDrawEnable);
		g_PlayerShotManager.SetDebugDraw(m_bDebugDrawEnable);
		g_Player.SetDebugDraw(m_bDebugDrawEnable);
	}
	if (this_stats && !m_LastPressedStats)
		m_bStatDraw = !m_bStatDraw;
	if (this_inv && !m_LastPressedInv)
		g_Player.m_IsInvincible = !g_Player.m_IsInvincible;
	if (this_reset && !m_LastPressedR)
		Reset();
	char buf[2048] = "";
	sprintf_s(buf,
		"Enemy		:%8d\n"
		"Bullet		:%8d\n"
		"PlayerShot	:%8d\n"
		"Player		:%8s\n"
		"FX0		:%8d\n"
		"FX1		:%8d\n"
		"FX2		:%8d\n"
		"FX3		:%8d\n"
		"FX4		:%8d\n"
		"\0",
		g_EnmManager.GetItemCnt(),
		g_BulletManager.GetItemCnt(),
		g_PlayerShotManager.GetItemCnt(),
		(g_Player.m_IsInvincible || g_Player.m_MutekiTime > 0) ? "Invi" : "Vuln",
		g_FX0.GetItemCnt(),
		g_FX1.GetItemCnt(),
		g_FX2.GetItemCnt(),
		g_FX3.GetItemCnt(),
		g_FX4.GetItemCnt()
	);
	m_DebugText.SetText(buf);
	m_DebugText.Update();
	m_LastPressedInv = this_inv;
	m_LastPressedR = this_reset;
	m_LastPressedHitbox = this_hitbox;
	m_LastPressedStats = this_stats;
}

void GameMain::DrawDebug() {
	if (m_bDebugDrawEnable) {
		g_EnmManager.DrawHitbox();
		g_BulletManager.DrawHitbox();
		g_PlayerShotManager.DrawHitbox();
		g_Player.DrawHitbox();
	}
	if (m_bStatDraw) {
		Clb184::CDefault2DShader::GetShaderInstance().SetCenter({ 0.0, 0.0 });
		Clb184::CDefault2DShader::GetShaderInstance().UpdateMatrix();
		D3D11_RECT rc = { 0.0, 0.0, 640.0, 480.0 };
		Clb184::g_pContext->RSSetScissorRects(1, &rc);
		Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
		Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
		m_SBuffer.BindToContext(0, 0);
		m_DebugText.BindToContext(0, SHADER_RESOURCE_BIND_PS);
		Clb184::g_pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Clb184::g_pContext->Draw(4, 0);
	}
}
#endif

void GameMain::Reset() {
	if (m_VM) {
		sq_close(m_VM);
		Initialize(m_ScriptName);
	}
}
