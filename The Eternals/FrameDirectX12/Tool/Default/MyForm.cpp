// MyForm.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_FORMVIEW)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyForm::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CMyForm 진단

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기

void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_Tab.InsertItem(0, _T("PathFinder"));
	m_Tab.InsertItem(1, _T("Map"));
	m_Tab.InsertItem(2, _T("Collider"));
	m_Tab.InsertItem(3, _T("Animation"));
	m_Tab.InsertItem(4, _T("Camera"));
	m_Tab.InsertItem(5, _T("Effect"));

	m_Tab.SetCurFocus(0);

	CRect rect;
	m_Tab.GetWindowRect(&rect);

	m_TabPathFinder.Create(IDD_CTabPathFinder, &m_Tab);
	m_TabPathFinder.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabPathFinder.ShowWindow(SW_SHOW);

	m_TabObject.Create(IDD_CTabObject, &m_Tab);
	m_TabObject.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabObject.ShowWindow(SW_HIDE);

	m_TabCollider.Create(IDD_CTabCollider, &m_Tab);
	m_TabCollider.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabCollider.ShowWindow(SW_HIDE);

	m_TabAnimation.Create(IDD_CTabAnimation, &m_Tab);
	m_TabAnimation.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabAnimation.ShowWindow(SW_HIDE);

	m_TabCamera.Create(IDD_CTabCamera, &m_Tab);
	m_TabCamera.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabCamera.ShowWindow(SW_HIDE);

	m_TabEffect.Create(IDD_CTabEffect, &m_Tab);
	m_TabEffect.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabEffect.ShowWindow(SW_HIDE);

}


void CMyForm::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSelect = m_Tab.GetCurFocus();

	switch (iSelect)
	{
	case 0:
		m_TabPathFinder.ShowWindow(SW_SHOW);
		m_TabObject.ShowWindow(SW_HIDE);
		m_TabCollider.ShowWindow(SW_HIDE);
		m_TabAnimation.ShowWindow(SW_HIDE);
		m_TabCamera.ShowWindow(SW_HIDE);
		m_TabEffect.ShowWindow(SW_HIDE);

		m_bIsTabPathFinder	= true;
		m_bIsTabObject		= false;
		m_bIsTabCollider	= false;
		m_bIsTabAnimation	= false;
		m_bIsTabCamera		= false;
		m_bIsTabEffect		= false;

		break;

	case 1:

		m_TabPathFinder.ShowWindow(SW_HIDE);
		m_TabObject.ShowWindow(SW_SHOW);
		m_TabCollider.ShowWindow(SW_HIDE);
		m_TabAnimation.ShowWindow(SW_HIDE);
		m_TabCamera.ShowWindow(SW_HIDE);
		m_TabEffect.ShowWindow(SW_HIDE);

		m_bIsTabPathFinder	= false;
		m_bIsTabObject		= true;
		m_bIsTabCollider	= false;
		m_bIsTabAnimation	= false;
		m_bIsTabCamera		= false;
		m_bIsTabEffect		= false;

		g_bIsLoadingStart = true;


		break;
	case 2:
		g_bIsLoadingStart = true;


		m_TabPathFinder.ShowWindow(SW_HIDE);
		m_TabObject.ShowWindow(SW_HIDE);
		m_TabCollider.ShowWindow(SW_SHOW);
		m_TabAnimation.ShowWindow(SW_HIDE);
		m_TabCamera.ShowWindow(SW_HIDE);
		m_TabEffect.ShowWindow(SW_HIDE);

		m_bIsTabPathFinder	= false;
		m_bIsTabObject		= false;
		m_bIsTabCollider	= true;
		m_bIsTabAnimation	= false;
		m_bIsTabCamera		= false;
		m_bIsTabEffect		= false;


		break;
	case 3:
		g_bIsLoadingStart = true;

		m_TabPathFinder.ShowWindow(SW_HIDE);
		m_TabObject.ShowWindow(SW_HIDE);
		m_TabCollider.ShowWindow(SW_HIDE);
		m_TabAnimation.ShowWindow(SW_SHOW);
		m_TabCamera.ShowWindow(SW_HIDE);
		m_TabEffect.ShowWindow(SW_HIDE);

		m_bIsTabPathFinder	= false;
		m_bIsTabObject		= false;
		m_bIsTabCollider	= false;
		m_bIsTabAnimation	= true;
		m_bIsTabCamera		= false;
		m_bIsTabEffect		= false;

		break;
	case 4:
		g_bIsLoadingStart = true;

		m_TabPathFinder.ShowWindow(SW_HIDE);
		m_TabObject.ShowWindow(SW_HIDE);
		m_TabCollider.ShowWindow(SW_HIDE);
		m_TabAnimation.ShowWindow(SW_HIDE);
		m_TabCamera.ShowWindow(SW_SHOW);
		m_TabEffect.ShowWindow(SW_HIDE);

		m_bIsTabPathFinder	= false;
		m_bIsTabObject		= false;
		m_bIsTabCollider	= false;
		m_bIsTabAnimation	= false;
		m_bIsTabCamera		= true;
		m_bIsTabEffect		= false;

		break;

	case 5:
		g_bIsLoadingStart = true;

		m_TabPathFinder.ShowWindow(SW_HIDE);
		m_TabObject.ShowWindow(SW_HIDE);
		m_TabCollider.ShowWindow(SW_HIDE);
		m_TabAnimation.ShowWindow(SW_HIDE);
		m_TabCamera.ShowWindow(SW_HIDE);
		m_TabEffect.ShowWindow(SW_SHOW);

		m_bIsTabPathFinder	= false;
		m_bIsTabObject		= false;
		m_bIsTabCollider	= false;
		m_bIsTabAnimation	= false;
		m_bIsTabCamera		= false;
		m_bIsTabEffect		= true;

		break;
	}

}



BOOL CMyForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CFormView::PreTranslateMessage(pMsg);
}
