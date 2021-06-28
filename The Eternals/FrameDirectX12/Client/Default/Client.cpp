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
#include "DescriptorHeapMgr.h"
#include "CollisionMgr.h"
#include "InstancePoolMgr.h"
#include "PartySystemMgr.h"
#include "ChattingMgr.h"
#include "InventoryEquipmentMgr.h"
#include "StoreMgr.h"
#include "QuickSlotMgr.h"
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

_bool g_bIsOnDebugCaemra = false;
_bool g_bIsStageChange   = false;
_bool g_bIsOpenShop		 = false;
_bool g_bIsLoadingFinish = false;
_bool g_bIsExitGame      = false;
_bool g_bIsChattingInput = false;
_bool g_bIsGameStart	 = false;

_ulong Release_Singleton();

#if defined(IME_KOREAN_SAMPLE)
/*__________________________________________________________________________________________________________
[ Chatting ]
____________________________________________________________________________________________________________*/
_tchar g_Text[256];     // 텍스트를 저장하기위한 변수
_tchar Cstr[10];      // 조합중인 문자!!

int Get_Text(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

#ifdef SERVER
// 해당 클라이언트 전역 소켓
SOCKET	g_hSocket;

/* 클라이언트가 서버에 접속을 했을 때, 서버로부터 받는 ID */
int		g_iSNum = -1;

/* 해당 클라이언트가 선택되었음을 나타냄 */
bool	g_bIsActive = false;

const _int MAX_ID_PWD_LEN = 16;
_tchar	g_szID[32]      = L"";
_tchar	g_szPWD[32]     = L"";
_bool	g_bIsInputID  = false;
_bool	g_bIsInputPWD = false;
_bool	g_bIsLoginID  = false;

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

	//locale("korean");
	_wsetlocale(LC_ALL, L"korean");

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

		if (g_bIsExitGame)
		{
			DestroyWindow(g_hWnd);
			break;
		}

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
				
				pMainApp->Process_PacketFromServer();
				pMainApp->Update_MainApp(fTime_TimeDelta);
				pMainApp->LateUpdate_MainApp(fTime_TimeDelta);
				pMainApp->Send_PacketToServer();
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
#if defined(IME_KOREAN_SAMPLE)
	/*__________________________________________________________________________________________________________
	[ Chatting ]
	____________________________________________________________________________________________________________*/
	if (Get_Text(hWnd, message, wParam, lParam) == 0)		//IME 처리는 따로 함수로 분리시킴.
	{
		return 0;
	}
#endif

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
			// DestroyWindow(g_hWnd);
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
		//CPacketMgr::Get_Instance()->send_logout();
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

	if (dwRefCnt = CQuickSlotMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CQuickSlotMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CStoreMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CStoreMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CInventoryEquipmentMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CInventoryEquipmentMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CInventoryEquipmentMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CInventoryEquipmentMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CChattingMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CChattingMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CEffectMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CEffectMgr Release Failed");
		return dwRefCnt;
	}

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

	if (dwRefCnt = CPartySystemMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CPartySystemMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CMouseCursorMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CMouseCursorMgr Release Failed");
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

	if (dwRefCnt = Engine::CDescriptorHeapMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CDescriptorHeapMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CCollisionMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CCollisionMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CObjectMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CObjectMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = CInstancePoolMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CInstancePoolMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CComponentMgr::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"Engine::CComponentMgr Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt = Engine::CGraphicDevice::Get_Instance()->Destroy_Instance())
	{
		MSG_BOX(L"CDirectInput Release Failed");
		return dwRefCnt;
	}

	return dwRefCnt;
}


#if defined(IME_KOREAN_SAMPLE)
/*__________________________________________________________________________________________________________
[ Chatting ]
____________________________________________________________________________________________________________*/
int Get_Text(HWND hWnd,UINT msg,WPARAM wparam, LPARAM lparam)
{
	//if (!INPUT_CHATTING)
	//	return 1;
	//if (!g_bIsChattingInput)
	//	return 1;

	int len = 0;     		
	HIMC hIMC = NULL;   // IME 핸들

	switch (msg)
	{
	case WM_IME_COMPOSITION:
		if (g_bIsChattingInput && INPUT_CHATTING)
		{
			hIMC = ImmGetContext(hWnd);	// ime핸들을 얻는것

			if (lstrlen(g_Text) > CChattingMgr::Get_Instance()->Get_MaxChattingLength())
			{
				return 0;
				break;
			}

			if (lparam & GCS_RESULTSTR)
			{
				if ((len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
				{
					// 완성된 글자가 있다.
					ImmGetCompositionString(hIMC, GCS_RESULTSTR, Cstr, len);
					Cstr[len] = 0;
					lstrcpy(g_Text + lstrlen(g_Text), Cstr);

					memset(Cstr, 0, 10);

					{
						_tchar szTemp[256] = L"";
						wsprintf(szTemp, L"완성된 글자 : %s\r\n", g_Text);

						CChattingMgr::Get_Instance()->Set_ChattingInputString(g_Text);
						CChattingMgr::Get_Instance()->Move_CursorPos(CHATTING_CURSOR_MOVE::MOVE_RIGHT);
					}
				}

			}
			else if (lparam & GCS_COMPSTR)
			{
				// 현재 글자를 조합 중이다.

				// 조합중인 길이를 얻는다. 
				// str에  조합중인 문자를 얻는다.
				len = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
				ImmGetCompositionString(hIMC, GCS_COMPSTR, Cstr, len);
				Cstr[len] = 0;
			}

			ImmReleaseContext(hWnd, hIMC);					// IME 핸들 반환!!
		}
		
		return 0;
		break;
	case WM_CHAR:				// 1byte 문자 (ex : 영어)

		return 0;
		break;
	case WM_IME_NOTIFY:			// 한자입력...

		return 0;
		break;
	case WM_KEYDOWN:			// 키다운..
		switch (wparam)
		{
		case VK_BACK:
		{
			if (g_bIsChattingInput && INPUT_CHATTING)
			{
				if (lstrlen(g_Text) > 0)
				{
					CChattingMgr::Get_Instance()->Move_CursorPos(CHATTING_CURSOR_MOVE::MOVE_LEFT);

					wstring wstrTemp = g_Text;
					wstrTemp.pop_back();
					lstrcpy(g_Text, wstrTemp.c_str());
					CChattingMgr::Get_Instance()->Set_ChattingInputString(g_Text);
				}
			}

			// ID
			if (g_bIsInputID && !g_bIsInputPWD)
			{
				if (lstrlen(g_szID) > 0)
				{
					wstring wstrTemp = g_szID;
					wstrTemp.pop_back();
					lstrcpy(g_szID, wstrTemp.c_str());
				}
			}
			// PWD
			else if (g_bIsInputPWD && !g_bIsInputID)
			{
				if (lstrlen(g_szPWD) > 0)
				{
					wstring wstrTemp = g_szPWD;
					wstrTemp.pop_back();
					lstrcpy(g_szPWD, wstrTemp.c_str());
				}
			}
		}
			break;
		case VK_PROCESSKEY:
			break;
		case VK_RETURN:
			break;
		default:
			if (g_bIsChattingInput && INPUT_CHATTING)
			{
				if (lstrlen(g_Text) > CChattingMgr::Get_Instance()->Get_MaxChattingLength())
				{
					return 0;
					break;
				}

				// 0~9 && A~Z
				if ((wparam >= 0x30 && wparam <= 0x39) ||
					(wparam >= 0x41 && wparam <= 0x5A))
				{
					wstring wstr = L"";
					wstr = wparam;
					lstrcat(g_Text, wstr.c_str());

					CChattingMgr::Get_Instance()->Set_ChattingInputString(g_Text);
					CChattingMgr::Get_Instance()->Move_CursorPos(CHATTING_CURSOR_MOVE::MOVE_RIGHT);
				}
			}

			// ID 입력
			if (g_bIsInputID && !g_bIsInputPWD)
			{
				if (lstrlen(g_szID) > MAX_ID_PWD_LEN)
				{
					return 0;
					break;
				}

				// 0~9 && A~Z
				if ((wparam >= 0x30 && wparam <= 0x39) ||
					(wparam >= 0x41 && wparam <= 0x5A))
				{
					wstring wstr = L"";
					wstr = wparam;
					lstrcat(g_szID, wstr.c_str());
				}
			}
			// PWD 입력
			else if (g_bIsInputPWD && !g_bIsInputID)
			{
				if (lstrlen(g_szPWD) > MAX_ID_PWD_LEN)
				{
					return 0;
					break;
				}

				// 0~9 && A~Z
				if ((wparam >= 0x30 && wparam <= 0x39) ||
					(wparam >= 0x41 && wparam <= 0x5A))
				{
					wstring wstr = L"";
					wstr = wparam;
					lstrcat(g_szPWD, wstr.c_str());
				}
			}


			break;
		}

		return 0;
		break;
	}

	return 1;
}
#endif
