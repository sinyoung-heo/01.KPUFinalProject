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
	DDX_Control(pDX, IDC_CHECK2102, m_CheckRootRenderRect);
	DDX_Control(pDX, IDC_EDIT2121, m_EditRootFrameSpeed);
	DDX_Text(pDX, IDC_EDIT2121, m_fRootFrameSpeed);
	DDX_Control(pDX, IDC_RADIO2100, m_RadioRootCreateMode);
	DDX_Control(pDX, IDC_RADIO2101, m_RadioRootModifyMode);
}


BEGIN_MESSAGE_MAP(CTab2DUI, CDialogEx)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_CHECK2100, &CTab2DUI::OnBnClickedCheck2100_RootUIIsAnimation)
	ON_BN_CLICKED(IDC_CHECK2102, &CTab2DUI::OnBnClickedCheck2102_RootUIRenderRect)
	ON_LBN_SELCHANGE(IDC_LIST2100, &CTab2DUI::OnLbnSelchangeList2100_RootUITagSelect)
	ON_BN_CLICKED(IDC_RADIO2100, &CTab2DUI::OnBnClickedRadio2100_RootCreateMode)
	ON_BN_CLICKED(IDC_RADIO2101, &CTab2DUI::OnBnClickedRadio2101_RootModifyMode)
END_MESSAGE_MAP()


// CTab2DUI 메시지 처리기


BOOL CTab2DUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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

	m_CheckRootRenderRect.EnableWindow(FALSE);
	m_EditRootRectPosOffsetX.EnableWindow(FALSE);
	m_EditRootRectPosOffsetY.EnableWindow(FALSE);
	m_EditRootRectScaleX.EnableWindow(FALSE);
	m_EditRootRectScaleY.EnableWindow(FALSE);

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

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

HRESULT CTab2DUI::Ready_Tab2DUI()
{
	UpdateData(TRUE);

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
	m_CheckRootRenderRect.EnableWindow(TRUE);
	m_EditRootRectPosOffsetX.EnableWindow(TRUE);
	m_EditRootRectPosOffsetY.EnableWindow(TRUE);
	m_EditRootRectScaleX.EnableWindow(TRUE);
	m_EditRootRectScaleY.EnableWindow(TRUE);

	UpdateData(FALSE);

	return S_OK;
}


void CTab2DUI::OnBnClickedCheck2100_RootUIIsAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_bIsRootAnimation = m_CheckRootIsAnimation.GetCheck();

	if (m_bIsRootAnimation)
		m_EditRootFrameSpeed.EnableWindow(TRUE);
	else
		m_EditRootFrameSpeed.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedCheck2102_RootUIRenderRect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_bIsRenderRect = m_CheckRootRenderRect.GetCheck();

	UpdateData(FALSE);
}

void CTab2DUI::OnLbnSelchangeList2100_RootUITagSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	_int iSelectIdx = m_ListBoxRootUI.GetCaretIndex();
	m_ListBoxRootUI.GetText(iSelectIdx, m_wstrRootUITag);

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2100_RootCreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_RadioRootCreateMode.SetCheck(TRUE);
	m_bIsRootCreateMode = true;

	m_RadioRootModifyMode.SetCheck(FALSE);
	m_bIsRootModifyMode = false;

	UpdateData(FALSE);
}


void CTab2DUI::OnBnClickedRadio2101_RootModifyMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_RadioRootCreateMode.SetCheck(FALSE);
	m_bIsRootCreateMode = false;

	m_RadioRootModifyMode.SetCheck(TRUE);
	m_bIsRootModifyMode = true;

	UpdateData(FALSE);
}
