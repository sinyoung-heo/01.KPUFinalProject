// TabCollider.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabCollider.h"
#include "afxdialogex.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"

// CTabCollider 대화 상자

IMPLEMENT_DYNAMIC(CTabCollider, CDialogEx)

CTabCollider::CTabCollider(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabCollider, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
{

}

CTabCollider::~CTabCollider()
{
}

void CTabCollider::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabCollider, CDialogEx)
END_MESSAGE_MAP()


// CTabCollider 메시지 처리기


BOOL CTabCollider::PreTranslateMessage(MSG* pMsg)
{
	/// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
