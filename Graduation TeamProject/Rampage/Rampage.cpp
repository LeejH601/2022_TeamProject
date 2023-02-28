#include "Global\stdafx.h"
#include "Global\GameFramework.h"
#include "Global/Camera.h"
#include "Global/Locator.h"
#include "Object/State.h"
#include "Global/EntityManager.h"
#include "Sound/SoundManager.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.

// 게임 프로그램의 골격을 나타내는 CGameFramework 클래스 객체
CGameFramework gGameFramework;
std::default_random_engine dre;
CLocator Locator;
physx::PxDefaultErrorCallback gDefaultErrorCallback;
physx::PxDefaultAllocator gDefaultAllocatorCallback;
bool b_simulation = false;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//Init FMOD
	CSoundManager::GetInst()->Init();

	Locator.Init();
	Locator.SetTimer(&gGameFramework.m_GameTimer);

	DataLoader loader;
	loader.LoadComponentSets(Locator.GetComponentSetRoot());
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 1 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 2 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 3 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 4 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Action 5 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 1 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 2 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 3 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 4 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);
	CSoundManager::GetInst()->RegisterSound("Sound/Background/Light Ambient 5 (Loop).wav", true, SOUND_CATEGORY::SOUND_BACKGROUND);

	//CSoundManager::GetInst()->PlaySound("Sound/Background/Action 1 (Loop).wav");
	//CSoundManager::GetInst()->PlaySound("Sound/Background/Light Ambient 1 (Loop).wav");

	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, TEXT("Rampage"));

	MSG msg;

	// 메세지 루프 입니다.
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
		}
	}
	loader.SaveComponentSets(Locator.GetComponentSetRoot());


	gGameFramework.OnDestroy();

	Locator.GetPxScene()->release();


	// Release Fmod Llibrary
	CSoundManager::GetInst()->Release();
	return 0;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(NULL));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("Rampage");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(NULL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

	AdjustWindowRect(&rc, dwStyle, FALSE);

	HWND hMainWnd = CreateWindow(TEXT("Rampage"), TEXT("Rampage"), dwStyle, FIRST_WINDOW_POS_WIDTH, FIRST_WINDOW_POS_HEIGHT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}