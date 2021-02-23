// ../Code/TabTexSpriteUV.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabTexSpriteUV.h"
#include "afxdialogex.h"


// CTabTexSpriteUV 대화 상자

IMPLEMENT_DYNAMIC(CTabTexSpriteUV, CDialogEx)

CTabTexSpriteUV::CTabTexSpriteUV(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_TexSpriteUV, pParent)
{

}

CTabTexSpriteUV::~CTabTexSpriteUV()
{
}

void CTabTexSpriteUV::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabTexSpriteUV, CDialogEx)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CTabTexSpriteUV 메시지 처리기


BOOL CTabTexSpriteUV::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabTexSpriteUV::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTabTexSpriteUV::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
