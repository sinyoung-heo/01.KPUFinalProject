// TabObject.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabMap.h"
#include "afxdialogex.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "ToolSceneStage.h"
#include "ToolTerrain.h"
#include "ToolSkyBox.h"

// CTabMap 대화 상자

IMPLEMENT_DYNAMIC(CTabMap, CDialogEx)

CTabMap::CTabMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabObject, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_fSkyBox_PosX(0)
	, m_fSkyBox_PosY(0)
	, m_fSkyBox_PosZ(0)
	, m_fSkyBox_Scale(0)
{

}

CTabMap::~CTabMap()
{
}

void CTabMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1001, m_TerrainRadio128);
	DDX_Control(pDX, IDC_RADIO1002, m_TerrainRadio256);
	DDX_Control(pDX, IDC_RADIO1003, m_TerrainRadio512);
	DDX_Control(pDX, IDC_CHECK1001, m_TerrainCheckBox_RenderWireFrame);
	DDX_Control(pDX, IDC_LIST1001, m_TerrainListBox_TexIndex);
	DDX_Control(pDX, IDC_EDIT1001, m_SkyBoxEdit_PosX);
	DDX_Control(pDX, IDC_EDIT1002, m_SkyBoxEdit_PosY);
	DDX_Control(pDX, IDC_EDIT1003, m_SkyBoxEdit_PosZ);
	DDX_Control(pDX, IDC_EDIT1004, m_SkyBoxEdit_Scale);
	DDX_Control(pDX, IDC_LIST1002, m_SkyBoxListBox_TexIndex);
	DDX_Control(pDX, IDC_CHECK1004, m_SkyBoxCheckBox_RenderOnOff);
	DDX_Text(pDX, IDC_EDIT1001, m_fSkyBox_PosX);
	DDX_Text(pDX, IDC_EDIT1002, m_fSkyBox_PosY);
	DDX_Text(pDX, IDC_EDIT1003, m_fSkyBox_PosZ);
	DDX_Text(pDX, IDC_EDIT1004, m_fSkyBox_Scale);
}


BEGIN_MESSAGE_MAP(CTabMap, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1001, &CTabMap::OnBnClickedRadio1001_Terrain128)
	ON_BN_CLICKED(IDC_RADIO1002, &CTabMap::OnBnClickedRadio1002_Terrain256)
	ON_BN_CLICKED(IDC_RADIO1003, &CTabMap::OnBnClickedRadio1003_Terrain512)
	ON_BN_CLICKED(IDC_CHECK1001, &CTabMap::OnBnClickedCheck1001_TerrainRenderWireFrame)
	ON_LBN_SELCHANGE(IDC_LIST1001, &CTabMap::OnLbnSelchangeList1001_TerrainTexIndex)
	ON_LBN_SELCHANGE(IDC_LIST1002, &CTabMap::OnLbnSelchangeList1002_SkyBoxTexIndex)
	ON_BN_CLICKED(IDC_CHECK1004, &CTabMap::OnBnClickedCheck1004_SkyBoxRenderOnOff)
	ON_EN_CHANGE(IDC_EDIT1001, &CTabMap::OnEnChangeEdit1001_SkyBoxPosX)
	ON_EN_CHANGE(IDC_EDIT1002, &CTabMap::OnEnChangeEdit1002_SkyBoxPosY)
	ON_EN_CHANGE(IDC_EDIT1003, &CTabMap::OnEnChangeEdit1003_SkyBoxPosZ)
	ON_EN_CHANGE(IDC_EDIT1004, &CTabMap::OnEnChangeEdit1004__SkyBoxScale)
END_MESSAGE_MAP()


// CTabMap 메시지 처리기
BOOL CTabMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 컨트롤 비활성화
	m_TerrainRadio128.EnableWindow(FALSE);
	m_TerrainRadio256.EnableWindow(FALSE);
	m_TerrainRadio512.EnableWindow(FALSE);
	m_TerrainCheckBox_RenderWireFrame.EnableWindow(FALSE);
	m_TerrainListBox_TexIndex.EnableWindow(FALSE);

	m_SkyBoxEdit_PosX.EnableWindow(FALSE);
	m_SkyBoxEdit_PosY.EnableWindow(FALSE);
	m_SkyBoxEdit_PosZ.EnableWindow(FALSE);
	m_SkyBoxEdit_Scale.EnableWindow(FALSE);
	m_SkyBoxListBox_TexIndex.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.SetCheck(true);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabMap::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

HRESULT CTabMap::Ready_TerrainControl()
{
	/*__________________________________________________________________________________________________________
	[ TERRAIN ]
	____________________________________________________________________________________________________________*/
	UpdateData(TRUE);

	// 컨트롤 활성화
	m_TerrainRadio128.EnableWindow(TRUE);
	m_TerrainRadio256.EnableWindow(TRUE);
	m_TerrainRadio512.EnableWindow(TRUE);
	m_TerrainCheckBox_RenderWireFrame.EnableWindow(TRUE);
	m_TerrainListBox_TexIndex.EnableWindow(TRUE);

	// Terrain 128X128 활성화.
	m_TerrainRadio128.SetCheck(true);
	static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"))->m_bIsUpdate = true;
	m_TerrainRadio256.SetCheck(false);
	m_TerrainRadio512.SetCheck(false);


	// Render WireFrame 활성화.
	m_TerrainCheckBox_RenderWireFrame.SetCheck(true);

	// ListBox 초기화.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(L"Terrain", Engine::COMPONENTID::ID_STATIC));
	for (_int i = 0; i < pTextureCom->Get_TexSize(); ++i)
	{
		_tchar m_szText[MAX_STR] = L"";
		wsprintf(m_szText, L"Index : %d", i);

		m_TerrainListBox_TexIndex.AddString(m_szText);
	}

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_SkyBoxControl()
{
	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	UpdateData(TRUE);

	// 컨트롤 활성화.
	m_SkyBoxEdit_PosX.EnableWindow(TRUE);
	m_SkyBoxEdit_PosY.EnableWindow(TRUE);
	m_SkyBoxEdit_PosZ.EnableWindow(TRUE);
	m_SkyBoxEdit_Scale.EnableWindow(TRUE);
	m_SkyBoxListBox_TexIndex.EnableWindow(TRUE);
	m_SkyBoxCheckBox_RenderOnOff.EnableWindow(TRUE);

	// ListBox 초기화.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(L"SkyBox", Engine::COMPONENTID::ID_STATIC));
	for (_int i = 0; i < pTextureCom->Get_TexSize(); ++i)
	{
		_tchar m_szText[MAX_STR] = L"";
		wsprintf(m_szText, L"Index : %d", i);

		m_SkyBoxListBox_TexIndex.AddString(m_szText);
	}

	// EditControl 초기화.
	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	m_fSkyBox_PosX	= pSkyBox->Get_Transform()->m_vPos.x;
	m_fSkyBox_PosY	= pSkyBox->Get_Transform()->m_vPos.y;
	m_fSkyBox_PosZ	= pSkyBox->Get_Transform()->m_vPos.z;
	m_fSkyBox_Scale = pSkyBox->Get_Transform()->m_vScale.x;

	UpdateData(FALSE);

	return S_OK;
}



void CTabMap::OnBnClickedRadio1001_Terrain128()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));
	
	m_pTerrain128->m_bIsUpdate = true;
	m_pTerrain256->m_bIsUpdate = false;
	m_pTerrain512->m_bIsUpdate = false;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain128;


	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1002_Terrain256()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_bIsUpdate = false;
	m_pTerrain256->m_bIsUpdate = true;
	m_pTerrain512->m_bIsUpdate = false;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain256;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1003_Terrain512()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_bIsUpdate = false;
	m_pTerrain256->m_bIsUpdate = false;
	m_pTerrain512->m_bIsUpdate = true;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain512;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1001_TerrainRenderWireFrame()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	if (m_TerrainCheckBox_RenderWireFrame.GetCheck())
	{
		// WireFrame
		m_pTerrain128->m_pShaderCom->Set_PipelineStatePass(1);
		m_pTerrain256->m_pShaderCom->Set_PipelineStatePass(1);
		m_pTerrain512->m_pShaderCom->Set_PipelineStatePass(1);
	}
	else
	{
		// Solid
		m_pTerrain128->m_pShaderCom->Set_PipelineStatePass(0);
		m_pTerrain256->m_pShaderCom->Set_PipelineStatePass(0);
		m_pTerrain512->m_pShaderCom->Set_PipelineStatePass(0);
	}

	UpdateData(FALSE);
}


void CTabMap::OnLbnSelchangeList1001_TerrainTexIndex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);


	// 선택한 ListBox의 Index를 얻어온다.
	_int iNewTexIdx = m_TerrainListBox_TexIndex.GetCaretIndex();

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_uiTexIdx = iNewTexIdx;
	m_pTerrain256->m_uiTexIdx = iNewTexIdx;
	m_pTerrain512->m_uiTexIdx = iNewTexIdx;

	UpdateData(FALSE);

}


void CTabMap::OnLbnSelchangeList1002_SkyBoxTexIndex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// 선택한 ListBox의 Index를 얻어온다.
	_int iNewTexIdx = m_SkyBoxListBox_TexIndex.GetCaretIndex();

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->m_uiTexIdx = iNewTexIdx;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1004_SkyBoxRenderOnOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	if (m_SkyBoxCheckBox_RenderOnOff.GetCheck())
		pSkyBox->m_bIsUpdate = true;
	else
		pSkyBox->m_bIsUpdate = false;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1001_SkyBoxPosX()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.x = m_fSkyBox_PosX;

	UpdateData(TRUE);
}


void CTabMap::OnEnChangeEdit1002_SkyBoxPosY()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.y = m_fSkyBox_PosY;

	UpdateData(FALSE);

}


void CTabMap::OnEnChangeEdit1003_SkyBoxPosZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.z = m_fSkyBox_PosZ;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1004__SkyBoxScale()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vScale.x = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.y = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.z = m_fSkyBox_Scale;

	UpdateData(FALSE);
}
