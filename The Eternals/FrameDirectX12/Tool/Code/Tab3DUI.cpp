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
END_MESSAGE_MAP()


// CTab3DUI 메시지 처리기
