// ../Code/Tab2DUI.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Tab2DUI.h"
#include "afxdialogex.h"
#include "GraphicDevice.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "DescriptorHeapMgr.h"
#include "ToolUIRoot.h"
#include "ToolUIChild.h"
#include "ToolSceneStage.h"


// CTab2DUI 대화 상자

IMPLEMENT_DYNAMIC(CTab2DUI, CDialogEx)

CTab2DUI::CTab2DUI(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_2DUI, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pDescriptorHeapMgr(Engine::CDescriptorHeapMgr::Get_Instance())
{

}

CTab2DUI::~CTab2DUI()
{
}

void CTab2DUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2100, m_ListBoxRootUI);
	DDX_Control(pDX, IDC_EDIT2100, m_EditRootUITag);
	DDX_Control(pDX, IDC_EDIT2101, m_EditDataFileName);
	DDX_Control(pDX, IDC_EDIT2102, m_EditObjectTag);
	DDX_Control(pDX, IDC_EDIT2103, m_EditRootPosX);
	DDX_Control(pDX, IDC_EDIT2104, m_EditRootPosY);
	DDX_Control(pDX, IDC_EDIT2106, m_EditRootScaleX);
	DDX_Control(pDX, IDC_EDIT2107, m_EditRootScaleY);
	DDX_Control(pDX, IDC_EDIT2105, m_EditRootUIDepth);
	DDX_Control(pDX, IDC_CHECK2100, m_CheckRootIsAnimation);
	DDX_Control(pDX, IDC_EDIT2111, m_EditRootRectPosOffsetX);
	DDX_Control(pDX, IDC_EDIT2112, m_EditRootRectPosOffsetY);
	DDX_Control(pDX, IDC_EDIT2113, m_EditRootRectScaleX);
	DDX_Control(pDX, IDC_EDIT2114, m_EditRootRectScaleY);
	DDX_Text(pDX, IDC_EDIT2100, m_wstrRootUITag);
	DDX_Text(pDX, IDC_EDIT2101, m_wstrRootDataFileName);
	DDX_Text(pDX, IDC_EDIT2102, m_wstrRootObjectTag);
	DDX_Text(pDX, IDC_EDIT2103, m_fRootPosX);
	DDX_Text(pDX, IDC_EDIT2104, m_fRootPosY);
	DDX_Text(pDX, IDC_EDIT2106, m_fRootScaleX);
	DDX_Text(pDX, IDC_EDIT2107, m_fRootScaleY);
	DDX_Text(pDX, IDC_EDIT2105, m_RootUIDepth);
	DDX_Text(pDX, IDC_EDIT2111, m_fRootRectPosOffsetX);
	DDX_Text(pDX, IDC_EDIT2112, m_fRootRectPosOffsetY);
	DDX_Text(pDX, IDC_EDIT2113, m_fRootRectScaleX);
	DDX_Text(pDX, IDC_EDIT2114, m_fRootRectScaleY);
	DDX_Control(pDX, IDC_EDIT2121, m_EditRootFrameSpeed);
	DDX_Text(pDX, IDC_EDIT2121, m_fRootFrameSpeed);
	DDX_Control(pDX, IDC_RADIO2100, m_RadioRootCreateMode);
	DDX_Control(pDX, IDC_RADIO2101, m_RadioRootModifyMode);
	DDX_Control(pDX, IDC_LIST2102, m_ListBoxDataFileName);
	DDX_Control(pDX, IDC_RADIO2103, m_RadioChildUICreateMode);
	DDX_Control(pDX, IDC_RADIO2104, m_RadioChildUIModifyMode);
	DDX_Control(pDX, IDC_LIST2101, m_ListBoxChildUI);
	DDX_Control(pDX, IDC_BUTTON2103, m_ButtonCreateChild);
	DDX_Control(pDX, IDC_BUTTON2102, m_ButtonDeleteChild);
	DDX_Control(pDX, IDC_EDIT2108, m_EditChildRootObjectTag);
	DDX_Control(pDX, IDC_EDIT2115, m_EditChildObjectTag);
	DDX_Control(pDX, IDC_EDIT2116, m_EditChildPosOffsetX);
	DDX_Control(pDX, IDC_EDIT2117, m_EditChildPosOffsetY);
	DDX_Control(pDX, IDC_EDIT2119, m_EditChildScaleOffsetX);
	DDX_Control(pDX, IDC_EDIT2120, m_EditChildScaleOffsetY);
	DDX_Control(pDX, IDC_EDIT2118, m_EditChildUIDepth);
	DDX_Control(pDX, IDC_CHECK2101, m_CheckChildIsAnimation);
	DDX_Control(pDX, IDC_EDIT2126, m_EditChildFrameSpeed);
	DDX_Control(pDX, IDC_EDIT2122, m_EditChildRectPosOffsetX);
	DDX_Control(pDX, IDC_EDIT2123, m_EditChildRectPosOffsetY);
	DDX_Control(pDX, IDC_EDIT2124, m_EditChildRectScaleOffsetX);
	DDX_Control(pDX, IDC_EDIT2125, m_EditChildRectScaleOffsetY);
	DDX_Control(pDX, IDC_BUTTON2106, m_ButtonChildSAVE);
	DDX_Control(pDX, IDC_BUTTON2107, m_ButtonChildLOAD);
	DDX_Text(pDX, IDC_EDIT2108, m_wstrChildRootObjectTag);
	DDX_Text(pDX, IDC_EDIT2115, m_wstrChildObjectTag);
	DDX_Text(pDX, IDC_EDIT2116, m_fChildPosOffsetX);
	DDX_Text(pDX, IDC_EDIT2117, m_fChildPosOffsetY);
	DDX_Text(pDX, IDC_EDIT2119, m_fChildScaleOffsetX);
	DDX_Text(pDX, IDC_EDIT2120, m_fChildScaleOffsetY);
	DDX_Text(pDX, IDC_EDIT2118, m_ChildUIDepth);
	DDX_Text(pDX, IDC_EDIT2126, m_fChildFrameSpeed);
	DDX_Text(pDX, IDC_EDIT2122, m_fChildRectPosOffsetX);
	DDX_Text(pDX, IDC_EDIT2123, m_fChildRectPosOffsetY);
	DDX_Text(pDX, IDC_EDIT2124, m_fChildRectScaleOffsetX);
	DDX_Text(pDX, IDC_EDIT2125, m_fChildRectScaleOffsetY);
}


BEGIN_MESSAGE_MAP(CTab2DUI, CDialogEx)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_CHECK2100, &CTab2DUI::OnBnClickedCheck2100_RootUIIsAnimation)
	ON_LBN_SELCHANGE(IDC_LIST2100, &CTab2DUI::OnLbnSelchangeList2100_RootUITagSelect)
	ON_BN_CLICKED(IDC_RADIO2100, &CTab2DUI::OnBnClickedRadio2100_RootCreateMode)
	ON_BN_CLICKED(IDC_RADIO2101, &CTab2DUI::OnBnClickedRadio2101_RootModifyMode)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_LIST2102, &CTab2DUI::OnLbnSelchangeList2102_DataFileName)
	ON_BN_CLICKED(IDC_BUTTON2104, &CTab2DUI::OnBnClickedButton2104_RootUISAVE)
	ON_BN_CLICKED(IDC_BUTTON2105, &CTab2DUI::OnBnClickedButton2105_RootUILOAD)
	ON_BN_CLICKED(IDC_BUTTON2101, &CTab2DUI::OnBnClickedButton2101_DeleteRootUI)
	ON_BN_CLICKED(IDC_RADIO2103, &CTab2DUI::OnBnClickedRadio2103_ChildUICreateMode)
	ON_BN_CLICKED(IDC_RADIO2104, &CTab2DUI::OnBnClickedRadio2104_ChildUIModifyMode)
	ON_BN_CLICKED(IDC_CHECK2101, &CTab2DUI::OnBnClickedCheck2101_CheckChildIsSpriteAnimation)
	ON_BN_CLICKED(IDC_BUTTON2103, &CTab2DUI::OnBnClickedButton2103_ChildUICreate)
	ON_BN_CLICKED(IDC_BUTTON2102, &CTab2DUI::OnBnClickedButton2102_ChildUIDelete)
	ON_LBN_SELCHANGE(IDC_LIST2101, &CTab2DUI::OnLbnSelchangeList2101_SelectChildUI)
	ON_EN_CHANGE(IDC_EDIT2103, &CTab2DUI::OnEnChangeEdit2103_RootUIPosX)
	ON_EN_CHANGE(IDC_EDIT2104, &CTab2DUI::OnEnChangeEdit2104_RootUIPosY)
	ON_EN_CHANGE(IDC_EDIT2106, &CTab2DUI::OnEnChangeEdit2106_RootUIScaleX)
	ON_EN_CHANGE(IDC_EDIT2107, &CTab2DUI::OnEnChangeEdit2107_RootUIScaleY)
	ON_EN_CHANGE(IDC_EDIT2105, &CTab2DUI::OnEnChangeEdit2105_RootUIUIDepth)
	ON_EN_CHANGE(IDC_EDIT2121, &CTab2DUI::OnEnChangeEdit2121_RootUIFrameSpeed)
	ON_EN_CHANGE(IDC_EDIT2111, &CTab2DUI::OnEnChangeEdit2111_RootUIRectPosOffsetX)
	ON_EN_CHANGE(IDC_EDIT2112, &CTab2DUI::OnEnChangeEdit2112_RootUIRectPosOffsetY)
	ON_EN_CHANGE(IDC_EDIT2113, &CTab2DUI::OnEnChangeEdit2113_RootUIRectScaleOffsetX)
	ON_EN_CHANGE(IDC_EDIT2114, &CTab2DUI::OnEnChangeEdit2114_RootUIRectScaleOffsetY)
	ON_EN_CHANGE(IDC_EDIT2116, &CTab2DUI::OnEnChangeEdit2116_ChildUIPosOffsetX)
	ON_EN_CHANGE(IDC_EDIT2117, &CTab2DUI::OnEnChangeEdit2117_ChildUIPosOffsetY)
	ON_EN_CHANGE(IDC_EDIT2119, &CTab2DUI::OnEnChangeEdit2119_ChildUIScaleOffsetX)
	ON_EN_CHANGE(IDC_EDIT2120, &CTab2DUI::OnEnChangeEdit2120_ChildUIScaleOffsetY)
	ON_EN_CHANGE(IDC_EDIT2118, &CTab2DUI::OnEnChangeEdit2118_ChildUIUIDepth)
	ON_EN_CHANGE(IDC_EDIT2126, &CTab2DUI::OnEnChangeEdit2126_ChildUIFrameSpeed)
	ON_EN_CHANGE(IDC_EDIT2122, &CTab2DUI::OnEnChangeEdit2122_ChildUIRectPosOffsetX)
	ON_EN_CHANGE(IDC_EDIT2123, &CTab2DUI::OnEnChangeEdit2123_ChildUIRectPosOffsetY)
	ON_EN_CHANGE(IDC_EDIT2124, &CTab2DUI::OnEnChangeEdit2124_ChildUIRectScaleOffsetX)
	ON_EN_CHANGE(IDC_EDIT2125, &CTab2DUI::OnEnChangeEdit2125_ChildUIRectScaleOffsetY)
END_MESSAGE_MAP()


// CTab2DUI 메시지 처리기


BOOL CTab2DUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	// RootUI
	m_ListBoxDataFileName.EnableWindow(FALSE);
	m_RadioRootCreateMode.EnableWindow(FALSE);
	m_RadioRootModifyMode.EnableWindow(FALSE);
	m_ListBoxRootUI.EnableWindow(FALSE);
	m_EditRootUITag.EnableWindow(FALSE);
	m_EditDataFileName.EnableWindow(FALSE);
	m_EditObjectTag.EnableWindow(FALSE);
	m_EditRootPosX.EnableWindow(FALSE);
	m_EditRootPosY.EnableWindow(FALSE);
	m_EditRootScaleX.EnableWindow(FALSE);
	m_EditRootScaleY.EnableWindow(FALSE);
	m_EditRootUIDepth.EnableWindow(FALSE);
	m_CheckRootIsAnimation.EnableWindow(FALSE);
	m_CheckRootIsAnimation.SetCheck(FALSE);
	m_bIsRootAnimation = false;
	m_EditRootFrameSpeed.EnableWindow(FALSE);
	m_EditRootRectPosOffsetX.EnableWindow(FALSE);
	m_EditRootRectPosOffsetY.EnableWindow(FALSE);
	m_EditRootRectScaleX.EnableWindow(FALSE);
	m_EditRootRectScaleY.EnableWindow(FALSE);

	// ChildUI
	m_RadioChildUICreateMode.EnableWindow(FALSE);
	m_RadioChildUIModifyMode.EnableWindow(FALSE);
	m_ListBoxChildUI.EnableWindow(FALSE);
	m_ButtonCreateChild.EnableWindow(FALSE);
	m_ButtonDeleteChild.EnableWindow(FALSE);
	m_EditChildRootObjectTag.EnableWindow(FALSE);
	m_EditChildObjectTag.EnableWindow(FALSE);
	m_EditChildPosOffsetX.EnableWindow(FALSE);
	m_EditChildPosOffsetY.EnableWindow(FALSE);
	m_EditChildScaleOffsetX.EnableWindow(FALSE);
	m_EditChildScaleOffsetY.EnableWindow(FALSE);
	m_EditChildUIDepth.EnableWindow(FALSE);
	m_CheckChildIsAnimation.EnableWindow(FALSE);
	m_EditChildFrameSpeed.EnableWindow(FALSE);
	m_EditChildRectPosOffsetX.EnableWindow(FALSE);
	m_EditChildRectPosOffsetY.EnableWindow(FALSE);
	m_EditChildRectScaleOffsetX.EnableWindow(FALSE);
	m_EditChildRectScaleOffsetY.EnableWindow(FALSE);
	m_ButtonChildSAVE.EnableWindow(FALSE);
	m_ButtonChildLOAD.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTab2DUI::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTab2DUI::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);

	RECT rcEditRoot[10] = { };
	m_EditRootPosX.GetWindowRect(&rcEditRoot[0]);
	m_EditRootPosY.GetWindowRect(&rcEditRoot[1]);
	m_EditRootScaleX.GetWindowRect(&rcEditRoot[2]);
	m_EditRootScaleY.GetWindowRect(&rcEditRoot[3]);
	m_EditRootUIDepth.GetWindowRect(&rcEditRoot[4]);
	m_EditRootFrameSpeed.GetWindowRect(&rcEditRoot[5]);
	m_EditRootRectPosOffsetX.GetWindowRect(&rcEditRoot[6]);
	m_EditRootRectPosOffsetY.GetWindowRect(&rcEditRoot[7]);
	m_EditRootRectScaleX.GetWindowRect(&rcEditRoot[8]);
	m_EditRootRectScaleY.GetWindowRect(&rcEditRoot[9]);



	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;

	if (PtInRect(&rcEditRoot[0], pt))		// RootPos X
	{
		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		{
			if (zDelta > 0)
				++m_fRootPosX;
			else
				--m_fRootPosX;

			pPickingRootUI->Get_Transform()->m_vPos.x = m_fRootPosX;
		}
	}
	else if (PtInRect(&rcEditRoot[1], pt))	// RootPos Y
	{
		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		{
			if (zDelta > 0)
				++m_fRootPosY;
			else
				--m_fRootPosY;

			pPickingRootUI->Get_Transform()->m_vPos.y = m_fRootPosY;
		}
	}

	else if (PtInRect(&rcEditRoot[2], pt))	// RootScale Y
	{
		if (zDelta > 0)
			++m_fRootScaleX;
		else
			--m_fRootScaleX;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->Get_Transform()->m_vScale.x = m_fRootScaleX;
	}
	else if (PtInRect(&rcEditRoot[3], pt))	// RootScale Y
	{
		if (zDelta > 0)
			++m_fRootScaleY;
		else
			--m_fRootScaleY;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->Get_Transform()->m_vScale.y = m_fRootScaleY;
	}


	else if (PtInRect(&rcEditRoot[4], pt))	// Root UI Depth
	{
		if (zDelta > 0)
			++m_RootUIDepth;
		else
			--m_RootUIDepth;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->Set_UIDepth(m_RootUIDepth);
	}

	else if (PtInRect(&rcEditRoot[5], pt))	// Root UI FraneSpeed
	{
		if (m_CheckRootIsAnimation.GetCheck())
		{
			if (zDelta > 0)
				++m_fRootFrameSpeed;
			else
				--m_fRootFrameSpeed;

			if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
				pPickingRootUI->m_tFrame.fFrameSpeed = m_fRootFrameSpeed;
		}
	}

	else if (PtInRect(&rcEditRoot[6], pt))	// Root RectPos Offset X
	{
		if (zDelta > 0)
			++m_fRootRectPosOffsetX;
		else
			--m_fRootRectPosOffsetX;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->m_vRectOffset.x = m_fRootRectPosOffsetX;
	}
	else if (PtInRect(&rcEditRoot[7], pt))	// Root RectPos Offset Y
	{
		if (zDelta > 0)
			++m_fRootRectPosOffsetY;
		else
			--m_fRootRectPosOffsetY;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->m_vRectOffset.y = m_fRootRectPosOffsetY;
	}

	else if (PtInRect(&rcEditRoot[8], pt))	// Root RectScale Offset Y
	{
		if (zDelta > 0)
			++m_fRootRectScaleX;
		else
			--m_fRootRectScaleX;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->m_pTransColor->m_vScale.x = m_fRootRectScaleX;
	}
	else if (PtInRect(&rcEditRoot[9], pt))	// Root RectScale Offset Y
	{
		if (zDelta > 0)
			++m_fRootRectScaleY;
		else
			--m_fRootRectScaleY;

		if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
			pPickingRootUI->m_pTransColor->m_vScale.y = m_fRootRectScaleY;
	}


	UpdateData(FALSE);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CTab2DUI::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnDropFiles(hDropInfo);
	UpdateData(TRUE);

	m_ListBoxDataFileName.ResetContent();	// 현재 리스트박스 초기화.
	TCHAR szFilePath[MAX_STR] = L"";

	int iFileCnt = DragQueryFile(hDropInfo, -1, nullptr, 0);

	TCHAR szFileName[MAX_STR] = L"";

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_STR);

		// 상대경로로 변환.
		CString strRelativePath = CToolFileInfo::ConvertRelativePath(szFilePath);

		// 경로에서 파일명만 남기기.
		// PathFindFileName: 인자로 받은 경로에서 파일명만 추출해주는 함수.
		CString strFileName = PathFindFileName(strRelativePath.GetString());

		// 파일명에서 확장자 제거.
		// PathRemoveExtension: 현재 파일명에서 확장자를 제거해주는 함수.
		lstrcpy(szFileName, strFileName.GetString());	
		m_ListBoxDataFileName.AddString(szFileName);
	}

	// 리스트 박스의 가로 스크롤을 생성.
	// HorizontalScroll();

	UpdateData(FALSE);
}



HRESULT CTab2DUI::Ready_Tab2DUI()
{
	UpdateData(TRUE);

	// RootUI
	m_ListBoxDataFileName.EnableWindow(TRUE);

	m_RadioRootCreateMode.EnableWindow(TRUE);
	m_RadioRootModifyMode.EnableWindow(TRUE);
	m_RadioRootCreateMode.SetCheck(TRUE);
	m_bIsRootCreateMode = true;
	m_RadioRootModifyMode.SetCheck(FALSE);
	m_bIsRootModifyMode = false;

	m_ListBoxRootUI.EnableWindow(TRUE);
	m_EditDataFileName.EnableWindow(TRUE);
	m_EditObjectTag.EnableWindow(TRUE);
	m_EditRootPosX.EnableWindow(TRUE);
	m_EditRootPosY.EnableWindow(TRUE);
	m_EditRootScaleX.EnableWindow(TRUE);
	m_EditRootScaleY.EnableWindow(TRUE);
	m_EditRootUIDepth.EnableWindow(TRUE);
	m_CheckRootIsAnimation.EnableWindow(TRUE);
	m_EditRootFrameSpeed.EnableWindow(FALSE);
	m_EditRootRectPosOffsetX.EnableWindow(TRUE);
	m_EditRootRectPosOffsetY.EnableWindow(TRUE);
	m_EditRootRectScaleX.EnableWindow(TRUE);
	m_EditRootRectScaleY.EnableWindow(TRUE);

	// ChildUI
	m_RadioChildUICreateMode.EnableWindow(TRUE);
	m_RadioChildUIModifyMode.EnableWindow(TRUE);
	m_RadioChildUICreateMode.SetCheck(TRUE);
	m_bIsChildCreateMode = true;
	m_RadioChildUIModifyMode.SetCheck(FALSE);
	m_bIsChildModifyMode = false;

	m_ListBoxChildUI.EnableWindow(TRUE);
	m_ButtonCreateChild.EnableWindow(TRUE);
	m_ButtonDeleteChild.EnableWindow(TRUE);
	m_EditChildRootObjectTag.EnableWindow(FALSE);
	m_EditChildObjectTag.EnableWindow(TRUE);
	m_EditChildPosOffsetX.EnableWindow(TRUE);
	m_EditChildPosOffsetY.EnableWindow(TRUE);
	m_EditChildScaleOffsetX.EnableWindow(TRUE);
	m_EditChildScaleOffsetY.EnableWindow(TRUE);
	m_EditChildUIDepth.EnableWindow(TRUE);

	m_CheckChildIsAnimation.EnableWindow(TRUE);
	m_CheckChildIsAnimation.SetCheck(false);
	m_EditChildFrameSpeed.EnableWindow(FALSE);

	m_EditChildRectPosOffsetX.EnableWindow(TRUE);
	m_EditChildRectPosOffsetY.EnableWindow(TRUE);
	m_EditChildRectScaleOffsetX.EnableWindow(TRUE);
	m_EditChildRectScaleOffsetY.EnableWindow(TRUE);
	m_ButtonChildSAVE.EnableWindow(TRUE);
	m_ButtonChildLOAD.EnableWindow(TRUE);

	UpdateData(FALSE);

	return S_OK;
}

void CTab2DUI::OnLbnSelchangeList2102_DataFileName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSelectIdx = m_ListBoxDataFileName.GetCaretIndex();
	m_ListBoxDataFileName.GetText(iSelectIdx, m_wstrRootDataFileName);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedCheck2100_RootUIIsAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_bIsRootAnimation = m_CheckRootIsAnimation.GetCheck();

	if (m_bIsRootAnimation)
		m_EditRootFrameSpeed.EnableWindow(TRUE);
	else
	{
		m_EditRootFrameSpeed.EnableWindow(FALSE);
		m_fRootFrameSpeed = 0.0f;
	}

	if (m_bIsRootModifyMode)
	{
		CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
		if (nullptr != pPickingRootUI)
			pPickingRootUI->m_bIsSpriteAnimation = m_bIsRootAnimation;
	}

	UpdateData(FALSE);
}

void CTab2DUI::OnLbnSelchangeList2100_RootUITagSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSelectIdx = m_ListBoxRootUI.GetCaretIndex();
	m_ListBoxRootUI.GetText(iSelectIdx, m_wstrRootUITag);
	m_ListBoxRootUI.GetText(iSelectIdx, m_wstrChildRootObjectTag);

	for (_int i = 0; i < m_ListBoxRootUI.GetCount(); ++i)
	{
		CString wstrRootUITag = L"";
		m_ListBoxRootUI.GetText(i, wstrRootUITag);
		Engine::OBJLIST* pRootUIList = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", wstring(wstrRootUITag));

		for (auto& pRootUI : *pRootUIList)
			static_cast<CToolUIRoot*>(pRootUI)->m_bIsRenderRect = false;
	}

	CToolUIRoot* pToolUIRoot = static_cast<CToolUIRoot*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", wstring(m_wstrRootUITag)));
	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI = pToolUIRoot;
	if (nullptr != pToolUIRoot)
		pToolUIRoot->m_bIsRenderRect = true;


	if (m_bIsRootModifyMode)
	{
		if (nullptr == pToolUIRoot)
			return;

		pToolUIRoot->m_bIsRenderRect = true;

		m_wstrRootUITag		= pToolUIRoot->m_wstrObjectTag.c_str();
		m_wstrRootObjectTag = pToolUIRoot->m_wstrObjectTag.c_str();
		m_fRootPosX         = pToolUIRoot->Get_Transform()->m_vPos.x;
		m_fRootPosY         = pToolUIRoot->Get_Transform()->m_vPos.y;
		m_fRootScaleX       = pToolUIRoot->Get_Transform()->m_vScale.x;
		m_fRootScaleY       = pToolUIRoot->Get_Transform()->m_vScale.y;
		m_RootUIDepth		= pToolUIRoot->Get_UIDepth();
		m_fRootFrameSpeed   = pToolUIRoot->m_tFrame.fFrameSpeed;
		if (m_fRootFrameSpeed > 0.0f)
		{
			m_bIsRootAnimation = true;
			m_CheckRootIsAnimation.SetCheck(TRUE);
			m_EditRootFrameSpeed.EnableWindow(TRUE);
		}
		else
		{
			m_bIsRootAnimation = false;
			m_CheckRootIsAnimation.SetCheck(FALSE);
			m_EditRootFrameSpeed.EnableWindow(FALSE);
		}
		m_fRootRectPosOffsetX = pToolUIRoot->m_vRectOffset.x;
		m_fRootRectPosOffsetY = pToolUIRoot->m_vRectOffset.y;
		m_fRootRectScaleX     = pToolUIRoot->m_pTransColor->m_vScale.x;
		m_fRootRectScaleY     = pToolUIRoot->m_pTransColor->m_vScale.y;
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2100_RootCreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// RootUI
	m_RadioRootCreateMode.SetCheck(TRUE);
	m_bIsRootCreateMode = true;
	m_RadioRootModifyMode.SetCheck(FALSE);
	m_bIsRootModifyMode = false;

	m_EditRootUITag.EnableWindow(TRUE);
	m_EditDataFileName.EnableWindow(TRUE);
	m_EditObjectTag.EnableWindow(TRUE);

	// ChildUI
	m_RadioChildUICreateMode.SetCheck(TRUE);
	m_bIsChildCreateMode = true;
	m_RadioChildUIModifyMode.SetCheck(FALSE);
	m_bIsChildModifyMode = false;

	m_EditChildObjectTag.EnableWindow(TRUE);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2101_RootModifyMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// RootUI
	m_RadioRootCreateMode.SetCheck(FALSE);
	m_bIsRootCreateMode = false;
	m_RadioRootModifyMode.SetCheck(TRUE);
	m_bIsRootModifyMode = true;

	m_EditRootUITag.EnableWindow(FALSE);
	m_EditDataFileName.EnableWindow(FALSE);
	m_EditObjectTag.EnableWindow(FALSE);
	
	// ChildUI
	m_RadioChildUICreateMode.SetCheck(FALSE);
	m_bIsChildCreateMode = false;
	m_RadioChildUIModifyMode.SetCheck(TRUE);
	m_bIsChildModifyMode = true;

	m_EditChildObjectTag.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedButton2104_RootUISAVE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolUIRoot* pToolUIRoot = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (nullptr == pToolUIRoot)
	{
		UpdateData(FALSE);
		return;
	}


	CFileDialog Dlg(FALSE,
					L"2DUI",
					L"*.2DUI",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.2DUI) | *.2DUI ||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wofstream fout { Dlg.GetPathName().GetString() };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		fout << wstring(m_wstrRootObjectTag)				<< L" "	// Object Tag
			 << pToolUIRoot->m_wstrDataFilePath				<< L" "	// DataFilePath
			 << pToolUIRoot->Get_Transform()->m_vPos.x		<< L" "	// Pos X
			 << pToolUIRoot->Get_Transform()->m_vPos.y		<< L" "	// Pos Y
			 << pToolUIRoot->Get_Transform()->m_vScale.x	<< L" "	// Scale X
			 << pToolUIRoot->Get_Transform()->m_vScale.y	<< L" "	// Scale Y
			 << pToolUIRoot->Get_UIDepth()					<< L" "	// UI Depth
			 << pToolUIRoot->m_bIsSpriteAnimation			<< L" "	// Is SpriteAnimation
			 << pToolUIRoot->m_tFrame.fFrameSpeed			<< L" "	// Frame Speed
			 << pToolUIRoot->m_vRectOffset.x				<< L" "	// RectPosOffset X
			 << pToolUIRoot->m_vRectOffset.y				<< L" "	// RectPosOffset Y
			 << pToolUIRoot->m_pTransColor->m_vScale.x		<< L" "	// RectScale X
			 << pToolUIRoot->m_pTransColor->m_vScale.y		<< L" ";	// RectScale Y

		AfxMessageBox(L"Data Save Successed");
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedButton2105_RootUILOAD()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE,
					L"2DUI",
					L"*.2DUI",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.2DUI)|*.2DUI||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wifstream fin { Dlg.GetPathName().GetString() };
		if (fin.fail())
		{
			AfxMessageBox(L"Load is Failed");
			return;
		}		

		wstring wstrObjectTag    = L"";			// ObjectTag
		wstring wstrDataFilePath = L"";			// DataFilePath
		_vec3 vPos               = _vec3(0.0f);	// Pos
		_vec3 vScale             = _vec3(1.0f);	// Scale
		_long UIDepth            = 0;			// UIDepth
		_bool bIsSpriteAnimation = false;		// IsSpriteAnimation
		_float fFrameSpeed       = 0.0f;		// FrameSpeed
		_vec3 vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3 vRectScale         = _vec3(1.0f);	// RectScale


		while (true)
		{
			fin >> wstrObjectTag
				>> wstrDataFilePath
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y;

			if (fin.eof())
				break;

			Engine::OBJLIST* pObjList = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", wstrObjectTag);
			if (nullptr != pObjList)
				m_pObjectMgr->Clear_OBJLIST(L"Layer_UI", wstrObjectTag);

			// ToolUIRoot 생성.
			Engine::CGameObject* pRootUI = CToolUIRoot::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(),
															   Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
															   wstrObjectTag,
															   wstrDataFilePath,
															   vPos,
															   vScale,
															   bIsSpriteAnimation,
															   fFrameSpeed,
															   vRectPosOffset,
															   vRectScale,
															   UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrObjectTag, pRootUI);


			// Control 최신화.
			m_wstrRootUITag        = wstrObjectTag.c_str();
			m_wstrRootDataFileName = PathFindFileName(wstrDataFilePath.c_str());
			m_wstrRootObjectTag    = wstrObjectTag.c_str();
			m_fRootPosX            = vPos.x;
			m_fRootPosY            = vPos.y;
			m_fRootScaleX          = vScale.x;
			m_fRootScaleY          = vScale.y;
			m_RootUIDepth          = UIDepth;
			m_bIsRootAnimation     = bIsSpriteAnimation;
			m_fRootFrameSpeed	   = fFrameSpeed;
			m_fRootRectPosOffsetX  = vRectPosOffset.x;
			m_fRootRectPosOffsetY  = vRectPosOffset.y;
			m_fRootRectScaleX      = vRectScale.x;
			m_fRootRectScaleY      = vRectScale.y;

			// ListBox 추가.
			for (_int i = 0; i < m_ListBoxRootUI.GetCount(); ++i)
			{
				CString wstrRootUITag = L"";
				m_ListBoxRootUI.GetText(i, wstrRootUITag);

				if (wstrRootUITag == CString(wstrObjectTag.c_str()))
					m_ListBoxRootUI.DeleteString(i);
			}
			m_ListBoxRootUI.AddString(wstrObjectTag.c_str());
			
		}

		AfxMessageBox(L"Data Load Successed");
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedButton2101_DeleteRootUI()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolUIRoot* pPickingToolUIRoot = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (nullptr != pPickingToolUIRoot && m_bIsRootModifyMode)
	{
		pPickingToolUIRoot->Set_DeadGameObject();

		// ListBox에서 제거.
		for (_int i = 0; i < m_ListBoxRootUI.GetCount(); ++i)
		{
			CString wstrRootUITag = L"";
			m_ListBoxRootUI.GetText(i, wstrRootUITag);

			if (wstrRootUITag == CString(pPickingToolUIRoot->m_wstrObjectTag.c_str()))
				m_ListBoxRootUI.DeleteString(i);
		}

		m_wstrRootUITag        = L"";
		m_wstrRootDataFileName = L"";
		m_wstrRootObjectTag    = L"";
	}

	UpdateData(FALSE);
}

void CTab2DUI::OnEnChangeEdit2103_RootUIPosX()
{
	UpdateData(TRUE);


	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->Get_Transform()->m_vPos.x = m_fRootPosX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2104_RootUIPosY()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->Get_Transform()->m_vPos.y = m_fRootPosY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2106_RootUIScaleX()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->Get_Transform()->m_vScale.x = m_fRootScaleX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2107_RootUIScaleY()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->Get_Transform()->m_vScale.y = m_fRootScaleY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2105_RootUIUIDepth()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->Set_UIDepth(m_RootUIDepth);

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2121_RootUIFrameSpeed()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
	{
		if (pPickingRootUI->m_bIsSpriteAnimation)
			pPickingRootUI->m_tFrame.fFrameSpeed = m_fRootFrameSpeed;
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2111_RootUIRectPosOffsetX()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->m_vRectOffset.x = m_fRootRectPosOffsetX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2112_RootUIRectPosOffsetY()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->m_vRectOffset.y = m_fRootRectPosOffsetY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2113_RootUIRectScaleOffsetX()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->m_pTransColor->m_vScale.x = m_fRootRectScaleX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2114_RootUIRectScaleOffsetY()
{
	UpdateData(TRUE);

	CToolUIRoot* pPickingRootUI = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingRootUI;
	if (m_bIsRootModifyMode && nullptr != pPickingRootUI)
		pPickingRootUI->m_pTransColor->m_vScale.y = m_fRootRectScaleY;

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2103_ChildUICreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_RadioChildUICreateMode.SetCheck(TRUE);
	m_bIsChildCreateMode = true;
	m_RadioChildUIModifyMode.SetCheck(FALSE);
	m_bIsChildModifyMode = false;

	m_EditChildObjectTag.EnableWindow(TRUE);


	m_RadioRootCreateMode.SetCheck(TRUE);
	m_bIsRootCreateMode = true;
	m_RadioRootModifyMode.SetCheck(FALSE);
	m_bIsRootModifyMode = false;

	m_EditRootUITag.EnableWindow(TRUE);
	m_EditDataFileName.EnableWindow(TRUE);
	m_EditObjectTag.EnableWindow(TRUE);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2104_ChildUIModifyMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_RadioChildUICreateMode.SetCheck(FALSE);
	m_bIsChildCreateMode = false;
	m_RadioChildUIModifyMode.SetCheck(TRUE);
	m_bIsChildModifyMode = true;

	m_EditChildObjectTag.EnableWindow(FALSE);

	m_RadioRootCreateMode.SetCheck(FALSE);
	m_bIsRootCreateMode = false;
	m_RadioRootModifyMode.SetCheck(TRUE);
	m_bIsRootModifyMode = true;

	m_EditRootUITag.EnableWindow(FALSE);
	m_EditDataFileName.EnableWindow(FALSE);
	m_EditObjectTag.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedCheck2101_CheckChildIsSpriteAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_bIsChildAnimation = m_CheckChildIsAnimation.GetCheck();

	if (m_bIsChildAnimation)
		m_EditChildFrameSpeed.EnableWindow(TRUE);
	else
	{
		m_EditChildFrameSpeed.EnableWindow(FALSE);
		m_fChildFrameSpeed = 0.0f;
	}

	if (m_bIsChildModifyMode)
	{
		if (nullptr != m_pChildUISelected)
			m_pChildUISelected->m_bIsSpriteAnimation = m_bIsChildAnimation;
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedButton2103_ChildUICreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_bIsChildCreateMode)
	{
		if (m_wstrRootDataFileName == L"" || 
			m_wstrChildRootObjectTag == L"" || 
			m_wstrChildObjectTag == L"")
			return;

		// RootUI
		CToolUIRoot* pRootUI = static_cast<CToolUIRoot*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", wstring(m_wstrChildRootObjectTag)));
		if (nullptr == pRootUI)
			return;


		wstring wstrDataFullPath;
		_tchar szPath[MAX_STR] = L"";
		GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
		PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
		PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
		lstrcat(szPath, L"\\Bin\\ToolData\\");
		wstrDataFullPath = wstring(szPath) + wstring(m_wstrRootDataFileName);
		wstrDataFullPath = CToolFileInfo::ConvertRelativePath(wstrDataFullPath.c_str());


		// UI 생성.
		Engine::CGameObject* pGameObj = nullptr;
		pGameObj = CToolUIChild::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(),
									    Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
									    wstring(m_wstrChildRootObjectTag),
									    wstring(m_wstrChildObjectTag),
									    wstrDataFullPath,
									    _vec3(m_fChildPosOffsetX, m_fChildPosOffsetY, 0.0f),
									    _vec3(m_fChildScaleOffsetX, m_fChildScaleOffsetX, 1.0f),
									    m_bIsChildAnimation,
									    m_fChildFrameSpeed,
									    _vec3(m_fChildRectPosOffsetX, m_fChildRectPosOffsetY, 0.0f),
									    _vec3(m_fChildRectScaleOffsetX, m_fChildRectScaleOffsetX, 1.0f),
									    m_ChildUIDepth);
		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstring(m_wstrChildObjectTag), pGameObj);
		pRootUI->m_vecUIChild.emplace_back(pGameObj);

		// ListBox 추가.
		m_ListBoxChildUI.AddString(m_wstrChildObjectTag);
	}

	CToolUIChild* pToolUIChild = nullptr;

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedButton2102_ChildUIDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
	{
		m_pChildUISelected->Set_DeadGameObject();

		// RootUI의 vector에서 제거.
		CToolUIRoot* pRootUI = static_cast<CToolUIRoot*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", wstring(m_wstrChildRootObjectTag)));
		
		auto iter_begin = pRootUI->m_vecUIChild.begin();
		auto iter_end = pRootUI->m_vecUIChild.end();
		for (; iter_begin != iter_end; ++iter_begin)
		{
			if ((*iter_begin)->Get_IsDead())
			{
				pRootUI->m_vecUIChild.erase(iter_begin);
				break;
			}
		}

		// ListBox에서 제거.
		m_ListBoxChildUI.DeleteString(m_iChildUISelectIdx);
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnLbnSelchangeList2101_SelectChildUI()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bIsChildCreateMode)
		return;

	m_pChildUISelected = nullptr;
	
	if (L"" == m_wstrChildRootObjectTag)
		return;
	CToolUIRoot* pRootUI = static_cast<CToolUIRoot*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", wstring(m_wstrChildRootObjectTag)));
	if (nullptr == pRootUI)
		return;


	UpdateData(TRUE);

	// 선택한 Index의 ChildUI 가져오기.
	m_iChildUISelectIdx = m_ListBoxChildUI.GetCaretIndex();
	m_pChildUISelected = static_cast<CToolUIChild*>(pRootUI->m_vecUIChild[m_iChildUISelectIdx]);
	m_pChildUISelected->m_bIsRenderRect = true;

	// 선택한 ChildUI의 정보를 MFC Control에 입력.
	m_wstrChildObjectTag     = CString(m_pChildUISelected->m_wstrObjectTag.c_str());
	m_fChildPosOffsetX       = m_pChildUISelected->Get_Transform()->m_vPos.x;
	m_fChildPosOffsetY       = m_pChildUISelected->Get_Transform()->m_vPos.y;
	m_fChildScaleOffsetX     = m_pChildUISelected->Get_Transform()->m_vScale.x;
	m_fChildScaleOffsetY     = m_pChildUISelected->Get_Transform()->m_vScale.y;
	m_bIsChildAnimation      = m_pChildUISelected->m_bIsSpriteAnimation;
	m_CheckChildIsAnimation.SetCheck(m_bIsChildAnimation);

	m_fChildFrameSpeed       = m_pChildUISelected->m_tFrame.fFrameSpeed;
	m_ChildUIDepth           = m_pChildUISelected->Get_UIDepth();
	m_fChildRectPosOffsetX   = m_pChildUISelected->m_vRectOffset.x;
	m_fChildRectPosOffsetY   = m_pChildUISelected->m_vRectOffset.y;
	m_fChildRectScaleOffsetX = m_pChildUISelected->m_pTransColor->m_vScale.x;
	m_fChildRectScaleOffsetY = m_pChildUISelected->m_pTransColor->m_vScale.y;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2116_ChildUIPosOffsetX()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->Get_Transform()->m_vPos.x = m_fChildPosOffsetX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2117_ChildUIPosOffsetY()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->Get_Transform()->m_vPos.y = m_fChildPosOffsetY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2119_ChildUIScaleOffsetX()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->Get_Transform()->m_vScale.x = m_fChildScaleOffsetX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2120_ChildUIScaleOffsetY()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->Get_Transform()->m_vScale.y = m_fChildScaleOffsetY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2118_ChildUIUIDepth()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->Set_UIDepth(m_ChildUIDepth);

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2126_ChildUIFrameSpeed()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
	{
		if (m_pChildUISelected->m_bIsSpriteAnimation)
			m_pChildUISelected->m_tFrame.fFrameSpeed = m_fChildFrameSpeed;
	}

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2122_ChildUIRectPosOffsetX()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->m_vRectOffset.x = m_fChildRectPosOffsetX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2123_ChildUIRectPosOffsetY()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->m_vRectOffset.y = m_fChildRectPosOffsetY;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2124_ChildUIRectScaleOffsetX()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->m_pTransColor->m_vScale.x = m_fChildRectScaleOffsetX;

	UpdateData(FALSE);
}


void CTab2DUI::OnEnChangeEdit2125_ChildUIRectScaleOffsetY()
{
	UpdateData(TRUE);

	if (m_bIsChildModifyMode && nullptr != m_pChildUISelected)
		m_pChildUISelected->m_pTransColor->m_vScale.y = m_fChildRectScaleOffsetY;

	UpdateData(FALSE);
}
