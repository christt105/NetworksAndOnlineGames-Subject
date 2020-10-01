#include "ModulePlatform.h"
#include "Application.h"
#include "ModuleUI.h"
#include "ModuleRender.h"
#include "Log.h"
#include "Time.h"
#include "Input.h"
#include "imgui/imgui.h"

#pragma comment (lib, "D3D11.lib")

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include <xinput.h>

// Data
const char *windowClassStr = "Networks and Online Games";
const char *windowTitleStr = "Networks and Online Games";
HINSTANCE instance = NULL;                           // Application instance
HWND hwnd = NULL;                                    // Window handle
WNDCLASSEX windowClass = {};                         // Window class


// XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}
static x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}
static x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_


static LONGLONG GlobalPerfCountFrequency;
static LARGE_INTEGER StartTime;
static LARGE_INTEGER EndTime;

inline LARGE_INTEGER
Win32GetWallClock(void)
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return(Result);
}

inline float
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	float Result = ((float)(End.QuadPart - Start.QuadPart) /
		(float)GlobalPerfCountFrequency);
	return(Result);
}

#define ENABLE_EXTERNAL

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef ENABLE_EXTERNAL
	if (App->modUI->HandleWindowsEvents(msg, wParam, lParam) == 1)
		return TRUE;
#endif

	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED && wParam != SIZE_MAXSHOW)
		{
			App->modRender->resizeBuffers((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
		}
		return 0;

	case WM_KILLFOCUS:
		for (auto &buttonState : Input.buttons)
		{
			if (buttonState == Pressed) {
				buttonState = Idle; // NOTE(jesus): Maybe Release?
			}
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void Win32ProcessKeyboardMessage(InputButtonState *NewState, bool IsDown)
{
	if (IsDown)
	{
		*NewState = Press;
	}
	else
	{
		*NewState = Release;
	}
}

void Win32ProcessGamepadMessage(InputButtonState *NewState, XINPUT_GAMEPAD *pad, int button_flag)
{
	bool IsPressed = pad->wButtons & button_flag;
	if (IsPressed)
	{
		if (*NewState == Idle)
		{
			*NewState = Press;
		}
		else
		{
			*NewState = Pressed;
		}
	}
	else
	{
		if (*NewState == Pressed || *NewState == Press)
		{
			*NewState = Release;
		}
		else
		{
			*NewState = Idle;
		}
	}
}

float Win32ProcessGamepadThumb(SHORT value, SHORT deadZoneThreshold)
{
	float res = 0;

	if (value < -deadZoneThreshold)
	{
		res = (float)((value + deadZoneThreshold) / (32768.0f - deadZoneThreshold));
	}
	else if (value > deadZoneThreshold)
	{
		res = (float)((value - deadZoneThreshold) / (32767.0f - deadZoneThreshold));
	}

	return(res);
}

bool ModulePlatform::init()
{
	// Get the application instance handle
	instance = ::GetModuleHandle("");

	// Create window class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpszClassName = _T(windowClassStr);
	windowClass.hInstance = instance;
	windowClass.lpfnWndProc = WndProc;
	windowClass.style = CS_OWNDC;
	if (::RegisterClassEx(&windowClass) == 0)
	{
		LOG("ModuleWindow::init() - RegisterClassEx() failed");
		return false;
	}

	// Create application window
	hwnd = ::CreateWindowEx(
		0,
		windowClass.lpszClassName,
		_T(windowTitleStr),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		windowClass.hInstance,
		NULL);

#ifdef ENABLE_EXTERNAL
	// Init XInput
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");

	if (!XInputLibrary) { XInputLibrary = LoadLibraryA("xinput9_1_0.dll"); }
	if (!XInputLibrary) { XInputLibrary = LoadLibraryA("xinput1_3.dll"); }
	if (XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		if (!XInputGetState) { XInputGetState = XInputGetStateStub; }
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
		if (!XInputSetState) { XInputSetState = XInputSetStateStub; }
	}
	else
	{
		LOG("ModuleWindow::init() - XInput library loading failed");
		::DestroyWindow(hwnd);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	// Initialize button states
	for (auto &buttonState : Input.buttons)
	{
		buttonState = Idle;
	}

	// Get the clock frequency
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	// Initialize time
	StartTime = Win32GetWallClock();
#endif

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	return true;
}

bool ModulePlatform::preUpdate()
{
	MSG msg = {};

	// Poll and handle messages (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	for(;;)
	{
		BOOL GotMessage = FALSE;
		DWORD LastMessage = 0;

		// NOTE(jesus): This is in case we want to skip certain messages (in sorted order)
		DWORD SkipMessages[] =
		{
			// NOTE(jesus): Add messages to skip above 0xFFFFFFFF
			0xFFFFFFFF,
		};
		for (auto Skip : SkipMessages)
		{
			GotMessage = ::PeekMessage(&msg, 0, LastMessage, Skip - 1, PM_REMOVE);
			LastMessage = Skip + 1;
			if (GotMessage)
			{
				break;
			}
		}

		if (!GotMessage)
		{
			break;
		}

		if (msg.message == WM_QUIT)
		{
			App->exit();
			return false;
		}

#ifdef ENABLE_EXTERNAL
		switch (msg.message)
		{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			if (ImGui::GetIO().WantCaptureKeyboard == false)
			{
				unsigned int VKCode = (unsigned int)msg.wParam;

				bool AltKeyWasDown = (msg.lParam & (1 << 29));
				bool ShiftKeyWasDown = (GetKeyState(VK_SHIFT) & (1 << 15));

				bool WasDown = ((msg.lParam & (1UL << 30)) != 0);
				bool IsDown =  ((msg.lParam & (1UL << 31)) == 0);

				if (WasDown != IsDown)
				{
					if (VKCode == 'W')
					{
						Win32ProcessKeyboardMessage(&Input.moveUp, IsDown);
					}
					else if (VKCode == 'A')
					{
						Win32ProcessKeyboardMessage(&Input.moveLeft, IsDown);
					}
					else if (VKCode == 'S')
					{
						Win32ProcessKeyboardMessage(&Input.moveDown, IsDown);
					}
					else if (VKCode == 'D')
					{
						Win32ProcessKeyboardMessage(&Input.moveRight, IsDown);
					}
					else if (VKCode == 'Q')
					{
						Win32ProcessKeyboardMessage(&Input.leftShoulder, IsDown);
					}
					else if (VKCode == 'E')
					{
						Win32ProcessKeyboardMessage(&Input.rightShoulder, IsDown);
					}
					else if (VKCode == VK_UP)
					{
						Win32ProcessKeyboardMessage(&Input.actionUp, IsDown);
					}
					else if (VKCode == VK_LEFT)
					{
						Win32ProcessKeyboardMessage(&Input.actionLeft, IsDown);
					}
					else if (VKCode == VK_DOWN)
					{
						Win32ProcessKeyboardMessage(&Input.actionDown, IsDown);
					}
					else if (VKCode == VK_RIGHT)
					{
						Win32ProcessKeyboardMessage(&Input.actionRight, IsDown);
					}
					else if (VKCode == VK_ESCAPE)
					{
						Win32ProcessKeyboardMessage(&Input.select, IsDown);
					}
					else if (VKCode == VK_RETURN)
					{
						Win32ProcessKeyboardMessage(&Input.start, IsDown);
					}
				}
			}
		}

#endif

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

#ifdef ENABLE_EXTERNAL

	static bool firstIteration = true;
	if (firstIteration)
	{
		StartTime = Win32GetWallClock();
		firstIteration = false;
	}

	// Time management
	EndTime = Win32GetWallClock();
	Time.deltaTime = Win32GetSecondsElapsed(StartTime, EndTime);
	Time.time += Time.deltaTime;
	StartTime = EndTime;

	// Input gamepads
	XINPUT_STATE controllerState;
	if (XInputGetState(0, &controllerState) == ERROR_SUCCESS)
	{
		XINPUT_GAMEPAD *pad = &controllerState.Gamepad;

		Win32ProcessGamepadMessage(&Input.moveUp, pad, XINPUT_GAMEPAD_DPAD_UP);
		Win32ProcessGamepadMessage(&Input.moveDown, pad, XINPUT_GAMEPAD_DPAD_DOWN);
		Win32ProcessGamepadMessage(&Input.moveLeft, pad, XINPUT_GAMEPAD_DPAD_LEFT);
		Win32ProcessGamepadMessage(&Input.moveRight, pad, XINPUT_GAMEPAD_DPAD_RIGHT);
		Win32ProcessGamepadMessage(&Input.actionUp, pad, XINPUT_GAMEPAD_X);
		Win32ProcessGamepadMessage(&Input.actionDown, pad, XINPUT_GAMEPAD_B);
		Win32ProcessGamepadMessage(&Input.actionLeft, pad, XINPUT_GAMEPAD_Y);
		Win32ProcessGamepadMessage(&Input.actionRight, pad, XINPUT_GAMEPAD_A);
		Win32ProcessGamepadMessage(&Input.leftShoulder, pad, XINPUT_GAMEPAD_LEFT_SHOULDER);
		Win32ProcessGamepadMessage(&Input.rightShoulder, pad, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		Win32ProcessGamepadMessage(&Input.select, pad, XINPUT_GAMEPAD_BACK);
		Win32ProcessGamepadMessage(&Input.start, pad, XINPUT_GAMEPAD_START);

		Input.leftThumbX = Win32ProcessGamepadThumb(
			pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		Input.leftThumbY = Win32ProcessGamepadThumb(
			pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else
	{
		// NOTE(jesus): No controller was found
	}
#endif

	return true;
}

bool ModulePlatform::postUpdate()
{
#ifdef ENABLE_EXTERNAL
	// Update buttons state
	for (auto &buttonState : Input.buttons)
	{
		if (buttonState == Press) {
			buttonState = Pressed;
		} else if (buttonState == Release) {
			buttonState = Idle;
		}
	}
#endif

	return true;
}

bool ModulePlatform::cleanUp()
{
	::DestroyWindow(hwnd);
	::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

	return true;
}
