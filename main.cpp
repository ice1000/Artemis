// dear imgui - standalone example application for DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <tchar.h>

#include "texture.h"
#include "image.h"
#include "danmuku.h"

#include <memory>

using std::shared_ptr;

// Data
ID3D11Device *g_pd3dDevice = NULL;
static ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
static IDXGISwapChain *g_pSwapChain = NULL;
static ID3D11RenderTargetView *g_mainRenderTargetView = NULL;

void CreateRenderTarget() {
	ID3D11Texture2D *pBackBuffer;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
	}
}

HRESULT CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2,
	                                  D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel,
	                                  &g_pd3dDeviceContext) != S_OK)
		return E_FAIL;

	CreateRenderTarget();

	return S_OK;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();
	if (g_pSwapChain) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pd3dDeviceContext) {
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
	}
	if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int main(int argc, const char *argv[]) {
	const char *fileName = argc >= 2 ? argv[1] : "Artemis.txt";

	// Create application window
	WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
	                 _T("Artemis"), NULL};
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, _T("Artemis Shooting Game Editor"), WS_OVERLAPPEDWINDOW, 100, 100, 1280,
	                         800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (CreateDeviceD3D(hwnd) < 0) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	CompleteImage completeImage;
	CompleteImage::fromFile("res/etama.png", completeImage);
	SubImage imageSet[12][16];
	{
		ImVec2 pos = ImVec2(0, 0), size = ImVec2(16, 15);
		for (auto &column : imageSet) {
			for (auto &item : column) {
				item = completeImage.toSubImage();
				item.pos = pos;
				item.size = size;
				pos.x += 16;
			}
			pos.x = 0;
			pos.y += 16;
			// So only the first column's y 15
			size.y = 16;
		}
	}

	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec2 originalSpacing = ImGui::GetStyle().ItemSpacing;
	ImVec2 spacing = originalSpacing;

	SpellCard spellCard;
	auto task0 = std::make_shared<LinearTask>();
	task0->setImage(imageSet[10][2]);
	auto task1 = std::make_shared<LinearTask>();
	task1->setImage(imageSet[10][1]);
	spellCard.addTask(task0);
	spellCard.addTask(task1);

	double startTime = ImGui::GetTime();
	double fixedTime = -1;
	double currentTime = startTime;
	char debugWindowTitle[100];
	bool previewWindowOpened = false;

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof msg);
	while (msg.message != WM_QUIT) {
		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::GetStyle().ItemSpacing = originalSpacing;
		if (fixedTime < 0)
			currentTime = ImGui::GetTime() - startTime;
		if (previewWindowOpened && ImGui::Begin("Preview Images", &previewWindowOpened)) {
			for (auto &column : imageSet) {
				for (auto &item : column) {
					item.drawWithBoarder();
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
			ImGui::End();
		}

		if (ImGui::Begin("Editor")) {
			ImGui::ColorEdit3("Background Color", reinterpret_cast<float *>(&clearColor));
			ImGui::SliderFloat2("Item Spacing", reinterpret_cast<float *>(&spacing), -5, 5);
			if (ImGui::SliderDouble("The World!", &fixedTime, -1, currentTime + 1)) {
				if (fixedTime < 0) fixedTime = 0;
			}
			if (ImGui::Button("Play")) {
				startTime = ImGui::GetTime();
				fixedTime = -1;
			}
			ImGui::SameLine();
			ImGui::Checkbox("Open Image Preview", &previewWindowOpened);
			for (size_t i = 0; i < spellCard.taskSize(); ++i) {
				char title[20], changeImage[20];
				ImFormatString(title, IM_ARRAYSIZE(title), "Task %i", i);
				ImFormatString(changeImage, IM_ARRAYSIZE(changeImage), "Change Image##%i", i);
				if (ImGui::TreeNode(title)) {
					auto task = spellCard.getTask(i);
					task->editor();
					const char *strId = "CHANGE_IMAGE_POPUP";
					if (ImGui::Button(changeImage)) ImGui::OpenPopup(strId);
					if (ImGui::BeginPopup(strId)) {
						for (auto &column : imageSet) {
							for (auto &item : column) {
								// This function does not return true, use IsItemClicked
								item.drawButton();
								if (ImGui::IsItemClicked()) task->setImage(item);
								ImGui::SameLine();
							}
							ImGui::NewLine();
						}
						ImGui::EndPopup();
					}
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		ImGui::GetStyle().ItemSpacing = spacing;
		ImGui::SetNextWindowBgAlpha(.5f);

		ImFormatString(debugWindowTitle, IM_ARRAYSIZE(debugWindowTitle), "Fps: %f, Time: %lf###Debug",
		               ImGui::GetIO().Framerate, currentTime);
		if (ImGui::Begin(debugWindowTitle)) {
			spellCard.draw(fixedTime > 0 ? fixedTime : currentTime);
			ImGui::End();
		}

		ImGui::Render();
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float *) &clearColor);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}
