#include "WinMain.h"
#include "GameMain.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

wrl::ComPtr<ID3D11DepthStencilState> m_Draw3DState;
wrl::ComPtr<ID3D11DepthStencilState> m_Draw2DState;

#pragma comment(lib, "xinput.lib")

int WINAPI wWinMain(HINSTANCE current, HINSTANCE prev, LPWSTR args, int cmd) {
	GameMain g_GameMain;
	
	HWND Window = 0;
	HRESULT HR;
	MSG Msg;
	Clb184::CD3DWindow window;
	Clb184::CFrameLimiter frame_limit;

#ifdef DEBUG
	static Clb184::CKeyboard kbd;
	wrl::ComPtr<IDXGIDebug1> pDXDebug;
	HR = DXGIGetDebugInterface1(0, IID_PPV_ARGS(pDXDebug.GetAddressOf()));
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
#endif
	frame_limit.SetFramerate(60);


#ifdef DEBUG
	const wchar_t* title = L"Bullet Hell Game - DEBUG VERSION";
#else 
	const wchar_t* title = L"Bullet Hell Game";
#endif


	Window = window.SpawnWindow(title, current, (WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME)) | WS_POPUP);
	if (0 == Window) return -1;


	D3D11_DEPTH_STENCIL_DESC depthd = {};
	depthd.DepthEnable = TRUE;
	depthd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthd.StencilEnable = FALSE;
	depthd.StencilWriteMask = 0;

	HR = Clb184::g_pD3D11Device->CreateDepthStencilState(&depthd, &m_Draw3DState);

	D3D11_DEPTH_STENCIL_DESC depthd2 = {};
	depthd2.DepthEnable = FALSE;
	depthd2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthd2.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthd2.StencilEnable = FALSE;
	depthd2.StencilWriteMask = 0;

	HR = Clb184::g_pD3D11Device->CreateDepthStencilState(&depthd2, &m_Draw2DState);

	Clb184::CTexture tex;
	tex.LoadTextureFromFile("graph/ball.tga");
	Clb184::CD3DSprite sprite;
	sprite.SetPos({ 320.0f, 240.0f });
	sprite.SetSize({ 64.0f, 64.0f });
	Clb184::CSamplerState sampler;
	sampler.SetWrappingMode(OFFSET_WRAP);
	sampler.SetSamplerKind(SAMPLER_PIXEL);
	sampler.CreateSampler();
	sampler.BindToContext(0, SHADER_RESOURCE_BIND_PS);

	Clb184::CText text;
	text.Setup(1024, 1024);
	text.SetText("Idiots are thinking...");
	text.SetFont(L"Consolas");
	text.SetSize(38.0f);
	text.Update();

	Clb184::CD3DSprite spritetext;
	spritetext.SetPos({ 320.0f, 400.0f });
	spritetext.SetSize({ 512.0f, 512.0f });

	Clb184::CText fps;
	fps.Setup(256, 256);
	fps.SetText("none");
	fps.SetFont(L"Consolas");
	fps.SetSize(18.0f);
	fps.SetTextAnchor(Clb184::CText::ANCHOR_RIGHT);
	fps.Update();
	const float onetfs = 1.0f / 256.0f;

	Clb184::CD3DSprite fpssprite;
	fpssprite.SetPos({640.0, 480.0});
	fpssprite.SetCenter(2, 2);
	fpssprite.SetSize({18 * 8, 18});
	fpssprite.SetUV(glm::vec4(256.0f - 18.0f * 8.0f, 0.0f, 256.0f, 18.0f) * onetfs);

	float f = 0;
	bool sign = true;
	g_GameMain.Initialize("script/main.nut");
	while (1) {
		if (PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&Msg, NULL, 0, 0)) {
				return Msg.wParam;
			}
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else if (window.GetWindowActive() || 1) {
			if(frame_limit.IsNeedUpdate()) {
				window.ClearWindow();
				window.BindTargetView();
				g_GameMain.Move();

				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
				Clb184::g_pContext->OMSetDepthStencilState(m_Draw2DState.Get(), 0);

				g_GameMain.Draw();
				double res = glm::mix((sign) ? -3.1415926 : 3.1415926, (sign) ? 3.1415926 : -3.1415926, glm::smoothstep(0.0, 1.0, fmod((double)f, 90.0) / 90.0));
				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				sprite.SetRotation(res);
				sprite.SetPos({ 320.0f + res * 40.0f, 240.0f });
				tex.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				sprite.Draw();
				
				//Testing XInput lol
				XINPUT_STATE state;
				XInputGetState(0, &state);
				DWORD buttons = state.Gamepad.wButtons;
				char pad[1024] = "";
				sprintf(pad, "LX:%d LY:%d RX:%d RY:%d\n"
					"X:%1d Y:%1d A:%1d B:%1d\n"
					"LT:%1d RT:%1d LS:%1d RS:%1d\n"
					"Back:%1d Start:%1d\n"
					"Up:%1d Down:%1d Left:%1d Right:%1d\n"
					"LeftTrigger:%d RightTrigger:%d\n"
					, state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, state.Gamepad.sThumbRX, state.Gamepad.sThumbRY,
					bool(buttons & XINPUT_GAMEPAD_X), bool(buttons & XINPUT_GAMEPAD_Y), bool(buttons & XINPUT_GAMEPAD_A), bool(buttons & XINPUT_GAMEPAD_B),
					bool(buttons & XINPUT_GAMEPAD_LEFT_THUMB), bool(buttons & XINPUT_GAMEPAD_RIGHT_THUMB), bool(buttons & XINPUT_GAMEPAD_LEFT_SHOULDER), bool(buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER),
					bool(buttons & XINPUT_GAMEPAD_BACK), bool(buttons & XINPUT_GAMEPAD_START),
					bool(buttons & XINPUT_GAMEPAD_DPAD_UP), bool(buttons & XINPUT_GAMEPAD_DPAD_DOWN), bool(buttons & XINPUT_GAMEPAD_DPAD_LEFT), bool(buttons & XINPUT_GAMEPAD_DPAD_RIGHT),
					state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger
					);
				
				text.SetText(pad);
				text.Update();
				
				text.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				spritetext.Draw();

				f+= 1.1;
				if (f >= 90.0) {
					f = 0.0;
					sign = !sign;
				}
#ifdef _DEBUG
				kbd.UpdateKeyboard();
				if (kbd.IsKeyPressed('L'))
					HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif

				char numbuf[8] = "";
				sprintf_s(numbuf, "%.2f", frame_limit.GetMeanFPS());
				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
				Clb184::g_pContext->OMSetDepthStencilState(m_Draw2DState.Get(), 0);
				fps.SetText(numbuf);
				fps.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				fps.Update();
				fpssprite.Draw();

				HR = Clb184::g_pSwapChain->Present(0, 0);
				switch (HR) {
				case DXGI_ERROR_DEVICE_REMOVED:
					HR = Clb184::g_pD3D11Device->GetDeviceRemovedReason();
					break;
				case DXGI_ERROR_INVALID_CALL:
					break;
				}
			}
		}
		else {
			WaitMessage();
		}
	}
#ifdef DEBUG
	HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif // _DEBUG

}