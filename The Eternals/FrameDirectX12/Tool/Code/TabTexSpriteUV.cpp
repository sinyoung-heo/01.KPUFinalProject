// ../Code/TabTexSpriteUV.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabTexSpriteUV.h"
#include "afxdialogex.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"


// CTabTexSpriteUV 대화 상자

IMPLEMENT_DYNAMIC(CTabTexSpriteUV, CDialogEx)

CTabTexSpriteUV::CTabTexSpriteUV(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_TexSpriteUV, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
{

}

CTabTexSpriteUV::~CTabTexSpriteUV()
{
}

void CTabTexSpriteUV::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2000, m_TexUITreeCtrl);
}


BEGIN_MESSAGE_MAP(CTabTexSpriteUV, CDialogEx)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CTabTexSpriteUV 메시지 처리기


BOOL CTabTexSpriteUV::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_TexUITreeCtrl.EnableWindow(FALSE);

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

HRESULT CTabTexSpriteUV::Ready_TabTexSpriteUV()
{
	UpdateData(TRUE);

	m_TexUITreeCtrl.EnableWindow(TRUE);



	// Ready Mesh TreeControl.
	HTREEITEM h_Texture, h_TextureRoot;

	h_Texture = m_TexUITreeCtrl.InsertItem(L"Texture", NULL, NULL);

	wifstream fin{ L"../../Bin/ToolData/TexTreeCtrlInfo.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar	szCurTexType[MIN_STR] = L"";
	_tchar	szCurTexRoot[MIN_STR] = L"";
	_tchar	szTexTag[MAX_STR] = L"";

	wstring wstrPreTexType = L"";
	wstring wstrPreTexRoot = L"";

	while (true)
	{
		fin.getline(szCurTexType, MIN_STR, '|');
		fin.getline(szCurTexRoot, MIN_STR, '|');
		fin.getline(szTexTag, MIN_STR);

		if (fin.eof())
			break;

		wstring wstrCurTexType = szCurTexType;
		wstring wstrCurTexRoot = szCurTexRoot;

		if (L"Texture" == wstrCurTexType &&
			L"UI" == wstrCurTexRoot)
		{
			if (wstrPreTexRoot != wstrCurTexRoot)
			{
				wstrPreTexRoot = wstrCurTexRoot;
				h_TextureRoot = m_TexUITreeCtrl.InsertItem(wstrPreTexRoot.c_str(), h_Texture, NULL);
			}

			m_TexUITreeCtrl.InsertItem(szTexTag, h_TextureRoot, NULL);
		}

	}

	fin.close();


	// 모든 트리의 노드를 펼친다.
	// m_TexUITreeCtrl.Expand(h_DynamciMesh, TVE_EXPAND);

	// DynamicMesh
	//HTREEITEM h_Child = m_TexUITreeCtrl.GetNextItem(h_DynamciMesh, TVGN_CHILD);
	//m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);

	//while (h_Child != NULL)
	//{
	//	h_Child = m_TexUITreeCtrl.GetNextItem(h_Child, TVGN_NEXT);
	//	m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);
	//}

	// Texture
	m_TexUITreeCtrl.Expand(h_Texture, TVE_EXPAND);

	HTREEITEM h_Child = m_TexUITreeCtrl.GetNextItem(h_Texture, TVGN_CHILD);
	m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_TexUITreeCtrl.GetNextItem(h_Child, TVGN_NEXT);
		m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);
	}

	UpdateData(FALSE);

	return S_OK;
}
