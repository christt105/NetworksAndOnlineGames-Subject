#include "ModuleUI.h"
#include "ModuleTextures.h"
#include "Application.h"
#include "Input.h"
#include "Time.h"
#include <d3d9.h>

// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

extern HWND hwnd;                                // Window handle
extern ID3D11Device        *g_pd3dDevice;        // Direct3d11 device pointer
extern ID3D11DeviceContext *g_pd3dDeviceContext; // Direct3d11 device context pointer

Texture *banner = nullptr;

bool ModuleUI::init()
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	banner = App->modTextures->loadTexture("banner.jpg");

	return true;
}

bool ModuleUI::preUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	return true;
}

bool ModuleUI::gui()
{
	//ImGui::Begin("Main menu");

	//ImGui::Image(banner->shaderResource, ImVec2(300, 100));

	//ImGui::Text("This is the main window!");

	//static char textBuffer[1024];
	//ImGui::InputText("Insert name", textBuffer, sizeof(textBuffer));

	//ImGui::Text("Time since beginning: %f", Time.time);
	//ImGui::Text("Delta time: %f", Time.deltaTime);
	//ImGui::Text("Input Thumb X: %f", Input.leftThumbX);
	//ImGui::Text("Input Thumb Y: %f", Input.leftThumbY);

	//if (ImGui::Button("Button 1"))
	//{
	//	// Button 1
	//}

	//if (ImGui::Button("Button 2"))
	//{
	//	// Button 2
	//}

	//ImGui::End();

	return true;
}

bool ModuleUI::postUpdate()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return true;
}

bool ModuleUI::cleanUp()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ModuleUI::HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}
