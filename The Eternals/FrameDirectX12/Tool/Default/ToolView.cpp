
// ToolView.cpp: CToolView 클래스의 구현
//

#include "stdafx.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "ToolMainApp.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND		g_hWnd;
HINSTANCE	g_hInst;

_bool	g_bIsLoadingStart	= false;
_bool	g_bIsLoadingFinish	= false;
_int	g_iFPS				= 0;

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CScrollView)

BEGIN_MESSAGE_MAP(CToolView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	Engine::Safe_Release(m_pMainApp);
	CMouseMgr::Get_Instance()->Destroy_Instance();
	CShadowLightMgr::Get_Instance()->Destroy_Instance();

	Engine::CFrameMgr::Get_Instance()->Destroy_Instance();
	Engine::CTimerMgr::Get_Instance()->Destroy_Instance();
	Engine::CDirectInput::Get_Instance()->Destroy_Instance();
	Engine::CDirectSound::Get_Instance()->Destroy_Instance();
	Engine::CManagement::Get_Instance()->Destroy_Instance();
	Engine::CRenderer::Get_Instance()->Destroy_Instance();
	Engine::CLightMgr::Get_Instance()->Destroy_Instance();
	Engine::CObjectMgr::Get_Instance()->Destroy_Instance();
	Engine::CComponentMgr::Get_Instance()->Destroy_Instance();
	Engine::CFrustumMgr::Get_Instance()->Destroy_Instance();
	Engine::CGraphicDevice::Get_Instance()->Destroy_Instance();

}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_hWnd	= m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	SetScrollSizes(MM_TEXT, CSize(WINCX, WINCY));

	// ToolView 크기 조정.
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());

	RECT rcMainWnd = {};
	pMainFrame->GetWindowRect(&rcMainWnd);

	SetRect(&rcMainWnd, 
			0, 
			0,
			rcMainWnd.right - rcMainWnd.left, 
			rcMainWnd.bottom - rcMainWnd.top);

	RECT rcView = {};
	GetClientRect(&rcView);

	int iRowFrm = rcMainWnd.right - rcView.right;
	int iColFrm = rcMainWnd.bottom - rcView.bottom;

	pMainFrame->SetWindowPos(nullptr, 
							 WINPOS_X, 
							 WINPOS_Y, 
							 WINCX + iRowFrm, 
							 WINCY + iColFrm, 
							 SWP_NOZORDER);

	/*__________________________________________________________________________________________________________
	[ MainApp 생성 ]
	____________________________________________________________________________________________________________*/
	m_pMainApp = CToolMainApp::Create();

	/*__________________________________________________________________________________________________________
	[ Timer 생성 ]
	Timer_TimeDelta : GameObject들이 공통적으로 사용할 Timer
	Timer_FPS60		: FPS를 60으로 제한하기 위해 사용할 Timer
	____________________________________________________________________________________________________________*/
	Engine::CTimerMgr::Get_Instance()->Ready_Timer(L"Timer_TimeDelta");
	Engine::CTimerMgr::Get_Instance()->Ready_Timer(L"Timer_FPS60");

	/*__________________________________________________________________________________________________________
	[ Frame 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CFrameMgr::Get_Instance()->Ready_Frame(L"Frame60", 60.f);

}
