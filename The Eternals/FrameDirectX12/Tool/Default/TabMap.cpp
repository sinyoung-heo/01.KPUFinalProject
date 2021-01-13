// TabObject.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabMap.h"
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
	DDX_Control(pDX, IDC_CHECK1005, m_EditCheck_StaticMesh);
	DDX_Control(pDX, IDC_CHECK1006, m_EditCheck_LightingInfo);
	DDX_Control(pDX, IDC_CHECK1007, m_EditCheck_NavigationMesh);
	DDX_Control(pDX, IDC_RADIO1005, m_StaticMeshRadio_CreateMode);
	DDX_Control(pDX, IDC_RADIO1006, m_StaticMeshRadio_ModifyMode);
	DDX_Control(pDX, IDC_TREE1001, m_StaticMeshTree_ResourceTree);
	DDX_Control(pDX, IDC_LIST1003, m_StaticMeshListBox_ObjectList);
	DDX_Control(pDX, IDC_BUTTON1001, m_StaticMeshButton_DeleteObject);
	DDX_Control(pDX, IDC_BUTTON1002, m_StaticMeshButton_AllDeleteObject);
	DDX_Control(pDX, IDC_EDIT1005, m_StaticMeshEdit_ScaleX);
	DDX_Control(pDX, IDC_EDIT1006, m_StaticMeshEdit_ScaleY);
	DDX_Control(pDX, IDC_EDIT1007, m_StaticMeshEdit_ScaleZ);
	DDX_Control(pDX, IDC_EDIT1008, m_StaticMeshEdit_AngleX);
	DDX_Control(pDX, IDC_EDIT1009, m_StaticMeshEdit_AngleY);
	DDX_Control(pDX, IDC_EDIT1010, m_StaticMeshEdit_AngleZ);
	DDX_Control(pDX, IDC_EDIT1011, m_StaticMeshEdit_PosX);
	DDX_Control(pDX, IDC_EDIT1012, m_StaticMeshEdit_PosY);
	DDX_Control(pDX, IDC_EDIT1013, m_StaticMeshEdit_PosZ);
	DDX_Control(pDX, IDC_CHECK1002, m_StaticMeshCheck_IsRenderShadow);
	DDX_Control(pDX, IDC_CHECK1003, m_StaticMeshCheck_IsCollision);
	DDX_Control(pDX, IDC_BUTTON1003, m_StaticMeshButton_Save);
	DDX_Control(pDX, IDC_BUTTON1004, m_StaticMeshButton_Load);
	DDX_Text(pDX, IDC_EDIT1005, m_fStaticMeshScaleX);
	DDX_Text(pDX, IDC_EDIT1006, m_fStaticMeshScaleY);
	DDX_Text(pDX, IDC_EDIT1007, m_fStaticMeshScaleZ);
	DDX_Text(pDX, IDC_EDIT1008, m_fStaticMeshAngleX);
	DDX_Text(pDX, IDC_EDIT1009, m_fStaticMeshAngleY);
	DDX_Text(pDX, IDC_EDIT1010, m_fStaticMeshAngleZ);
	DDX_Text(pDX, IDC_EDIT1011, m_fStaticMeshPosX);
	DDX_Text(pDX, IDC_EDIT1012, m_fStaticMeshPosY);
	DDX_Text(pDX, IDC_EDIT1013, m_fStaticMeshPosZ);
	DDX_Control(pDX, IDC_EDIT1057, m_StaticMeshEdit_ObjectSize);
	DDX_Text(pDX, IDC_EDIT1057, m_iStaticMeshObjectSize);
	DDX_Control(pDX, IDC_EDIT1058, m_StaticMeshEdit_SelectMesthTag);
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
	ON_BN_CLICKED(IDC_CHECK1005, &CTabMap::OnBnClickedCheck1005_EditStaticMesh)
	ON_BN_CLICKED(IDC_CHECK1006, &CTabMap::OnBnClickedCheck1006_EditLightingInfo)
	ON_BN_CLICKED(IDC_CHECK1007, &CTabMap::OnBnClickedCheck1007_EditNavigationMesh)
	ON_NOTIFY(NM_CLICK, IDC_TREE1001, &CTabMap::OnNMClickTree1001_TreeMeshTag)
	ON_BN_CLICKED(IDC_RADIO1005, &CTabMap::OnBnClickedRadio1005_StaticMeshCreateMode)
	ON_BN_CLICKED(IDC_RADIO1006, &CTabMap::OnBnClickedRadio1006_StaticMeshModifyeMode)
	ON_EN_CHANGE(IDC_EDIT1005, &CTabMap::OnEnChangeEdit1005_StaticMeshScaleX)
	ON_EN_CHANGE(IDC_EDIT1006, &CTabMap::OnEnChangeEdit1006_StaticMeshScaleY)
	ON_EN_CHANGE(IDC_EDIT1007, &CTabMap::OnEnChangeEdit1007_StaticMeshScaleZ)
	ON_EN_CHANGE(IDC_EDIT1008, &CTabMap::OnEnChangeEdit1008_StaticMeshAngleX)
	ON_EN_CHANGE(IDC_EDIT1009, &CTabMap::OnEnChangeEdit1009_StaticMeshAngleY)
	ON_EN_CHANGE(IDC_EDIT1010, &CTabMap::OnEnChangeEdit1010_StaticMeshAngleZ)
	ON_EN_CHANGE(IDC_EDIT1011, &CTabMap::OnEnChangeEdit1011_StaticMeshPosX)
	ON_EN_CHANGE(IDC_EDIT1012, &CTabMap::OnEnChangeEdit1012_StaticMeshPosY)
	ON_EN_CHANGE(IDC_EDIT1013, &CTabMap::OnEnChangeEdit1013_StaticMeshPosZ)
	ON_BN_CLICKED(IDC_CHECK1002, &CTabMap::OnBnClickedCheck1002_StaticMeshRenderShadow)
	ON_BN_CLICKED(IDC_CHECK1003, &CTabMap::OnBnClickedCheck1003_StaticMeshIsCollision)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_LBN_SELCHANGE(IDC_LIST1003, &CTabMap::OnLbnSelchangeList1003_StaticMeshObjectList)
	ON_BN_CLICKED(IDC_BUTTON1001, &CTabMap::OnBnClickedButton1001_StasticMeshDelete)
	ON_BN_CLICKED(IDC_BUTTON1002, &CTabMap::OnBnClickedButton1002_StaticMeshAllDelete)
END_MESSAGE_MAP()


// CTabMap 메시지 처리기
BOOL CTabMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// Terrain 컨트롤 비활성화
	m_TerrainRadio128.EnableWindow(FALSE);
	m_TerrainRadio256.EnableWindow(FALSE);
	m_TerrainRadio512.EnableWindow(FALSE);
	m_TerrainCheckBox_RenderWireFrame.EnableWindow(FALSE);
	m_TerrainListBox_TexIndex.EnableWindow(FALSE);

	// SkyBox 컨트롤 비활성화
	m_SkyBoxEdit_PosX.EnableWindow(FALSE);
	m_SkyBoxEdit_PosY.EnableWindow(FALSE);
	m_SkyBoxEdit_PosZ.EnableWindow(FALSE);
	m_SkyBoxEdit_Scale.EnableWindow(FALSE);
	m_SkyBoxListBox_TexIndex.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.SetCheck(true);

	// EditCheck 컨트롤 비활성화.
	m_EditCheck_StaticMesh.EnableWindow(FALSE);
	m_EditCheck_LightingInfo.EnableWindow(FALSE);
	m_EditCheck_NavigationMesh.EnableWindow(FALSE);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosX.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(FALSE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

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

BOOL CTabMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);

	/*__________________________________________________________________________________________________________
	[ StaticMesh Control ]
	____________________________________________________________________________________________________________*/
	if (m_EditCheck_StaticMesh.GetCheck())
	{
		RECT rcStaticMeshEdit[9] = { };
		m_StaticMeshEdit_ScaleX.GetWindowRect(&rcStaticMeshEdit[0]);
		m_StaticMeshEdit_ScaleY.GetWindowRect(&rcStaticMeshEdit[1]);
		m_StaticMeshEdit_ScaleZ.GetWindowRect(&rcStaticMeshEdit[2]);
		m_StaticMeshEdit_AngleX.GetWindowRect(&rcStaticMeshEdit[3]);
		m_StaticMeshEdit_AngleY.GetWindowRect(&rcStaticMeshEdit[4]);
		m_StaticMeshEdit_AngleZ.GetWindowRect(&rcStaticMeshEdit[5]);
		m_StaticMeshEdit_PosX.GetWindowRect(&rcStaticMeshEdit[6]);
		m_StaticMeshEdit_PosY.GetWindowRect(&rcStaticMeshEdit[7]);
		m_StaticMeshEdit_PosZ.GetWindowRect(&rcStaticMeshEdit[8]);

		if (PtInRect(&rcStaticMeshEdit[0], pt) ||	// Scale X
			PtInRect(&rcStaticMeshEdit[1], pt) ||	// Scale Y
			PtInRect(&rcStaticMeshEdit[2], pt))		// Scale X
		{
			if (zDelta > 0)
			{
				m_fStaticMeshScaleX += 0.01f;
				m_fStaticMeshScaleY += 0.01f;
				m_fStaticMeshScaleZ += 0.01f;
			}
			else if (zDelta < 0 && m_fStaticMeshScaleX > 0.f)
			{
				m_fStaticMeshScaleX -= 0.01f;
				m_fStaticMeshScaleY -= 0.01f;
				m_fStaticMeshScaleZ -= 0.01f;
			}
		}
		else if (PtInRect(&rcStaticMeshEdit[3], pt))	// Angle X
		{
			if (zDelta > 0)
				m_fStaticMeshAngleX += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshAngleX > 0.f)
				m_fStaticMeshAngleX -= 1.0f;
		}
		else if (PtInRect(&rcStaticMeshEdit[4], pt))	// Angle Y
		{
			if (zDelta > 0)
				m_fStaticMeshAngleY += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshAngleY > 0.f)
				m_fStaticMeshAngleY -= 1.0f;
		}
		else if (PtInRect(&rcStaticMeshEdit[5], pt))	// Angle Z
		{
			if (zDelta > 0)
				m_fStaticMeshAngleZ += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshAngleZ > 0.f)
				m_fStaticMeshAngleZ -= 1.0f;
		}
		else if (PtInRect(&rcStaticMeshEdit[6], pt))	// Pos X
		{
			if (zDelta > 0)
				m_fStaticMeshPosX += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshPosX > 0.f)
				m_fStaticMeshPosX -= 1.0f;
		}
		else if (PtInRect(&rcStaticMeshEdit[7], pt))	// Pos Y
		{
			if (zDelta > 0)
				m_fStaticMeshPosY += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshPosY > 0.f)
				m_fStaticMeshPosY -= 1.0f;
		}
		else if (PtInRect(&rcStaticMeshEdit[8], pt))	// Pos Z
		{
			if (zDelta > 0)
				m_fStaticMeshPosZ += 1.0f;
			else if (zDelta < 0 && m_fStaticMeshPosZ > 0.f)
				m_fStaticMeshPosZ -= 1.0f;
		}
	}



	UpdateData(FALSE);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
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
	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	m_pTerrain128->m_bIsUpdate = true;

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

HRESULT CTabMap::Ready_EditControl()
{
	UpdateData(TRUE);

	// EditCheck 컨트롤 활성화.
	m_EditCheck_StaticMesh.EnableWindow(TRUE);
	m_EditCheck_LightingInfo.EnableWindow(TRUE);
	m_EditCheck_NavigationMesh.EnableWindow(TRUE);

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_StaticMeshControl()
{
	UpdateData(TRUE);

	// StaticMesh
	m_EditCheck_StaticMesh.SetCheck(true);
	m_StaticMeshRadio_CreateMode.EnableWindow(TRUE);
	m_StaticMeshRadio_CreateMode.SetCheck(true);
	m_StaticMeshRadio_ModifyMode.EnableWindow(TRUE);
	m_StaticMeshRadio_ModifyMode.SetCheck(false);
	m_bIsCreateMode = true;
	m_bIsModifyMode = false;


	m_StaticMeshTree_ResourceTree.EnableWindow(TRUE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(TRUE);
	m_StaticMeshListBox_ObjectList.EnableWindow(TRUE);
	m_StaticMeshButton_DeleteObject.EnableWindow(TRUE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleX.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleY.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleX.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleY.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_PosX.EnableWindow(TRUE);
	m_StaticMeshEdit_PosY.EnableWindow(TRUE);
	m_StaticMeshEdit_PosZ.EnableWindow(TRUE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(TRUE);
	m_StaticMeshCheck_IsCollision.EnableWindow(TRUE);
	m_StaticMeshButton_Save.EnableWindow(TRUE);
	m_StaticMeshButton_Load.EnableWindow(TRUE);

	// Ready Mesh TreeControl.

	HTREEITEM h_DynamciMesh, h_DynamicMeshRoot;
	HTREEITEM h_StaticMesh, h_StaticMeshRoot;

	h_DynamciMesh	= m_StaticMeshTree_ResourceTree.InsertItem(L"DynamicMesh", NULL, NULL);
	h_StaticMesh	= m_StaticMeshTree_ResourceTree.InsertItem(L"StaticMesh", NULL, NULL);

	wifstream fin { L"../../Bin/ToolData/MeshTreeCtrlInfo.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar	szCurMeshType[MIN_STR]	= L"";
	_tchar	szCurMeshRoot[MIN_STR]	= L"";
	_tchar	szMeshTag[MAX_STR]		= L"";

	wstring wstrPreMeshType			= L"";
	wstring wstrPreMeshRoot			= L"";

	while (true)
	{
		fin.getline(szCurMeshType, MIN_STR, '|');
		fin.getline(szCurMeshRoot, MIN_STR, '|');
		fin.getline(szMeshTag, MIN_STR);

		if (fin.eof())
			break;

		wstring wstrCurMeshType = szCurMeshType;
		wstring wstrCurMeshRoot = szCurMeshRoot;

		// 이전의 MeshRootTag값이 현재의 MeshRootTag값과 다르면 TreeCtrl에 삽입.
		if (L"DynamicMesh" == wstrCurMeshType)
		{
			if (wstrPreMeshRoot != wstrCurMeshRoot)
			{
				wstrPreMeshRoot = wstrCurMeshRoot;
				h_DynamicMeshRoot = m_StaticMeshTree_ResourceTree.InsertItem(wstrPreMeshRoot.c_str(), h_DynamciMesh, NULL);
			}

			m_StaticMeshTree_ResourceTree.InsertItem(szMeshTag, h_DynamicMeshRoot, NULL);
		}

		if (L"StaticMesh" == wstrCurMeshType)
		{
			if (wstrPreMeshRoot != wstrCurMeshRoot)
			{
				wstrPreMeshRoot = wstrCurMeshRoot;
				h_StaticMeshRoot = m_StaticMeshTree_ResourceTree.InsertItem(wstrPreMeshRoot.c_str(), h_StaticMesh, NULL);
			}

			m_StaticMeshTree_ResourceTree.InsertItem(szMeshTag, h_StaticMeshRoot, NULL);
		}


	}

	fin.close();


	// 모든 트리의 노드를 펼친다.
	m_StaticMeshTree_ResourceTree.Expand(h_DynamciMesh, TVE_EXPAND);

	// DynamicMesh
	HTREEITEM h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_DynamciMesh, TVGN_CHILD);
	m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_Child, TVGN_NEXT);
		m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);
	}

	// StaticMesh
	m_StaticMeshTree_ResourceTree.Expand(h_StaticMesh, TVE_EXPAND);

	h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_StaticMesh, TVGN_CHILD);
	m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_Child, TVGN_NEXT);
		m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);
	}

	UpdateData(FALSE);

	return S_OK;
}



void CTabMap::OnBnClickedRadio1001_Terrain128()
{
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
	UpdateData(TRUE);

	// 선택한 ListBox의 Index를 얻어온다.
	_int iNewTexIdx = m_SkyBoxListBox_TexIndex.GetCaretIndex();

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->m_uiTexIdx = iNewTexIdx;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1004_SkyBoxRenderOnOff()
{
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
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.x = m_fSkyBox_PosX;

	UpdateData(TRUE);
}


void CTabMap::OnEnChangeEdit1002_SkyBoxPosY()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.y = m_fSkyBox_PosY;

	UpdateData(FALSE);

}


void CTabMap::OnEnChangeEdit1003_SkyBoxPosZ()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.z = m_fSkyBox_PosZ;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1004__SkyBoxScale()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vScale.x = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.y = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.z = m_fSkyBox_Scale;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1005_EditStaticMesh()
{
	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(true);
	m_EditCheck_LightingInfo.SetCheck(false);
	m_EditCheck_NavigationMesh.SetCheck(false);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(TRUE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(TRUE);
	m_StaticMeshTree_ResourceTree.EnableWindow(TRUE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(TRUE);
	m_StaticMeshListBox_ObjectList.EnableWindow(TRUE);
	m_StaticMeshButton_DeleteObject.EnableWindow(TRUE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleX.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleY.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleX.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleY.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_PosX.EnableWindow(TRUE);
	m_StaticMeshEdit_PosY.EnableWindow(TRUE);
	m_StaticMeshEdit_PosZ.EnableWindow(TRUE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(TRUE);
	m_StaticMeshCheck_IsCollision.EnableWindow(TRUE);
	m_StaticMeshButton_Save.EnableWindow(TRUE);
	m_StaticMeshButton_Load.EnableWindow(TRUE);

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1006_EditLightingInfo()
{

	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(false);
	m_EditCheck_LightingInfo.SetCheck(true);
	m_EditCheck_NavigationMesh.SetCheck(false);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1007_EditNavigationMesh()
{
	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(false);
	m_EditCheck_LightingInfo.SetCheck(false);
	m_EditCheck_NavigationMesh.SetCheck(true);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosX.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CTabMap::OnBnClickedRadio1005_StaticMeshCreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_StaticMeshRadio_CreateMode.SetCheck(true);
	m_StaticMeshRadio_ModifyMode.SetCheck(false);
	m_bIsCreateMode = true;
	m_bIsModifyMode = false;

	m_fStaticMeshScaleX = 0.01f;
	m_fStaticMeshScaleY = 0.01f;
	m_fStaticMeshScaleZ = 0.01f;
	m_fStaticMeshAngleX = 0.0f;
	m_fStaticMeshAngleY = 0.0f;
	m_fStaticMeshAngleZ = 0.0f;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1006_StaticMeshModifyeMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_StaticMeshRadio_CreateMode.SetCheck(false);
	m_StaticMeshRadio_ModifyMode.SetCheck(true);
	m_bIsCreateMode = false;
	m_bIsModifyMode = true;

	UpdateData(FALSE);
}


void CTabMap::OnNMClickTree1001_TreeMeshTag(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CPoint mouse;
	GetCursorPos(&mouse);
	m_StaticMeshTree_ResourceTree.ScreenToClient(&mouse);

	_uint iFlag = 0;
	HTREEITEM h_MeshTag = m_StaticMeshTree_ResourceTree.HitTest(mouse, &iFlag);

	// 클릭한 Tree의 문자열을 얻어온다.
	m_wstrTreeMeshTag = m_StaticMeshTree_ResourceTree.GetItemText(h_MeshTag);

	// 클릭한 문자열 설정.
	m_StaticMeshEdit_SelectMesthTag.SetWindowTextW(m_wstrTreeMeshTag.c_str());

	*pResult = 0;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1005_StaticMeshScaleX()
{
	UpdateData(TRUE);


	UpdateData(FALSE);

}



void CTabMap::OnEnChangeEdit1006_StaticMeshScaleY()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1007_StaticMeshScaleZ()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1008_StaticMeshAngleX()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1009_StaticMeshAngleY()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1010_StaticMeshAngleZ()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1011_StaticMeshPosX()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1012_StaticMeshPosY()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1013_StaticMeshPosZ()
{
	UpdateData(TRUE);


	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1002_StaticMeshRenderShadow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_StaticMeshCheck_IsRenderShadow.GetCheck())
		m_bIsRenderShadow = true;
	else
		m_bIsRenderShadow = false;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1003_StaticMeshIsCollision()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);


	if (m_StaticMeshCheck_IsCollision.GetCheck())
		m_bIsCollision = true;
	else
		m_bIsCollision = false;

	UpdateData(FALSE);

}


void CTabMap::OnLbnSelchangeList1003_StaticMeshObjectList()
{
	UpdateData(TRUE);

	Engine::OBJLIST* pStaticMeshList = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");
	// BoundingBox 색상 Green으로 변경.
	for (auto& pObject : *pStaticMeshList)
		pObject->Set_BoundingBoxColor(_rgba(0.0f, 1.0f, 0.0f, 1.0f));


	// 선택한 ListBox의 Index를 얻어온다.
	m_iStaticMeshSelectIdx = m_StaticMeshListBox_ObjectList.GetCaretIndex();

	// 선택한 Object의 색상을 Red로 변경.
	Engine::CGameObject* pSelectedObject = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"StaticMesh", m_iStaticMeshSelectIdx);
	if (nullptr != pSelectedObject)
	{
		pSelectedObject->Set_BoundingBoxColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

		// 선택한 Object의 정보로 Edit Control을 채운다.
		_tchar szTemp[MIN_STR] = L"";

		_stprintf_s(szTemp, MIN_STR, L"%0.001f", pSelectedObject->Get_Transform()->m_vScale.x);
		m_StaticMeshEdit_ScaleX.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.001f", pSelectedObject->Get_Transform()->m_vScale.y);
		m_StaticMeshEdit_ScaleY.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.001f", pSelectedObject->Get_Transform()->m_vScale.z);
		m_StaticMeshEdit_ScaleZ.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vAngle.x);
		m_StaticMeshEdit_AngleX.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vAngle.y);
		m_StaticMeshEdit_AngleY.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vAngle.z);
		m_StaticMeshEdit_AngleZ.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vPos.x);
		m_StaticMeshEdit_PosX.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vPos.y);
		m_StaticMeshEdit_PosY.SetWindowTextW(szTemp);
		_stprintf_s(szTemp, MIN_STR, L"%0.1f", pSelectedObject->Get_Transform()->m_vPos.z);
		m_StaticMeshEdit_PosZ.SetWindowTextW(szTemp);

		static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject = pSelectedObject;
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1001_StasticMeshDelete()
{
	// 선택한 StaticMeshObject 삭제.
	if (!m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->empty() &&
		m_iStaticMeshSelectIdx < m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->size())
	{
		m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"StaticMesh", m_iStaticMeshSelectIdx)->Set_DeadGameObject();

		// ListBox와 Edit컨트롤 수정.
		m_StaticMeshListBox_ObjectList.DeleteString(m_iStaticMeshSelectIdx);
	}
}


void CTabMap::OnBnClickedButton1002_StaticMeshAllDelete()
{
	// 모든 StaticMeshObject 삭제.
	if (!m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->empty())
	{
		Engine::OBJLIST* lstStaticMeshObject = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");

		for (auto& pStaticMesh : *lstStaticMeshObject)
			pStaticMesh->Set_DeadGameObject();

		// ListBox와 Edit컨트롤 수정.
		m_StaticMeshListBox_ObjectList.ResetContent();
	}

}
