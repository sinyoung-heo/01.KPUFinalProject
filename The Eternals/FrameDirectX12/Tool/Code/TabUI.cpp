// ../Code/TabUI.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabUI.h"
#include "afxdialogex.h"

// CTabUI 대화 상자

IMPLEMENT_DYNAMIC(CTabUI, CDialogEx)

CTabUI::CTabUI(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI, pParent)
{

}

CTabUI::~CTabUI()
{
}

void CTabUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabUI);
}


BEGIN_MESSAGE_MAP(CTabUI, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTabUI::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CTabUI 메시지 처리기


BOOL CTabUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_TabUI.InsertItem(0, _T("TexSprite UV"));
	m_TabUI.InsertItem(1, _T("2D UI"));
	m_TabUI.InsertItem(2, _T("3D UI"));

	m_TabUI.SetCurFocus(0);

	CRect rect;
	m_TabUI.GetWindowRect(&rect);

	m_TabTexSpriteUV.Create(IDD_CTabUI_TexSpriteUV, &m_TabUI);
	m_TabTexSpriteUV.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_TabTexSpriteUV.ShowWindow(SW_SHOW);

	m_Tab2DUI.Create(IDD_CTabUI_2DUI, &m_TabUI);
	m_Tab2DUI.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_Tab2DUI.ShowWindow(SW_HIDE);

	m_Tab3DUI.Create(IDD_CTabUI_3DUI, &m_TabUI);
	m_Tab3DUI.MoveWindow(0, 25, rect.Width(), rect.Height());
	m_Tab3DUI.ShowWindow(SW_HIDE);

	m_bIsTabTexSpriteUV = false;
	m_bIsTab2DUI        = false;
	m_bIsTab3DUI        = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabUI::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTabUI::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSelect = m_TabUI.GetCurFocus();

	switch (iSelect)
	{
	case 0:
		m_TabTexSpriteUV.ShowWindow(SW_SHOW);
		m_Tab2DUI.ShowWindow(SW_HIDE);
		m_Tab3DUI.ShowWindow(SW_HIDE);

		m_bIsTabTexSpriteUV = true;
		m_bIsTab2DUI        = false;
		m_bIsTab3DUI        = false;

		break;

	case 1:
		m_TabTexSpriteUV.ShowWindow(SW_HIDE);
		m_Tab2DUI.ShowWindow(SW_SHOW);
		m_Tab3DUI.ShowWindow(SW_HIDE);

		m_bIsTabTexSpriteUV = false;
		m_bIsTab2DUI        = true;
		m_bIsTab3DUI        = false;

		break;
	case 2:
		m_TabTexSpriteUV.ShowWindow(SW_HIDE);
		m_Tab2DUI.ShowWindow(SW_HIDE);
		m_Tab3DUI.ShowWindow(SW_SHOW);

		m_bIsTabTexSpriteUV = false;
		m_bIsTab2DUI        = false;
		m_bIsTab3DUI        = true;

		break;
	}

}
