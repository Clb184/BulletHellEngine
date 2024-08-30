#include "Player.h"

Player g_Player;

Player::Player() {
	m_VM = nullptr;
	ZeroMemory(&m_GamePadState, sizeof(XINPUT_STATE));
	m_bRunOK = false;
	size = { 56.88, 64.0 };
	co_shape.r = 24.0f;
	uv = glm::vec4(0.0f, 0.0f, 160.0f, 180.0f) / 1024.0f;
	pos = {320.0f, 400.0f};
}

Player::~Player() {

}

void Player::Initialize(const SQChar* fname) {
	m_VM = sq_open(512);
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
	RegisterSQFunc(m_VM, SetPlayerTexture, _SC("SetPlayerTexture"));
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

	if (m_bRunOK = CompileSQScript(fname, m_VM)) {
		//Initialize managers
		sq_pushroottable(m_VM);
		m_TextureObj = SQGetObjectByName(m_VM, _SC("Texture"));
		sq_pop(m_VM, 1);
		m_VBuffer.SetStrideInfo(sizeof(Clb184::Vertex2D));
		m_VBuffer.Initialize(nullptr, sizeof(Clb184::Vertex2D) * 4, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		m_PlayerTexture.CreateEmptyTexture(256, 256, 0xffffffff);

		//Finally, initialize script
		CallNPSQFunc(m_VM, "main");
	}
}

const double sqrtpre = sqrt(2.0);
const double sqrtpre2 = sqrt(0.5);
constexpr double delta = 3.0;
constexpr int deadzone = (16000) * (16000);

void Player::Move() {
	bool enablex = false;
	bool enabley = false;
	float dx = 0.0f;
	float dy = 0.0f;
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (ERROR_SUCCESS == XInputGetState(0, &state)) {
		int xx = int(state.Gamepad.sThumbLX);
		int yy = int(state.Gamepad.sThumbLY);
		if ((xx * xx + yy * yy) > deadzone) {
			enablex = abs(xx) > (16000 * sqrtpre2);
			enabley = abs(yy) > (16000 * sqrtpre2);
			if (enablex && enabley) {
				dx = ((xx < 0) ? -sqrtpre2 : sqrtpre2);
				dy = ((yy < 0) ? sqrtpre2 : -sqrtpre2);
			}
			else {
				dx = (xx < 0) ? -enablex : enablex;
				dy = (yy < 0) ? enabley : -enabley;
			}
		}
	}
	bool u = (GetAsyncKeyState(VK_UP) & 0x8000);
	bool d = (GetAsyncKeyState(VK_DOWN) & 0x8000);
	bool l = (GetAsyncKeyState(VK_LEFT) & 0x8000);
	bool r = (GetAsyncKeyState(VK_RIGHT) & 0x8000);
	enablex = l || r;
	enabley = u || d;

	if (enablex && enabley) {
		dx = ((l) ? -sqrtpre2 : sqrtpre2);
		dy = ((d) ? sqrtpre2 : -sqrtpre2);
	}
	else if(enablex){
		dx = (l) ? -enablex : enablex;
	}
	else if (enabley) {
		dy = (d) ? enabley : -enabley;
	}
	pos += glm::vec2{ dx, dy } *glm::vec2(delta);
	co_shape.pos = pos;
	if (sq_getvmstate(m_VM) != SQ_VMSTATE_IDLE) {
		sq_collectgarbage(m_VM);
	}
	if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
		sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQTrue);
	}
}

void Player::Draw() {

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	if (!pVertices) return;

	float c = cosf(rotation);
	float s = sinf(rotation);
	glm::vec4 rc = uv;

	glm::vec2 fsize = size * scale;


	glm::vec2 pos2 = {};

	pos2.x = -size.x * 0.5f;
	pos2.y = -size.y * 0.5f;

	float l = pos2.x;
	float t = pos2.y;

	pVertices[0] = { pos2.x * c - pos2.y * s + pos.x, pos2.x * s + pos2.y * c + pos.y, color, rc.x, rc.y };
	pVertices[1] = { -pos2.x * c - pos2.y * s + pos.x, -pos2.x * s + pos2.y * c + pos.y, color, rc.z, rc.y };
	pVertices[3] = { -pos2.x * c - -pos2.y * s + pos.x, -pos2.x * s + -pos2.y * c + pos.y, color, rc.z, rc.w };
	pVertices[2] = { pos2.x * c - -pos2.y * s + pos.x, pos2.x * s + -pos2.y * c + pos.y, color, rc.x, rc.w };
	m_VBuffer.Unlock(sizeof(Clb184::Vertex2D) * 4);
	m_PlayerTexture.BindToContext(0, SHADER_RESOURCE_BIND_PS);
	m_VBuffer.BindToContext(0,0);
	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Clb184::g_pContext->Draw(4, 0);
}

void Player::SetTexture(const SQChar* name) {
	if (!m_PlayerTexture.LoadTextureFromFile(name))
		m_PlayerTexture.CreateEmptyTexture(256, 256, 0xffffffff);
}

void Player::Kill() {

}

const CollissionCircle& Player::GetCollisionShape() {
	return co_shape;
}
