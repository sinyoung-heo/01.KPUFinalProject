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
