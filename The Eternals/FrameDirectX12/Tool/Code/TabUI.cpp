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
}


BEGIN_MESSAGE_MAP(CTabUI, CDialogEx)
END_MESSAGE_MAP()


// CTabUI 메시지 처리기


BOOL CTabUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

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
