// TabObject.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabObject.h"
#include "afxdialogex.h"


// CTabObject 대화 상자

IMPLEMENT_DYNAMIC(CTabObject, CDialogEx)

CTabObject::CTabObject(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabObject, pParent)
{

}

CTabObject::~CTabObject()
{
}

void CTabObject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabObject, CDialogEx)
END_MESSAGE_MAP()


// CTabObject 메시지 처리기


BOOL CTabObject::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
