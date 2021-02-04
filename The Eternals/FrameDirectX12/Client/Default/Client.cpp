// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"
#include "GraphicDevice.h"
#include "TimeMgr.h"
#include "FrameMgr.h"
#include "DirectInput.h"
#include "DirectSound.h"
#include "Management.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "FrustumMgr.h"
#include <chrono>

#define MAX_LOADSTRING 100

// 전역 변수:
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

/*__________________________________________________________________________________________________________
[ Extern ]
____________________________________________________________________________________________________________*/
HINSTANCE	g_hInst;							// 현재 인스턴스입니다.
HWND		g_hWnd;

_bool		g_bIsOnDebugCaemra = true;


_ulong Release_Singleton();

#ifdef SERVER
// 해당 클라이언트 전역 소켓
SOCKET	g_hSocket;

/* 클라이언트가 서버에 접속을 했을 때, 서버로부터 받는 ID */
int		g_iSNum = -1;

/* 해당 클라이언트가 선택되었음을 나타냄 */
bool	g_bIsActive = false;
#endif // SERVER



// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	MSG msg;
	msg.message = WM_NULL;

	/*__________________________________________________________________________________________________________
	[ MainApp 생성 ]
	____________________________________________________________________________________________________________*/
	CMainApp* pMainApp = CMainApp::Create();
	if (nullptr == pMainApp)
		return FALSE;

	// FPS
	TCHAR	szFPS[32]	= L"";
	int		iFPS		= 0;
	float	fFPSTime	= 1.0f;

	// time
	chrono::milliseconds algorithm_time {};

	/*__________________________________________________________________________________________________________
	[ Timer 생성 ]
	Timer_TimeDelta : GameObject들이 공통적으로 사용할 Timer
	Timer_FPS60		: FPS를 60으로 제한하기 위해 사용할 Timer
	____________________________________________________________________________________________________________*/
	if (FAILED(Engine::CTimerMgr::Get_Instance()->Ready_Timer(L"Timer_TimeDelta")))
	{
		Engine::CTimerMgr::Get_Instance()->Destroy_Instance();
		return FALSE;
	}
	if (FAILED(Engine::CTimerMgr::Get_Instance()->Ready_Timer(L"Timer_FPS60")))
	{
		Engine::CTimerMgr::Get_Instance()->Destroy_Instance();
		return FALSE;
	}

	/*__________________________________________________________________________________________________________
	[ Frame 생성 ]
	____________________________________________________________________________________________________________*/
	if (FAILED(Engine::CFrameMgr::Get_Instance()->Ready_Frame(L"Frame60", 60.f)))
	{
		Engine::CFrameMgr::Get_Instance()->Destroy_Instance();
		return FALSE;
	}

	// 기본 메시지 루프입니다:
	while (true)
	{
		/*__________________________________________________________________________________________________________
		[ 종료 메시지가 발생할 때 까지 Game Loop 실행 ]
		____________________________________________________________________________________________________________*/
		if (WM_QUIT == msg.message)
			break;

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/*__________________________________________________________________________________________________________
			[ 프레임 제한에 사용할 Timer를 갱신 (Timer_FPS60) ]
			____________________________________________________________________________________________________________*/
			Engine::CTimerMgr::Get_Instance()->SetUp_TimeDelta(L"Timer_FPS60");
			_float fTime_FPS60 = Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_FPS60");

			/*__________________________________________________________________________________________________________
			[ 1초에 60번만 호출되도록 조건문 사용 ]
			____________________________________________________________________________________________________________*/
			if (Engine::CFrameMgr::Get_Instance()->IsPermit_Call(L"Frame60", fTime_FPS60))
			{
				/*__________________________________________________________________________________________________________
				[ GameObject들이 공통적으로 사용할 Timer 갱신 (Timer_TimeDelta) ]
				____________________________________________________________________________________________________________*/
				Engine::CTimerMgr::Get_Instance()->SetUp_TimeDelta(L"Timer_TimeDelta");
				_float fTime_TimeDelta = Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta");

				/*__________________________________________________________________________________________________________
				[ MainApp - Update & Rendering ]
				____________________________________________________________________________________________________________*/
				chrono::steady_clock::time_point start = chrono::steady_clock::now();
				
				pMainApp->Update_MainApp(fTime_TimeDelta);
				pMainApp->LateUpdate_MainApp(fTime_TimeDelta);
				pMainApp->Render_MainApp(fTime_TimeDelta);

				algorithm_time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);

				++iFPS;
				fFPSTime += fTime_TimeDelta;
			}

			if (fFPSTime >= 1.0f)
			{
				swprintf_s(szFPS, L"FPS : %d     GameLogic : %dms", iFPS, algorithm_time);
				SetWindowText(g_hWnd, szFPS);

				iFPS		= 0;
				fFPSTime	= 0.0f;
			}

		}
	}

	Engine::CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();

	/*__________________________________________________________________________________________________________
	[ Delete MainApp ]
	- 강제적으로 RefCnt를 0으로 만들어준다.
	____________________________________________________________________________________________________________*/
	_ulong dwRefCnt = 0;

	if (dwRefCnt = Engine::Safe_Release(pMainApp))
	{
		MSG_BOX(L"MainApp Release Failed");
		return FALSE;
	}

	/*__________________________________________________________________________________________________________
	[ Destroy Singleton ]
	____________________________________________________________________________________________________________*/
	if (dwRefCnt = Release_Singleton())
	{
		MSG_BOX(L"Singleton Release Failed");
		return FALSE;
	}

	return (int) msg.wParam;
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

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rc{ 0, 0, WINCX, WINCY };

	// AdjustWindowRect: 윈도우의 View영역을 사용자가 지정한 RECT 크기만큼 조정하는 함수.
	// 조정된 View영역의 크기가 반영된 전체 윈도우 프레임 크기를 반환.
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
							  WINPOS_X, WINPOS_Y,
							  rc.right - rc.left, 
							  rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 메뉴 선택을 구문 분석합니다:
			switch (wmId)
			{
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(g_hWnd);
			break;

		default:
			break;
		}
		break;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			g_bIsActive = false;
		else
			g_bIsActive = true;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

_ulong Release_Singleton()
{
#ifdef CLIENT_LOG
	COUT_STR("-------------------------");
	COUT_STR("Destroy Singleton Object");
	COUT_STR("-------------------------");
#endif
	_ulong dwRefCnt = 0;

	if (dwRefCnt = CPacketMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CPacketMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CServerMath::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CServerMath Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CShadowLightMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CShadowLightMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CFrameMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CFrameMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CTimerMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CTimerMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CMathMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CMathMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CDirectInput::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CDirectInput Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CDirectSound::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CDirectSound Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CManagement::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CManagement Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CRenderer::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CRenderer Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CLightMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CLightMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CObjectMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CObjectMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CComponentMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"Engine::CComponentMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CFrustumMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CFrustumMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CGraphicDevice::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CDirectInput Release Failed");
		return dwRefCnt;
	}

	return dwRefCnt;
}