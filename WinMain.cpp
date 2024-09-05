#include "WinMain.h"
#include "GameMain.h"
#include "3D/Camera.h"
#include "3D/Transform.h"
#include "3D/TriDiObject.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

wrl::ComPtr<ID3D11DepthStencilState> g_Draw3DState;

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


	InitAudio();
	Camera cam;
	Clb184::CVertexShader v3d;
	Clb184::CPixelShader p3d;
	v3d.LoadFromFile(L"shader/Default3DVS.hlsl", "vs_4_0", "main");
	p3d.LoadFromFile(L"shader/Default3DPS.hlsl", "ps_4_0", "main");

	Clb184::CInputLayout ly3d;
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc3d {
			{"POSITION"   , 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR"   , 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD"   , 0,DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL"   , 0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	ly3d.CreateInputLayout(&desc3d, &v3d);
	Clb184::Vertex3D vertices[] = {
		{0.0,0.0,0.0, 1.0,1.0,0.0,1.0, 0.0,1.0, 0.0,0.0,0.0},
		{100.0,0.0,0.0, 1.0,1.0,0.0,1.0, 0.0,1.0, 0.0,0.0,0.0},

		{0.0,0.0,0.0, 1.0,0.0,0.0,1.0, 0.0,1.0, 0.0,0.0,0.0},
		{0.0,100.0,0.0, 1.0,0.0,0.0,1.0, 0.0,1.0, 0.0,0.0,0.0},

		{0.0,0.0,0.0, 0.0,0.0,1.0,1.0, 0.0,1.0, 0.0,0.0,0.0},
		{0.0,0.0,100.0, 0.0,0.0,1.0,1.0, 0.0,1.0, 0.0,0.0,0.0},
	};
	Clb184::CVertexBuffer vb3d(vertices, sizeof(vertices), D3D11_USAGE_DEFAULT, 0);
	vb3d.SetStrideInfo(sizeof(Clb184::Vertex3D));

	TriDiObject obj;
	obj.LoadModel("bg/ssg_city.obj");

	cam.Initialize();
	cam.SetPos({0.0, 0.0, 10.0});
	cam.SetFOV(3.1415926 / 6.0);
	cam.SetFog({ 200.0,300.0 });
	cam.UpdateMatrix();
	Clb184::CTexture nt;
	nt.CreateEmptyTexture(256, 256, 0xffffffff);
	Transform tr3d;
	tr3d.SetPosition({ 0.0,0.0,0.0 });
	tr3d.SetScale({ 1.0,1.0, 1.0 });
	tr3d.UpdateMatrix();

	Clb184::BasicWorldComposition bwc;
	bwc.FogColor = { 1.0,1.0,0.0, 1.0};
	bwc.Ambient = {0.1,0.1,0.1,0.0};
	Clb184::CConstantBuffer bwcb(&bwc, sizeof(Clb184::BasicWorldComposition));


	D3D11_DEPTH_STENCIL_DESC depthd = {};
	depthd.DepthEnable = TRUE;
	depthd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Clb184::CSamplerState smp;
	smp.SetSamplerKind(SAMPLER_PIXEL);
	smp.SetWrappingMode(OFFSET_WRAP);
	smp.CreateSampler();

	depthd.StencilEnable = FALSE;
	depthd.StencilWriteMask = 0;

	HR = Clb184::g_pD3D11Device->CreateDepthStencilState(&depthd, &g_Draw3DState);


	Clb184::CTexture tex;
	tex.LoadTextureFromFile("graph/ball.tga");
	Clb184::CD3DSprite sprite;
	sprite.SetPos({ 600.0f, 430.0f });
	sprite.SetSize({ 32.0f, 32.0f });
	Clb184::CSamplerState sampler;
	sampler.SetWrappingMode(OFFSET_WRAP);
	sampler.SetSamplerKind(SAMPLER_PIXEL);
	sampler.CreateSampler();
	sampler.BindToContext(0, SHADER_RESOURCE_BIND_PS);
	float rot = 0.0;
	/*
	Clb184::CText text;
	text.Setup(1024, 1024);
	text.SetText("Idiots are thinking...");
	text.SetFont(L"Consolas");
	text.SetSize(38.0f);
	text.Update();*/

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
	glm::vec3 p = { 0.0, 0.0, 0.0 };
	float f = 0;
	bool sign = true;
	Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_GameMain.SetPlayerScript("player/player_test.nut");
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

				//Testing 3D
				v3d.BindToContext();
				p3d.BindToContext();
				ly3d.BindToContext();
				vb3d.BindToContext(0, SHADER_RESOURCE_BIND_VS);
				Clb184::g_pContext->OMSetDepthStencilState(g_Draw3DState.Get(), 1);


				if (GetAsyncKeyState('W') & 0x8000)
					p.y += 1.0;
				if (GetAsyncKeyState('S') & 0x8000)
					p.y -= 1.0;

				if (GetAsyncKeyState('A') & 0x8000)
					p.x -= 1.0;
				if (GetAsyncKeyState('D') & 0x8000)
					p.x += 1.0;

				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
					p.z += 1.0;
				if (GetAsyncKeyState(VK_RCONTROL) & 0x8000)
					p.z -= 1.0;

				tr3d.Bind(1);
				cam.SetPos(p);
				cam.UpdateMatrix();
				cam.Bind(0);
				nt.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				Clb184::g_pContext->Draw(6, 0);

				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				bwcb.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				smp.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				obj.Draw();
				nt.BindToContext(0, SHADER_RESOURCE_BIND_PS);


				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
				g_GameMain.Draw();
#ifdef _DEBUG
				kbd.UpdateKeyboard();
				if (kbd.IsKeyPressed('L'))
					HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
				
				char numbuf[8] = "";
				sprintf_s(numbuf, "%.2f", frame_limit.GetMeanFPS());
				D3D11_RECT rc = { 0.0, 0.0, 640.0, 480.0 };
				Clb184::g_pContext->RSSetScissorRects(1, &rc);
				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
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
	DestroyAudio();
#ifdef DEBUG
	HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif // _DEBUG

}