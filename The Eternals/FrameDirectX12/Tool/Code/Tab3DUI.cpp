// ../Code/Tab3DUI.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Tab3DUI.h"
#include "afxdialogex.h"


// CTab3DUI 대화 상자

IMPLEMENT_DYNAMIC(CTab3DUI, CDialogEx)

CTab3DUI::CTab3DUI(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_3DUI, pParent)
{

}

CTab3DUI::~CTab3DUI()
{
}

void CTab3DUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTab3DUI, CDialogEx)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CTab3DUI 메시지 처리기


BOOL CTab3DUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTab3DUI::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTab3DUI::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
