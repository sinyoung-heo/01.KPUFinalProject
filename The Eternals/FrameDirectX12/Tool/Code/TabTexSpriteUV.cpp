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
END_MESSAGE_MAP()


// CTabTexSpriteUV 메시지 처리기
