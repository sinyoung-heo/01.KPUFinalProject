// TabEffect.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabEffect.h"
#include "afxdialogex.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"

// CTabEffect 대화 상자

IMPLEMENT_DYNAMIC(CTabEffect, CDialogEx)

CTabEffect::CTabEffect(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabEffect, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
{

}

CTabEffect::~CTabEffect()
{
}

void CTabEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabEffect, CDialogEx)
END_MESSAGE_MAP()


// CTabEffect 메시지 처리기


BOOL CTabEffect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
