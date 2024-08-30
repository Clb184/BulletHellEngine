#include "Player.h"

Player g_Player;

Player::Player() {
	m_VM = nullptr;
	m_bRunOK = false;
	m_TextureObj = {};
	co_shape.r = 10.0f;
	m_bEnableMove = true;
	m_IsDead = false;
	m_IsInvincible = false;
	m_MutekiTime = 0;
	m_WaitNextLife = 0;
	m_bDebugDrawEnable = true;
}

Player::~Player() {
	if (m_VM) {
		sq_close(m_VM);
		m_VM = nullptr;
	}
}

void Player::Initialize(const SQChar* fname) {
	if (m_VM) {
		sq_close(m_VM);
		m_VM = nullptr;
		co_shape.r = 10.0f;
		m_bEnableMove = true;
		m_IsDead = false;
		m_MutekiTime = 0;
		m_WaitNextLife = 0;
	}
	m_VM = sq_open(512);
	sq_pushroottable(m_VM);
	bool math_init = SQ_SUCCEEDED(sqstd_register_mathlib(m_VM));
	bool sys_init = SQ_SUCCEEDED(sqstd_register_systemlib(m_VM));
	sq_setcompilererrorhandler(m_VM, SQCompilerErrorHandler);
	sq_newclosure(m_VM, &SQErrorFunction, 0);
	sq_seterrorhandler(m_VM);
	sq_setprintfunc(m_VM, PrintFn, PrintFn);
	RegisterSQFunc(m_VM, PrintCallStack, "PrintCallStack");
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
		sq_pushstring(m_VM, _SC("player"), -1);
		sq_newtable(m_VM);
		RegisterSQFunc(m_VM, Player_SetPos, _SC("SetPos"));
		RegisterSQFunc(m_VM, Player_SetSize, _SC("SetSize"));
		RegisterSQFunc(m_VM, Player_SetScale, _SC("SetRotation"));
		RegisterSQFunc(m_VM, Player_SetUV, _SC("SetUV"));
		RegisterSQFunc(m_VM, Player_SetColor, _SC("SetColor"));
		RegisterSQFunc(m_VM, Player_SetRadius, _SC("SetRadius"));
		RegisterSQFunc(m_VM, Player_SetMutekiTime, _SC("SetMutekiTime"));
		RegisterSQFunc(m_VM, Player_EnableMove, _SC("EnableMove"));
		sq_pushstring(m_VM, _SC("texture"), -1);
		sq_pushnull(m_VM);
		sq_newslot(m_VM, -3, SQFalse);
		sq_pushstring(m_VM, _SC("muteki_time"), -1);
		sq_pushinteger(m_VM, 180);
		sq_newslot(m_VM, -3, SQFalse);
		sq_pushstring(m_VM, _SC("wait_next_life"), -1);
		sq_pushinteger(m_VM, 120);
		sq_newslot(m_VM, -3, SQFalse);
		sq_newslot(m_VM, -3, SQFalse);
		PrintStack(m_VM);
		sq_pop(m_VM, 1);


		m_VBuffer.SetStrideInfo(sizeof(Clb184::Vertex2D));
		m_VBuffer.Initialize(nullptr, sizeof(Clb184::Vertex2D) * 4, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		m_PlayerTexture.CreateEmptyTexture(256, 256, 0xffffffff);
		//Finally, initialize script
		CallNPSQFunc(m_VM, "main");
		constexpr float delta = 3.14159 / 8.0;
		float ang = 0.0f;
		for (int i = 0; i < 16; i++) {
			m_Points[i] = { cos(ang), sin(ang) };
			ang += delta;
		}
		m_Points[16] = m_Points[0];
		m_PrimBuffer.SetStrideInfo(sizeof(Clb184::Point2D));
		m_PrimBuffer.Initialize(nullptr, sizeof(Clb184::Point2D) * 17, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

	}
}

const double sqrtpre = sqrt(2.0);
const double sqrtpre2 = sqrt(0.5);
constexpr double delta = 5.0;
constexpr int deadzone = (16000) * (16000);

void Player::Move() {
	if (!m_bRunOK) return;
	if (m_bEnableMove) {
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
		else if (enablex) {
			dx = (l) ? -enablex : enablex;
		}
		else if (enabley) {
			dy = (d) ? enabley : -enabley;
		}
		pos += glm::vec2{ dx, dy } *glm::vec2(delta);
		co_shape.pos = pos;
	}
	//if (sq_getvmstate(m_VM) != SQ_VMSTATE_IDLE) {
	sq_collectgarbage(m_VM);
	if (sq_getvmstate(m_VM) == SQ_VMSTATE_SUSPENDED) {
		sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQFalse);
	}
	if (m_MutekiTime > 0) {
		m_MutekiTime--;
	}
	if (m_WaitNextLife > 0 && m_IsDead) {
		m_WaitNextLife--;
	}
	else if (m_WaitNextLife <= 0 && m_IsDead) {
		m_bEnableMove = true;
		sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQTrue);
		CallNPSQFunc(m_VM, "main");
		sq_pushroottable(m_VM);
		sq_pushstring(m_VM, _SC("player"), -1);
		sq_get(m_VM, -2);
		sq_pushstring(m_VM, _SC("muteki_time"), -1);
		sq_get(m_VM, -2);
		sq_getinteger(m_VM, -1, &m_MutekiTime);
		sq_pop(m_VM, 3);
		m_IsDead = false;
	}
}

void Player::Draw() {
	if (!m_bRunOK) return;

	//Obtain an address with the size of the data
	Clb184::Vertex2D* pVertices = (Clb184::Vertex2D*)(m_VBuffer.Lock());
	Clb184::CTexture* pTexture;

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
	sq_pushroottable(m_VM);
	sq_pushstring(m_VM, _SC("player"), -1);
	if (SQ_SUCCEEDED(sq_get(m_VM, -2))) {
		sq_pushobject(m_VM, m_TextureObj);
		sq_pushstring(m_VM, _SC("texture"), -1);
		sq_get(m_VM, -3);
		if (SQTrue == sq_instanceof(m_VM)) {
			sq_getinstanceup(m_VM, -1, (SQUserPointer*)(&pTexture), NULL);
			Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
			Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
			pTexture->BindToContext(0, SHADER_RESOURCE_BIND_PS);
			m_VBuffer.BindToContext(0, 0);
			Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			Clb184::g_pContext->Draw(4, 0);
		}
		sq_pop(m_VM, 2);
	}
	sq_pop(m_VM, 2);

	if (m_bDebugDrawEnable) {
		Clb184::Point2D* pPoints = (Clb184::Point2D*)m_PrimBuffer.Lock();
		pPoints[0] = { m_Points[0] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 1] = { m_Points[1] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 2] = { m_Points[2] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 3] = { m_Points[3] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 4] = { m_Points[4] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 5] = { m_Points[5] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 6] = { m_Points[6] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 7] = { m_Points[7] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 8] = { m_Points[8] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 9] = { m_Points[9] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 10] = { m_Points[10] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 11] = { m_Points[11] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 12] = { m_Points[12] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 13] = { m_Points[13] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 14] = { m_Points[14] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 15] = { m_Points[15] * co_shape.r + pos, 0x8c1414ff };
		pPoints[0 + 16] = { m_Points[16] * co_shape.r + pos, 0x8c1414ff };
		m_PrimBuffer.Unlock(sizeof(Clb184::Point2D) * 17);
	}

}

void Player::Kill() {
	if (!m_IsInvincible && !(m_MutekiTime > 0 || m_WaitNextLife >0)) {
		m_IsDead = true;
		m_bEnableMove = false;
		sq_wakeupvm(m_VM, SQFalse, SQFalse, SQTrue, SQTrue);
		sq_pushroottable(m_VM);
		sq_pushstring(m_VM, _SC("player"), -1);
		sq_get(m_VM, -2);
		sq_pushstring(m_VM, _SC("wait_next_life"), -1);
		sq_get(m_VM, -2);
		sq_getinteger(m_VM, -1, &m_WaitNextLife);
		sq_pop(m_VM, 3);
		CallNPSQFunc(m_VM, "OnDeath");
	}
}

void Player::DrawHitbox() {
	if (m_bDebugDrawEnable) {
		Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessVertexShader();
		Clb184::CDefault2DShader::GetShaderInstance().BindTexturelessPixelShader();
		m_PrimBuffer.BindToContext(0, 0);
		Clb184::g_pContext->Draw(17, 0);
	}
}

void Player::SetDebugDraw(bool state) {
	m_bDebugDrawEnable = state;
}

SQInteger Player_SetPos(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -1, &pData2, NULL);
	g_Player.pos = pVec->obj;
	g_Player.co_shape.pos = pVec->obj;
	return 0;
}

SQInteger Player_SetSize(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -1, &pData2, NULL);
	g_Player.size = pVec->obj;
	return 0;
}

SQInteger Player_SetScale(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec2>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -1, &pData2, NULL);
	g_Player.scale = pVec->obj;
	return 0;
}

SQInteger Player_SetRotation(HSQUIRRELVM v) {
	sq_getfloat(v, -1, &g_Player.rotation);
	return 0;
}

SQInteger Player_SetUV(HSQUIRRELVM v) {
	union {
		LinalgType<glm::vec4>* pVec;
		SQUserPointer pData2;
	};
	sq_getinstanceup(v, -1, &pData2, NULL);
	g_Player.uv = pVec->obj;
	return 0;
}

SQInteger Player_SetMutekiTime(HSQUIRRELVM v) {
	sq_getinteger(v, -1, &g_Player.m_MutekiTime);
	return 0;
}

SQInteger Player_SetColor(HSQUIRRELVM v) {
	SQInteger color;
	sq_getinteger(v, -1, &color);
	g_Player.color = static_cast<uint32_t>(color);
	return 0;
}

SQInteger Player_SetRadius(HSQUIRRELVM v) {
	sq_getfloat(v, -1, &g_Player.co_shape.r);
	return 0;
}

SQInteger Player_EnableMove(HSQUIRRELVM v) {
	SQBool state;
	sq_getbool(v, -1, &state);
	g_Player.m_bEnableMove = static_cast<bool>(state);
	return 0;
}
