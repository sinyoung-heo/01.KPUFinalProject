#include "stdafx.h"
#include "ToolSceneStage.h"

#include "ComponentMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "LightMgr.h"
#include "Font.h"
#include "ToolCamera.h"
#include "ToolCoordinate.h"
#include "ToolTerrain.h"
#include "ToolSkyBox.h"
#include "ToolStaticMesh.h"


CToolSceneStage::CToolSceneStage(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{

}

HRESULT CToolSceneStage::Ready_Scene()
{

	Engine::FAILED_CHECK_RETURN(Ready_LayerCamera(L"Layer_Camera"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerEnvironment(L"Layer_Environment"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerGameObject(L"Layer_GameObject"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerUI(L"Layer_UI"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerFont(L"Layer_Font"), E_FAIL);

	Engine::FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Stage Font ]
	____________________________________________________________________________________________________________*/
	m_pFont_Stage = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_Stage, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_Stage->Ready_GameObject(L"", _vec2(1410.f, 0.f), D2D1::ColorF::SpringGreen), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ MFC 컨트롤 활성화 ]
	____________________________________________________________________________________________________________*/
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm = static_cast<CMyForm*>(pMainFrame->m_MainSplit.GetPane(0, 0));
	pMyForm->m_TabMap.Ready_TerrainControl();
	pMyForm->m_TabMap.Ready_SkyBoxControl();
	pMyForm->m_TabMap.Ready_EditControl();
	pMyForm->m_TabMap.Ready_StaticMeshControl();
	m_pPickingTerrain = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));

	return S_OK;
}

_int CToolSceneStage::Update_Scene(const _float& fTimeDelta)
{
	CMainFrame* pMainFrame	= static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm*	pMyForm		= static_cast<CMyForm*>(pMainFrame->m_MainSplit.GetPane(0, 0));

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_PRESSING(DIK_LSHIFT))
		KeyInput();

	// CreateMode <-> ModifyMode
	if (Engine::KEY_DOWN(DIK_TAB))
	{
		if (pMyForm->m_bIsTabMap)
			KeyInput_ModeChange(pMyForm->m_TabMap);
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CToolSceneStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Stage Text ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pFont_Stage)
	{
		m_pFont_Stage->Update_GameObject(fTimeDelta);
		wsprintf(m_szFPS, L"StageScene FPS : %d", g_iFPS);
		m_pFont_Stage->Set_Text(wstring(m_szFPS));
	}

	/*__________________________________________________________________________________________________________
	[ MFC Control ]
	____________________________________________________________________________________________________________*/
	CMainFrame* pMainFrame	= static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm* pMyForm		= static_cast<CMyForm*>(pMainFrame->m_MainSplit.GetPane(0, 0));

	// StaticMeshObjectList Size
	if (pMyForm->m_bIsTabMap && 
		nullptr != m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh"))
	{
		pMyForm->m_TabMap.m_iStaticMeshObjectSize = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->size();

		_tchar szTemp[MIN_STR] = L"";
		wsprintf(szTemp, L"%d", pMyForm->m_TabMap.m_iStaticMeshObjectSize);
		pMyForm->m_TabMap.m_StaticMeshEdit_ObjectSize.SetWindowTextW(szTemp);
	}

	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CToolSceneStage::Render_Scene(const _float& fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

HRESULT CToolSceneStage::Ready_LayerCamera(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Camera Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	/*__________________________________________________________________________________________________________
	[ ToolCamera ]
	____________________________________________________________________________________________________________*/
	CToolCamera* pCToolCamera = CToolCamera::Create(m_pGraphicDevice, m_pCommandList,
													Engine::CAMERA_DESC(_vec3(4.0, 6.0f, -6.0f),		// Eye
																		_vec3(4.0f, 4.0f, 0.0f),		// At
																		_vec3(0.0f, 1.0f, 0.f)),		// Up
													Engine::PROJ_DESC(60.0f,							// FovY
																	  _float(WINCX) / _float(WINCY),	// Aspect
																	  1.0f,								// Near
																	  1000.0f),							// Far
													Engine::ORTHO_DESC(WINCX,							// Viewport Width
																	   WINCY,							// Viewport Height
																	   0.0f,							// Near
																	   1.0f));							// Far
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"ToolCamera", pCToolCamera), E_FAIL);


	return S_OK;
}

HRESULT CToolSceneStage::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	/*__________________________________________________________________________________________________________
	[ Coordinate ]
	____________________________________________________________________________________________________________*/
	CToolCoordinate* pCoordinate = CToolCoordinate::Create(m_pGraphicDevice, m_pCommandList,
														   _vec3(512.0f, 512.0f, 512.0f),	// Scale
														   _vec3(0.0f, 0.0f, 0.0f),			// Angle
														   _vec3(-0.05f, 0.0f, -0.05f));	// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"ToolCoordinate", pCoordinate), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Terrain ]
	____________________________________________________________________________________________________________*/
	CToolTerrain* pTerrain = nullptr;

	pTerrain = CToolTerrain::Create(m_pGraphicDevice, m_pCommandList, L"TerrainTex128");
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainTex128", pTerrain), E_FAIL);

	pTerrain = CToolTerrain::Create(m_pGraphicDevice, m_pCommandList, L"TerrainTex256");
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainTex256", pTerrain), E_FAIL);

	pTerrain = CToolTerrain::Create(m_pGraphicDevice, m_pCommandList, L"TerrainTex512");
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainTex512", pTerrain), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	CToolSkyBox* pSkyBox = CToolSkyBox::Create(m_pGraphicDevice, m_pCommandList,
											   L"SkyBox",							// Texture Tag
											   _vec3(512.f, 512.f, 512.f),			// Scale
											   _vec3(0.0f, 0.0f, 0.0f),				// Angle
											   _vec3(0.0f, 0.0f, 0.0f));			// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pSkyBox), E_FAIL);

	return S_OK;
}

HRESULT CToolSceneStage::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	return S_OK;
}

HRESULT CToolSceneStage::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CToolSceneStage::Ready_LayerFont(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	return S_OK;
}

HRESULT CToolSceneStage::Ready_LightInfo()
{
	Engine::D3DLIGHT tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(Engine::D3DLIGHT));

	// Direction
	tLightInfo.Type			= Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse		= _rgba(1.0f, 1.0f, 1.0f, 1.0f);
	tLightInfo.Specular		= _rgba(0.4f, 0.4f, 0.4f, 1.0f);
	tLightInfo.Ambient		= _rgba(0.3f, 0.3f, 0.3f, 1.0f);
	tLightInfo.Direction	= _vec4(-1.0f, -1.0f, -1.0f, 0.0f);
	Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice, 
																			 m_pCommandList, 
																			 Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL,
																			 tLightInfo), E_FAIL);

	return S_OK;
}

void CToolSceneStage::KeyInput()
{
	CMainFrame* pMainFrame	= static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm*	pMyForm		= static_cast<CMyForm*>(pMainFrame->m_MainSplit.GetPane(0, 0));

	/*__________________________________________________________________________________________________________
	[ Terrain Picking ]
	____________________________________________________________________________________________________________*/
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON(Engine::DIM_LB)))
	{
		// TabMap Mouse Picking Event
		if (pMyForm->m_bIsTabMap)
			KeyInput_TabMap(pMyForm->m_TabMap);

	}

}

void CToolSceneStage::KeyInput_TabMap(CTabMap& TabMap)
{
	
	// StaticMesh Object 생성.
	if (TabMap.m_EditCheck_StaticMesh.GetCheck() &&
		TabMap.m_bIsCreateMode)
	{
		if (nullptr != m_pPickingTerrain)
		{
			TabMap.UpdateData(TRUE);
			CToolStaticMesh* pStaticMesh = nullptr;

			wstring wstrMeshTag		= TabMap.m_wstrTreeMeshTag;
			_vec3 vScale			= _vec3(TabMap.m_fStaticMeshScaleX, TabMap.m_fStaticMeshScaleY, TabMap.m_fStaticMeshScaleZ);
			_vec3 vAngle			= _vec3(TabMap.m_fStaticMeshAngleX, TabMap.m_fStaticMeshAngleY, TabMap.m_fStaticMeshAngleZ);
			_vec3 vPickingPos		= CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);
			_bool bIsRenderShadow	= TabMap.m_bIsRenderShadow;
			_bool bIsCollision		= TabMap.m_bIsCollision;

			TabMap.m_fStaticMeshPosX = vPickingPos.x;
			TabMap.m_fStaticMeshPosY = vPickingPos.y;
			TabMap.m_fStaticMeshPosZ = vPickingPos.z;

			if (L"" == wstrMeshTag)
				return;
			pStaticMesh = CToolStaticMesh::Create(m_pGraphicDevice, m_pCommandList,
												  wstrMeshTag,
												  vScale,
												  vAngle,
												  vPickingPos,
												  bIsRenderShadow,
												  bIsCollision);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"StaticMesh", pStaticMesh), E_FAIL);

			// StaticMeshListBox에 삽입.
			TabMap.m_StaticMeshListBox_ObjectList.AddString(wstrMeshTag.c_str());

			// 생성 위치 Edit Control에 기록.
			_tchar szTemp[MIN_STR] = L"";

			
			_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.x);
			TabMap.m_StaticMeshEdit_PosX.SetWindowTextW(szTemp);
			TabMap.m_fStaticMeshPosX = vPickingPos.x;

			_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.y);
			TabMap.m_StaticMeshEdit_PosY.SetWindowTextW(szTemp);
			TabMap.m_fStaticMeshPosY = vPickingPos.y;

			_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.z);
			TabMap.m_StaticMeshEdit_PosZ.SetWindowTextW(szTemp);
			TabMap.m_fStaticMeshPosZ = vPickingPos.z;
			TabMap.UpdateData(FALSE);

		}
	}

	// StaticMesh를 선택하여 정보 수정.
	if (TabMap.m_EditCheck_StaticMesh.GetCheck() &&
		TabMap.m_bIsModifyMode)
	{
		// m_pPickingObject = nullptr;

		Engine::OBJLIST* pStaticMeshList = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");
		if (nullptr != pStaticMeshList)
		{
			if (CMouseMgr::Get_Instance()->Picking_Object(&m_pPickingObject, pStaticMeshList))
			{
				// 선택한 Object의 정보로 Edit Control을 채운다.
				_tchar szTemp[MIN_STR] = L"";

				TabMap.UpdateData(TRUE);
				TabMap.m_fStaticMeshScaleX = m_pPickingObject->Get_Transform()->m_vScale.x;
				TabMap.m_fStaticMeshScaleY = m_pPickingObject->Get_Transform()->m_vScale.y;
				TabMap.m_fStaticMeshScaleZ = m_pPickingObject->Get_Transform()->m_vScale.z;
				TabMap.m_fStaticMeshAngleX = m_pPickingObject->Get_Transform()->m_vAngle.x;
				TabMap.m_fStaticMeshAngleY = m_pPickingObject->Get_Transform()->m_vAngle.y;
				TabMap.m_fStaticMeshAngleZ = m_pPickingObject->Get_Transform()->m_vAngle.z;
				TabMap.m_fStaticMeshPosX	= m_pPickingObject->Get_Transform()->m_vPos.x;
				TabMap.m_fStaticMeshPosY	= m_pPickingObject->Get_Transform()->m_vPos.y;
				TabMap.m_fStaticMeshPosZ	= m_pPickingObject->Get_Transform()->m_vPos.z;
				TabMap.UpdateData(FALSE);
			}
		}
		

	}
	
}

void CToolSceneStage::KeyInput_ModeChange(CTabMap& TabMap)
{
	TabMap.UpdateData(TRUE);

	if (TabMap.m_EditCheck_StaticMesh.GetCheck())
	{
		TabMap.m_bIsCreateMode = !TabMap.m_bIsCreateMode;
		TabMap.m_bIsModifyMode = !TabMap.m_bIsModifyMode;

		TabMap.m_StaticMeshRadio_CreateMode.SetCheck(TabMap.m_bIsCreateMode);
		TabMap.m_StaticMeshRadio_ModifyMode.SetCheck(TabMap.m_bIsModifyMode);
		// Object 생성 모드일 경우.
		if (TabMap.m_bIsCreateMode)
		{
			TabMap.m_fStaticMeshScaleX = 0.01f;
			TabMap.m_fStaticMeshScaleY = 0.01f;
			TabMap.m_fStaticMeshScaleZ = 0.01f;
			TabMap.m_fStaticMeshAngleX = 0.0f;
			TabMap.m_fStaticMeshAngleY = 0.0f;
			TabMap.m_fStaticMeshAngleZ = 0.0f;
		}
		// Object 수정 모드일 경우.
		else if (TabMap.m_bIsModifyMode)
		{

		}
	}

	TabMap.UpdateData(FALSE);
}

CToolSceneStage* CToolSceneStage::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CToolSceneStage* pInstance = new CToolSceneStage(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolSceneStage::Free()
{
	Engine::CScene::Free();

	if (nullptr != m_pFont_Stage)
		Engine::Safe_Release(m_pFont_Stage);
}
