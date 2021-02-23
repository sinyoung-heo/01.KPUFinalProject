// ../Code/Tab2DUI.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Tab2DUI.h"
#include "afxdialogex.h"


// CTab2DUI 대화 상자

IMPLEMENT_DYNAMIC(CTab2DUI, CDialogEx)

CTab2DUI::CTab2DUI(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_2DUI, pParent)
{

}

CTab2DUI::~CTab2DUI()
{
}

void CTab2DUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTab2DUI, CDialogEx)
END_MESSAGE_MAP()


// CTab2DUI 메시지 처리기
