#include "WinMain.h"
#include "GameMain.h"
#include "3D/Camera.h"
#include "3D/Transform.h"
#include "3D/TriDiObject.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

wrl::ComPtr<ID3D11DepthStencilState> g_Draw3DState;

#pragma comment(lib, "xinput.lib")



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ClbProcIG(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static bool fullscreen = false;
	HDC hdc;
	PAINTSTRUCT ps;
	HRESULT hr;
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	// (Your code process Win32 messages)


	switch (uMsg) {
	case WM_ACTIVATEAPP:
		Clb184::CD3DWindow::SetWindowState(wParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return 1;
	case WM_SIZE: {
		wrl::ComPtr<IDXGIOutput> pOutput;
		BOOL wind;
		if (Clb184::g_pSwapChain) {
			if (SUCCEEDED(Clb184::g_pSwapChain->GetFullscreenState(&wind, &pOutput))) {
				Clb184::CD3DWindow::CleanWindowElements();
			    hr = Clb184::g_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				Clb184::CD3DWindow::CreateBackBuffer();
				POINT wh = Clb184::CD3DWindow::GetWindowActualWH();
				D3D11_VIEWPORT v;
				v.TopLeftX = 0.0;
				v.TopLeftY = 0.0;
				v.Width = wh.x;
				v.Height = wh.y;
				v.MinDepth = 0.0;
				v.MaxDepth = 1.0;
				Clb184::g_pContext->RSSetViewports(1, &v);
			}
		}
	}    return 1;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


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


	Window = window.SpawnWindow(title, current, (WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME)) | WS_POPUP, ClbProcIG);
	if (0 == Window) return -1;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(Clb184::g_pD3D11Device.Get(), Clb184::g_pContext.Get());

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
	cam.SetPos({0.0, -100.0, 40.0});
	float fov = 60.0f;
	cam.SetRatio(4.0 / 3.0);
	cam.UpdateMatrix();
	Clb184::CTexture nt;
	nt.CreateEmptyTexture(256, 256, 0xffffffff);
	glm::vec3 tp[5] = {glm::vec3(0),glm::vec3(0) ,glm::vec3(0),glm::vec3(0) ,glm::vec3(0) };
	glm::vec3 rtt[5] = {glm::vec3(0),glm::vec3(0) ,glm::vec3(0),glm::vec3(0) ,glm::vec3(0) };
	glm::vec3 ts[5] = {glm::vec3(1),glm::vec3(1) ,glm::vec3(1),glm::vec3(1) ,glm::vec3(1) };
	Transform tr3d[5];

	Clb184::BasicWorldComposition bwc;
	bwc.FogColor = { 1.0,1.0,0.0, 1.0};
	bwc.Ambient = {0.1,0.1,0.1,0.0};
	bwc.WorldLightDir = { 0.0f, 0.0f, 0.0f, 0.0f};
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
	char buff[512] = "";
	glm::vec2 fg = { 200.0,300.0 };
	glm::vec3 rt = {};
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


				if (Clb184::CD3DWindow::GetWindowActive()) {
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
				}
				cam.SetFog(fg);
				cam.SetRotation(rt);
				cam.SetPos(p);
				cam.SetFOV(fov * (3.14159 / 180.0));
				cam.UpdateMatrix();

				bwcb.QuickLockUnlock(&bwc, sizeof(bwc));
				// (Your code process and dispatch Win32 messages)
				// Start the Dear ImGui frame
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImGui::Begin("3D Manipulation");                          // Create a window called "Hello, world!" and append into it.
				ImGui::InputText(" ", buff, sizeof(buff));
				ImGui::SameLine();
				if (ImGui::Button("Init Script"))
					g_GameMain.Initialize(buff);
				ImGui::SliderFloat("Cam FOV", &fov, 0.0f, 180.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat3("Cam Pos", &p.x, -1000.0f, 1000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat3("Cam Rot", &rt.x, -3.1415926, 3.1415926);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat2("Fog Pos", &fg.x, 10.0, 1000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat3("World Light Dir", &bwc.WorldLightDir.x, -3.1415926, 3.1415926);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("Ambient", &bwc.Ambient.x);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("Fog", &bwc.FogColor.x);            // Edit 1 float using a slider from 0.0f to 1.0f
				for (int i = 0; i < 1; i++) {
					ImGui::SliderFloat3(std::string("Obj pos " + std::to_string(i)).c_str(), &tp[i].x, -1000.0f, 1000.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
					ImGui::SliderFloat3(std::string("Obj rot " + std::to_string(i)).c_str(), &rtt[i].x, -3.1415926, 3.1415926);            // Edit 1 float using a slider from 0.0f to 1.0f
					ImGui::SliderFloat3(std::string("Obj sca " + std::to_string(i)).c_str(), &ts[i].x, -10.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				}
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();

				g_GameMain.Move();
				//Testing 3D
				v3d.BindToContext();
				p3d.BindToContext();
				ly3d.BindToContext();
				vb3d.BindToContext(0, SHADER_RESOURCE_BIND_VS);
				Clb184::g_pContext->OMSetDepthStencilState(g_Draw3DState.Get(), 1);

				tr3d[0].Bind(1);
				cam.Bind(0);
				nt.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				Clb184::g_pContext->Draw(6, 0);

				Clb184::g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				bwcb.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				smp.BindToContext(0, SHADER_RESOURCE_BIND_PS);
				for (int i = 0; i < 1; i++) {
					tr3d[i].SetPosition(tp[i]);
					tr3d[i].SetRotation(rtt[i]);
					tr3d[i].SetScale(ts[i]);
					tr3d[i].UpdateMatrix();
					tr3d[i].Bind(1);
					obj.Draw();
				}

				Clb184::CDefault2DShader::GetShaderInstance().BindVertexShader();
				Clb184::CDefault2DShader::GetShaderInstance().BindPixelShader();
				g_GameMain.Draw();
				// Rendering
				// (Your code clears your framebuffer, renders your other stuff etc.)
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				// (Your code calls swapchain's Present() function)
#ifdef _DEBUG
				kbd.UpdateKeyboard();
				if (kbd.IsKeyPressed('L'))
					HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
				
				char numbuf[8] = "";
				sprintf_s(numbuf, "%.2f", frame_limit.GetMeanFPS());
				D3D11_RECT rc = { 0.0, 0.0, 640.0, 480.0 };
				Clb184::CD3DWindow::SetScissors(rc);
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
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#ifdef DEBUG
	HR = pDXDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif // _DEBUG

}