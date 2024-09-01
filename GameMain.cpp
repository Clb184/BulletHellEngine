#include "GameMain.h"

GameMain::GameMain() {
	m_GlobalTime = 0.0;
	m_VM = nullptr;
	m_DeltaTime = 1.0f;
	m_bRunOK = false;
	m_bInitialized = false;
#ifdef DEBUG
	m_bDebugDrawEnable = true;
#endif
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
		sq_collectgarbage(m_VM);
		if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
			sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQFalse);
		}
		g_Player.Move();
		g_PlayerShotManager.Move();
		g_EnmManager.Move();
		g_BulletManager.Move();
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
		g_EnmManager.Draw();
		g_PlayerShotManager.Draw();
		g_Player.Draw();
		g_BulletManager.Draw();
#ifdef DEBUG
		DrawDebug();
#endif
		Clb184::CDefault2DShader::GetShaderInstance().SetCenter({ 0.0, 0.0 });
		Clb184::CDefault2DShader::GetShaderInstance().UpdateMatrix();
	}
}

void GameMain::SetDebugDraw(bool state) {
	m_bDebugDrawEnable = state;
}

bool GameMain::IsInitialized() const {
	return m_bInitialized;
}

#ifdef DEBUG
void GameMain::MoveDebug() {
	bool this_hitbox = GetAsyncKeyState(VK_F2) & 0x8000;
	bool this_inv = GetAsyncKeyState(VK_F4) & 0x8000;
	bool this_reset = GetAsyncKeyState('R') & 0x8000;
	if (this_hitbox && !m_LastPressedHitbox) {
		m_bDebugDrawEnable = !m_bDebugDrawEnable;
		g_EnmManager.SetDebugDraw(m_bDebugDrawEnable);
		g_BulletManager.SetDebugDraw(m_bDebugDrawEnable);
		g_PlayerShotManager.SetDebugDraw(m_bDebugDrawEnable);
		g_Player.SetDebugDraw(m_bDebugDrawEnable);
	}
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
		"\0",
		g_EnmManager.GetItemCnt(),
		g_BulletManager.GetItemCnt(),
		g_PlayerShotManager.GetItemCnt(),
		(g_Player.m_IsInvincible || g_Player.m_MutekiTime > 0) ? "Invi" : "Vuln"
	);
	m_DebugText.SetText(buf);
	m_DebugText.Update();
	m_LastPressedInv = this_inv;
	m_LastPressedR = this_reset;
	m_LastPressedHitbox = this_hitbox;
}

void GameMain::DrawDebug() {
	g_EnmManager.DrawHitbox();
	g_BulletManager.DrawHitbox();
	g_PlayerShotManager.DrawHitbox();
	g_Player.DrawHitbox();

	if (m_bDebugDrawEnable) {
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
