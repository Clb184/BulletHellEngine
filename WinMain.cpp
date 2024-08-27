#include "WinMain.h"
#include "GameMain.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

wrl::ComPtr<ID3D11DepthStencilState> m_Draw3DState;
wrl::ComPtr<ID3D11DepthStencilState> m_Draw2DState;

int WINAPI wWinMain(HINSTANCE current, HINSTANCE prev, LPWSTR args, int cmd) {


	GameMain* pTasks = new GameMain();
	
	HWND Window = 0;
	HRESULT HR;
	MSG Msg;
	Clb184::CD3DWindow window;
	Clb184::CFrameLimiter frame_limit;

	std::wstring num;
	std::wstring num2;

#ifdef DEBUG
	static Clb184::CKeyboard kbd;
	wrl::ComPtr<IDXGIDebug1> pDXDebug;
	HR = DXGIGetDebugInterface1(0, IID_PPV_ARGS(pDXDebug.GetAddressOf()));
#endif
	frame_limit.SetFramerate(60);


#ifdef DEBUG
	const wchar_t* title = L"Bullet Hell Game - DEBUG VERSION";
#else 
	const wchar_t* title = L"Bullet Hell Game";
#endif

	Window = window.SpawnWindow(title, current, (WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME)) | WS_POPUP);

#ifdef DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
#endif

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
	text.SetSize(64.0f);
	text.Update();
	Clb184::CD3DSprite spritetext;
	spritetext.SetPos({ 320.0f, 400.0f });
	spritetext.SetSize({ 512.0f, 512.0f });
	float f = 0;
	bool sign = true;
	pTasks->Initialize("script/main.nut");
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
				pTasks->Move();

				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
				Clb184::g_pContext->OMSetDepthStencilState(m_Draw2DState.Get(), 0);

				pTasks->Draw();
				double res = glm::mix((sign) ? -3.1415926 : 3.1415926, (sign) ? 3.1415926 : -3.1415926, glm::smoothstep(0.0, 1.0, fmod((double)f, 90.0) / 90.0));
				sprite.SetRotation(res);
				sprite.SetPos({ 320.0f + res * 40.0f, 240.0f });
				tex.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				sprite.Draw();


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

				if (frame_limit.GetMeanFPS() != INFINITY) {
					num = std::to_wstring(frame_limit.GetMeanFPS());
					num2 = L"";
					for (int i = 0; ; i++) {
						num2 += num.at(i);
						if (num.at(i) == '.') {
							num2 += num.at(i + 1);
							num2 += num.at(i + 2);
							break;
						}
					}
				}
				else {
					num2 = L"INF ";
				}
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