#include "stdafx.h"
#include "ToolSceneStage.h"
#include "ComponentMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "Light.h"
#include "LightMgr.h"
#include "Font.h"
#include "ToolCamera.h"
#include "ToolCoordinate.h"
#include "ToolTerrain.h"
#include "ToolSkyBox.h"
#include "ToolStaticMesh.h"
#include "Popori_F.h"
#include "ToolCell.h"
#include "ToolUICanvas.h"
#include "ToolGridLine.h"
#include "ToolGridRect.h"
#include "ToolUIRoot.h"
#include "ToolUIChild.h"


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

	CMouseMgr::Get_Instance()->Ready_MouseMgr();

	/*__________________________________________________________________________________________________________
	[ NaviMesh Cell Picking Collider ]
	____________________________________________________________________________________________________________*/
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_pPickingCollider[i] = static_cast<Engine::CColliderSphere*>(Engine::CComponentMgr::Get_Instance()->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pPickingCollider[i], E_FAIL);

		m_matColliderWorld[i] = XMMatrixTranslation(1000.0f, 1000.0f, 1000.0f);
		m_pPickingCollider[i]->Set_ParentMatrix(&m_matColliderWorld[i]);	// Parent Matrix
		m_pPickingCollider[i]->Set_Scale(_vec3(0.55f, 0.55f, 0.55f));		// Collider Scale
		m_pPickingCollider[i]->Set_Radius(_vec3(1.f, 1.f, 1.f));			// Collider Radius
		m_pPickingCollider[i]->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
		m_pPickingCollider[i]->Set_PipelineStatePass(0);
	}

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
	
	// Tab Map
	pMyForm->m_TabMap.Ready_TerrainControl();
	pMyForm->m_TabMap.Ready_SkyBoxControl();
	pMyForm->m_TabMap.Ready_EditControl();
	pMyForm->m_TabMap.Ready_StaticMeshControl();
	pMyForm->m_TabMap.Ready_LightingInfoContorl();
	pMyForm->m_TabMap.Ready_NavigationMeshControl();
	m_pPickingTerrain = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));

	// Tab UI
	pMyForm->m_TabUI.m_TabTexSpriteUV.Ready_TabTexSpriteUV();
	pMyForm->m_TabUI.m_Tab2DUI.Ready_Tab2DUI();
	pMyForm->m_TabUI.m_TabTexSpriteUV.m_pToolUICanvas = m_pUICanvas;

	//for (_uint i = 0; i < 512; ++i)
	//{
	//	// Engine::CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(Engine::COLOR_BUFFER::BUFFER_RECT);
	//	// Engine::CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(Engine::COLOR_BUFFER::BUFFER_CUBE);
	//	Engine::CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(Engine::COLOR_BUFFER::BUFFER_BOX);
	//	Engine::CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(Engine::COLOR_BUFFER::BUFFER_SPHERE);
	//}

	Engine::CShaderColorInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	Engine::CShaderLightingInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);

	return S_OK;
}

_int CToolSceneStage::Update_Scene(const _float& fTimeDelta)
{
	CMainFrame* pMainFrame	= static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMyForm*	pMyForm		= static_cast<CMyForm*>(pMainFrame->m_MainSplit.GetPane(0, 0));

	/*__________________________________________________________________________________________________________
	[ NaviMesh Picking Collider ]
	____________________________________________________________________________________________________________*/
	if (pMyForm->m_TabMap.m_EditCheck_NavigationMesh.GetCheck() && 
		pMyForm->m_TabMap.m_bIsNaviCreateMode)
	{
		// Collider 위치 갱신.
		for (_int i = 0; i < POINT_END; ++i)
		{
			m_matColliderWorld[i] = XMMatrixTranslation(m_vPickingPoint[i].x, m_vPickingPoint[i].y, m_vPickingPoint[i].z);
			m_pPickingCollider[i]->Set_ParentMatrix(&m_matColliderWorld[i]);
			m_pPickingCollider[i]->Update_Component(fTimeDelta);
		}
	}

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_PRESSING(DIK_LSHIFT))
		KeyInput();

	// CreateMode <-> ModifyMode
	if (Engine::KEY_DOWN(DIK_TAB))
	{
		if (pMyForm->m_bIsTabMap)
			KeyInput_TabMapModeChange(pMyForm->m_TabMap);

		if (pMyForm->m_TabUI.m_bIsTab2DUI)
			KeyInput_Tab2DUIModeChange(pMyForm->m_TabUI.m_Tab2DUI);
	}

	// NavigationMesh ModeChange (Find Near Point)
	if (Engine::KEY_DOWN(DIK_LCONTROL))
	{
		if (pMyForm->m_bIsTabMap &&
			pMyForm->m_TabMap.m_EditCheck_NavigationMesh.GetCheck() &&
			pMyForm->m_TabMap.m_bIsNaviCreateMode)
		{
			pMyForm->m_TabMap.UpdateData(TRUE);

			_bool bIsFindNear = pMyForm->m_TabMap.m_NaviMeshCheck_FindNearPoint.GetCheck();
			pMyForm->m_TabMap.m_NaviMeshCheck_FindNearPoint.SetCheck(!bIsFindNear);

			pMyForm->m_TabMap.UpdateData(FALSE);
		}
	}

	CMouseMgr::Get_Instance()->Update_MouseMgr(fTimeDelta);
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
		pMyForm->m_TabMap.m_iStaticMeshObjectSize = (_int)m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->size();

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
													Engine::CAMERA_DESC(_vec3(8.0, 35.0f, -23.0f),		// Eye
																		_vec3(16.0f, 22.0f, 0.0f),		// At
																		_vec3(0.0f, 1.0f, 0.f)),		// Up
													Engine::PROJ_DESC(60.0f,							// FovY
																	  _float(WINCX) / _float(WINCY),	// Aspect
																	  0.1f,								// Near
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
														   _vec3(5120.0f, 5120.0f, 5120.0f),	// Scale
														   _vec3(0.0f, 0.0f, 0.0f),				// Angle
														   _vec3(-0.05f, 0.0f, -0.05f));		// Pos
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
											   _vec3(900.0f, 900.0f, 900.0f),		// Scale
											   _vec3(0.0f, 0.0f, 0.0f),				// Angle
											   _vec3(128.0f, 0.0f, 128.0f));		// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pSkyBox), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ BumpTerrainMesh ]
	____________________________________________________________________________________________________________*/
	//Engine::CGameObject* pGameObj = nullptr;
	//_vec3 vPos    = _vec3(0.0f, -0.1f, 0.5f);
	//_vec3 vOffset = _vec3(13.83f, 0.0f, 11.98f);

	//for (_int i = 0; i < 21; ++i)
	//{
	//	if (0 == i % 2)
	//		vPos.x = 0.0f;
	//	else
	//		vPos.x = 6.915f;

	//	for (_int j = 0; j < 21; ++j)
	//	{
	//		pGameObj = CToolStaticMesh::Create(m_pGraphicDevice, m_pCommandList,
	//										   L"BumpTerrainMesh01",		// MeshTag
	//										   _vec3(0.003f),				// Scale
	//										   _vec3(90.0f, 0.0f, 0.0f),	// Angle
	//										   vPos,						// Pos
	//										   true,						// Render Shadow
	//										   false,						// Bounding Box
	//										   false,						// Bounding Sphere
	//										   _vec3(0.0f),					// Bounding Sphere Scale
	//										   _vec3(0.0f),					// Bounding Sphere Pos
	//										   false);
	//		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainMesh", pGameObj), E_FAIL);
	//		vPos.x += vOffset.x;
	//	}

	//	vPos.z += vOffset.z;
	//}


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


	/*__________________________________________________________________________________________________________
	[ Popori_F ]
	____________________________________________________________________________________________________________*/
	CPopori_F* pPopori_F = nullptr;

	//pPopori_F =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
	//							  L"PoporiR19",						// MeshTag
	//							  _vec3(0.05f, 0.05f, 0.05f),		// Scale
	//							  _vec3(0.0f, 180.0f, 0.0f),		// Angle
	//							  _vec3(0.0f, 0.0f, 0.0f));			// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);

	//pPopori_F =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
	//							  L"PoporiR19",						// MeshTag
	//							  _vec3(0.05f, 0.05f, 0.05f),		// Scale
	//							  _vec3(0.0f, 0.0f, 0.0f),			// Angle
	//							  _vec3(2.0f, 0.0f, 0.0f));			// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);

	pPopori_F =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
								  L"PoporiR19",						// MeshTag
								  _vec3(0.05f, 0.05f, 0.05f),		// Scale
								  _vec3(0.0f, 0.0f, 0.0f),			// Angle
								  _vec3(95.0f, 0.f, 90.0f));		// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);

	pPopori_F =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
								  L"PoporiR19",						// MeshTag
								  _vec3(0.05f, 0.05f, 0.05f),		// Scale
								  _vec3(0.0f, 0.0f, 0.0f),			// Angle
								  _vec3(180.0f, 0.f, 75.0f));		// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);

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

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ UICanvas ]
	____________________________________________________________________________________________________________*/
	pGameObj = CToolUICanvas::Create(m_pGraphicDevice, m_pCommandList,
									 _vec3(0.0f),
									 _vec3(0.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"UICanvas", pGameObj), E_FAIL);
	m_pUICanvas = static_cast<CToolUICanvas*>(pGameObj);

	/*__________________________________________________________________________________________________________
	[ UIGridLine ]
	____________________________________________________________________________________________________________*/
	pGameObj = CToolGridLine::Create(m_pGraphicDevice, m_pCommandList,
									 _vec3(0.0f),
									 _vec3(0.0f),
									 900);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"UIGridLineWidth", pGameObj), E_FAIL);
	
	pGameObj = CToolGridLine::Create(m_pGraphicDevice, m_pCommandList,
									 _vec3(0.0f),
									 _vec3(0.0f),
									 900);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"UIGridLineHeight", pGameObj), E_FAIL);
	
	/*__________________________________________________________________________________________________________
	[ UIGridRect ]
	____________________________________________________________________________________________________________*/
	pGameObj = CToolGridRect::Create(m_pGraphicDevice, m_pCommandList,
									 _vec3(0.0f),
									 _vec3(0.0f),
									 950);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"UIGridRect", pGameObj), E_FAIL);


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
	tLightInfo.Ambient		= _rgba(0.0f, 0.0f, 0.0f, 1.0f);
	tLightInfo.Direction	= _vec4(-1.0f, -1.0f, 0.5f, 0.0f);
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
	[ L Button Picking ]
	____________________________________________________________________________________________________________*/
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON(Engine::DIM_LB)))
	{
		// TabMap Mouse Picking Event. (STATIC MESH / LIGHTING / NAVIGATION MESH)
#pragma region TABMAP_MOUSEPICKING
		if (pMyForm->m_bIsTabMap)
		{
			if (pMyForm->m_TabMap.m_EditCheck_StaticMesh.GetCheck())
				KeyInput_TabMapStaticMesh(pMyForm->m_TabMap);

			else if (pMyForm->m_TabMap.m_EditCheck_LightingInfo.GetCheck())
				KeyInput_TabMapLightingInfo(pMyForm->m_TabMap);

			else if (pMyForm->m_TabMap.m_EditCheck_NavigationMesh.GetCheck())
				KeyInput_TabMapNavigationMesh(pMyForm->m_TabMap);
		}
#pragma endregion



		// TabMap Mouse Picking Event.
#pragma region TABUI_MOUSEPICKING
		else if (pMyForm->m_bIsTabUI)
		{
			if (pMyForm->m_TabUI.m_bIsTabTexSpriteUV)
				KeyInput_TabUITexSpriteUV(pMyForm->m_TabUI.m_TabTexSpriteUV);
			else if (pMyForm->m_TabUI.m_bIsTab2DUI)
				KeyInput_TabUI2DUI(pMyForm->m_TabUI.m_Tab2DUI);
		}
#pragma endregion
	}

	// NaviMesh 수정모드일 경우.
#pragma region TABMAP_NAVIMESH
	if (pMyForm->m_TabMap.m_EditCheck_NavigationMesh.GetCheck() && pMyForm->m_TabMap.m_bIsNaviModifyMode)
	{
		_long	dwMouseMove = 0;

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON(Engine::DIM_LB)))
		{
			// if (m_vPrePickingPos != m_vCurPickingPos)
			{
				m_vCurPickingPos = CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);
				m_vPrePickingPos = m_vCurPickingPos;

				if (nullptr != m_pNearPoint)
				{
					m_pNearPoint->x = m_vCurPickingPos.x;
					m_pNearPoint->z = m_vCurPickingPos.z;

					pMyForm->m_TabMap.UpdateData(TRUE);
					pMyForm->m_TabMap.m_fNaviMeshPointA_X	= m_pPickingCell->m_pPoint[POINT_A]->x;
					pMyForm->m_TabMap.m_fNaviMeshPointA_Y	= m_pPickingCell->m_pPoint[POINT_A]->y;
					pMyForm->m_TabMap.m_fNaviMeshPointA_Z	= m_pPickingCell->m_pPoint[POINT_A]->z;
					pMyForm->m_TabMap.m_fNaviMeshPointB_X	= m_pPickingCell->m_pPoint[POINT_B]->x;
					pMyForm->m_TabMap.m_fNaviMeshPointB_Y	= m_pPickingCell->m_pPoint[POINT_B]->y;
					pMyForm->m_TabMap.m_fNaviMeshPointB_Z	= m_pPickingCell->m_pPoint[POINT_B]->z;
					pMyForm->m_TabMap.m_fNaviMeshPointC_X	= m_pPickingCell->m_pPoint[POINT_C]->x;
					pMyForm->m_TabMap.m_fNaviMeshPointC_Y	= m_pPickingCell->m_pPoint[POINT_C]->y;
					pMyForm->m_TabMap.m_fNaviMeshPointC_Z	= m_pPickingCell->m_pPoint[POINT_C]->z;
					pMyForm->m_TabMap.m_iNaviMeshCellOption = m_pPickingCell->m_iOption;
					pMyForm->m_TabMap.UpdateData(FALSE);
				}
			}

		}

		else if (dwMouseMove = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Z))
		{
			pMyForm->m_TabMap.UpdateData(TRUE);

			if (nullptr != m_pNearPoint)
			{
				cout << dwMouseMove << endl;

				if (dwMouseMove > 0)
					m_pNearPoint->y += 0.5f;
				else
					m_pNearPoint->y -= 0.5f;

				pMyForm->m_TabMap.m_fNaviMeshPointA_X	= m_pPickingCell->m_pPoint[POINT_A]->x;
				pMyForm->m_TabMap.m_fNaviMeshPointA_Y	= m_pPickingCell->m_pPoint[POINT_A]->y;
				pMyForm->m_TabMap.m_fNaviMeshPointA_Z	= m_pPickingCell->m_pPoint[POINT_A]->z;
				pMyForm->m_TabMap.m_fNaviMeshPointB_X	= m_pPickingCell->m_pPoint[POINT_B]->x;
				pMyForm->m_TabMap.m_fNaviMeshPointB_Y	= m_pPickingCell->m_pPoint[POINT_B]->y;
				pMyForm->m_TabMap.m_fNaviMeshPointB_Z	= m_pPickingCell->m_pPoint[POINT_B]->z;
				pMyForm->m_TabMap.m_fNaviMeshPointC_X	= m_pPickingCell->m_pPoint[POINT_C]->x;
				pMyForm->m_TabMap.m_fNaviMeshPointC_Y	= m_pPickingCell->m_pPoint[POINT_C]->y;
				pMyForm->m_TabMap.m_fNaviMeshPointC_Z	= m_pPickingCell->m_pPoint[POINT_C]->z;
				pMyForm->m_TabMap.m_iNaviMeshCellOption = m_pPickingCell->m_iOption;
			}

			pMyForm->m_TabMap.UpdateData(FALSE);
		}

	}
#pragma endregion


#pragma region TOOLROOTUI_MODIFY
	// ToolRootUI 수정모드일 경우.
	if (pMyForm->m_TabUI.m_bIsTab2DUI &&
		pMyForm->m_TabUI.m_Tab2DUI.m_bIsRootModifyMode && nullptr != m_pPickingRootUI)
	{

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON(Engine::DIM_LB)))
		{
			pMyForm->m_TabUI.m_Tab2DUI.UpdateData(TRUE);
			
			CMouseMgr::Get_CursorPoint().x;
			m_pPickingRootUI->Get_Transform()->m_vPos.x = (_float)(CMouseMgr::Get_CursorPoint().x);
			m_pPickingRootUI->Get_Transform()->m_vPos.y = (_float)(CMouseMgr::Get_CursorPoint().y);

			pMyForm->m_TabUI.m_Tab2DUI.m_fRootPosX = (_float)(CMouseMgr::Get_CursorPoint().x);
			pMyForm->m_TabUI.m_Tab2DUI.m_fRootPosY = (_float)(CMouseMgr::Get_CursorPoint().y);

			pMyForm->m_TabUI.m_Tab2DUI.UpdateData(FALSE);
		}

	}
#pragma endregion
}




#pragma region TABMAP_KEYINPUT
void CToolSceneStage::KeyInput_TabMapStaticMesh(CTabMap& TabMap)
{
	// StaticMesh Object 생성.
	if (TabMap.m_bIsCreateMode)
	{
		if (nullptr != m_pPickingTerrain)
		{
			TabMap.UpdateData(TRUE);
			CToolStaticMesh* pStaticMesh = nullptr;

			_vec3 vScale				= _vec3(TabMap.m_fStaticMeshScaleX, TabMap.m_fStaticMeshScaleY, TabMap.m_fStaticMeshScaleZ);
			_vec3 vAngle				= _vec3(TabMap.m_fStaticMeshAngleX, TabMap.m_fStaticMeshAngleY, TabMap.m_fStaticMeshAngleZ);
			_vec3 vPickingPos			= CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);
			_vec3 vBoundingSpherePos	= _vec3(TabMap.m_fStaticMeshColliderPosX, TabMap.m_fStaticMeshColliderPosY, TabMap.m_fStaticMeshColliderPosZ);

			TabMap.m_fStaticMeshPosX = vPickingPos.x;
			TabMap.m_fStaticMeshPosY = vPickingPos.y;
			TabMap.m_fStaticMeshPosZ = vPickingPos.z;

			if (L"" == TabMap.m_wstrTreeMeshTag)
				return;
			pStaticMesh = CToolStaticMesh::Create(m_pGraphicDevice, m_pCommandList,
												  TabMap.m_wstrTreeMeshTag,					// MeshTag
												  vScale,									// Scale
												  vAngle,									// Angle
												  vPickingPos,								// Pos
												  TabMap.m_bIsRenderShadow,					// Render Shadow
												  true,										// Bounding Box
												  TabMap.m_bIsCollision,					// Bounding Sphere
												  _vec3(TabMap.m_fStaticMeshColliderScale),	// Bounding Sphere Scale
												   vBoundingSpherePos,						// Bounding Sphere Pos
												  TabMap.m_bIsMousePicking);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"StaticMesh", pStaticMesh), E_FAIL);

			// StaticMeshListBox에 삽입.
			TabMap.m_StaticMeshListBox_ObjectList.AddString(TabMap.m_wstrTreeMeshTag.c_str());

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
	if (TabMap.m_bIsModifyMode)
	{
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
				TabMap.m_fStaticMeshPosX = m_pPickingObject->Get_Transform()->m_vPos.x;
				TabMap.m_fStaticMeshPosY = m_pPickingObject->Get_Transform()->m_vPos.y;
				TabMap.m_fStaticMeshPosZ = m_pPickingObject->Get_Transform()->m_vPos.z;

				TabMap.m_bIsRenderShadow = static_cast<CToolStaticMesh*>(m_pPickingObject)->Get_IsRenderShadow();
				TabMap.m_bIsCollision = static_cast<CToolStaticMesh*>(m_pPickingObject)->Get_IsCollision();
				TabMap.m_fStaticMeshColliderScale = m_pPickingObject->Get_BoundingSphere()->Get_Transform()->m_vScale.x;
				TabMap.m_fStaticMeshColliderPosX = m_pPickingObject->Get_BoundingSphere()->Get_Transform()->m_vPos.x;
				TabMap.m_fStaticMeshColliderPosY = m_pPickingObject->Get_BoundingSphere()->Get_Transform()->m_vPos.y;
				TabMap.m_fStaticMeshColliderPosZ = m_pPickingObject->Get_BoundingSphere()->Get_Transform()->m_vPos.z;

				if (TabMap.m_bIsRenderShadow)
					TabMap.m_StaticMeshCheck_IsRenderShadow.SetCheck(true);
				else
					TabMap.m_StaticMeshCheck_IsRenderShadow.SetCheck(false);

				if (TabMap.m_bIsCollision)
				{
					TabMap.m_StaticMeshCheck_IsCollision.SetCheck(true);
					TabMap.m_StaticMeshEdit_ColliderScale.EnableWindow(TRUE);
					TabMap.m_StaticMeshEdit_ColliderPosX.EnableWindow(TRUE);
					TabMap.m_StaticMeshEdit_ColliderPosY.EnableWindow(TRUE);
					TabMap.m_StaticMeshEdit_ColliderPosZ.EnableWindow(TRUE);
				}
				else
				{
					TabMap.m_StaticMeshCheck_IsCollision.SetCheck(false);
					TabMap.m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
					TabMap.m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
					TabMap.m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
					TabMap.m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
				}

				TabMap.m_iStaticMeshSelectIdx = -1;

				TabMap.UpdateData(FALSE);
			}
		}

	}

}

void CToolSceneStage::KeyInput_TabMapLightingInfo(CTabMap& TabMap)
{
	// PointLight 생성.
	if (TabMap.m_bIsLightingCreateMode)
	{
		TabMap.UpdateData(TRUE);

		_vec3	vPickingPos	= CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);

		_rgba	PL_Diffuse	= _rgba(TabMap.m_fLightInfo_PL_DiffuseR, TabMap.m_fLightInfo_PL_DiffuseG, TabMap.m_fLightInfo_PL_DiffuseB, TabMap.m_fLightInfo_PL_DiffuseA);
		_rgba	PL_Specular = _rgba(TabMap.m_fLightInfo_PL_SpecularR, TabMap.m_fLightInfo_PL_SpecularG, TabMap.m_fLightInfo_PL_SpecularB, TabMap.m_fLightInfo_PL_SpecularA);
		_rgba	PL_Ambient	= _rgba(TabMap.m_fLightInfo_PL_AmbientR, TabMap.m_fLightInfo_PL_AmbientG, TabMap.m_fLightInfo_PL_AmbientB, TabMap.m_fLightInfo_PL_AmbientA);
		_vec4	vPos		= _vec4(vPickingPos, TabMap.m_fLightInfo_PL_PosW);
		_float	fRange		= TabMap.m_fLightInfo_PL_Range;

		Engine::D3DLIGHT tLightInfo;
		ZeroMemory(&tLightInfo, sizeof(Engine::D3DLIGHT));
		tLightInfo.Type			= Engine::LIGHTTYPE::D3DLIGHT_POINT;
		tLightInfo.Diffuse		= PL_Diffuse;
		tLightInfo.Specular		= PL_Specular;
		tLightInfo.Ambient		= PL_Ambient;
		tLightInfo.Position		= _vec4(vPos.x, vPos.y, vPos.z, 1.0f);
		tLightInfo.Range		= fRange;
		Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice,
																				 m_pCommandList,
																				 Engine::LIGHTTYPE::D3DLIGHT_POINT,
																				 tLightInfo), E_FAIL);

		// ListBox 추가.
		_uint iSize = (_uint)Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).size();
		_tchar szTemp[MIN_STR] = L"";
		wsprintf(szTemp, L"Index : %d", iSize - 1);
		TabMap.m_LightInfoListBox_PL_List.AddString(szTemp);



		_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.x);
		TabMap.m_LightInfoEdit_PL_PosX.SetWindowTextW(szTemp);
		TabMap.m_fLightInfo_PL_PosX = vPickingPos.x;

		_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.y);
		TabMap.m_LightInfoEdit_PL_PosY.SetWindowTextW(szTemp);
		TabMap.m_fLightInfo_PL_PosY = vPickingPos.y;

		_stprintf_s(szTemp, MIN_STR, L"%0.1f", vPickingPos.z);
		TabMap.m_LightInfoEdit_PL_PosZ.SetWindowTextW(szTemp);
		TabMap.m_fLightInfo_PL_PosZ = vPickingPos.z;


		TabMap.UpdateData(FALSE);
	}

	// PointLight 수정.
	else if (TabMap.m_bIsLightingModifyMode)
	{
		TabMap.UpdateData(TRUE);

		vector<Engine::CLight*> vecPointLight = Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT);
		if (vecPointLight.empty())
			return;

		if (CMouseMgr::Get_Instance()->Picking_Light(&m_pPickingLight, vecPointLight))
		{
			// 선택한 Light의 정보로 Edit Control을 채운다.
			TabMap.m_fLightInfo_PL_DiffuseR		= m_pPickingLight->Get_LightInfo().Diffuse.x;
			TabMap.m_fLightInfo_PL_DiffuseG		= m_pPickingLight->Get_LightInfo().Diffuse.y;
			TabMap.m_fLightInfo_PL_DiffuseB		= m_pPickingLight->Get_LightInfo().Diffuse.z;
			TabMap.m_fLightInfo_PL_DiffuseA		= m_pPickingLight->Get_LightInfo().Diffuse.w;
			TabMap.m_fLightInfo_PL_SpecularR	= m_pPickingLight->Get_LightInfo().Specular.x;
			TabMap.m_fLightInfo_PL_SpecularG	= m_pPickingLight->Get_LightInfo().Specular.y;
			TabMap.m_fLightInfo_PL_SpecularB	= m_pPickingLight->Get_LightInfo().Specular.z;
			TabMap.m_fLightInfo_PL_SpecularA	= m_pPickingLight->Get_LightInfo().Specular.w;
			TabMap.m_fLightInfo_PL_AmbientR		= m_pPickingLight->Get_LightInfo().Ambient.x;
			TabMap.m_fLightInfo_PL_AmbientG		= m_pPickingLight->Get_LightInfo().Ambient.y;
			TabMap.m_fLightInfo_PL_AmbientB		= m_pPickingLight->Get_LightInfo().Ambient.z;
			TabMap.m_fLightInfo_PL_AmbientA		= m_pPickingLight->Get_LightInfo().Ambient.w;
			TabMap.m_fLightInfo_PL_PosX			= m_pPickingLight->Get_LightInfo().Position.x;
			TabMap.m_fLightInfo_PL_PosY			= m_pPickingLight->Get_LightInfo().Position.y;
			TabMap.m_fLightInfo_PL_PosZ			= m_pPickingLight->Get_LightInfo().Position.z;
			TabMap.m_fLightInfo_PL_PosW			= m_pPickingLight->Get_LightInfo().Position.w;
			TabMap.m_fLightInfo_PL_Range		= m_pPickingLight->Get_LightInfo().Range;

			TabMap.m_iSelectPLIdx = -1;
		}


		TabMap.UpdateData(FALSE);
	}
}

void CToolSceneStage::KeyInput_TabMapNavigationMesh(CTabMap& TabMap)
{
	TabMap.UpdateData(TRUE);

	Engine::OBJLIST*	pCellList	= Engine::CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_Environment", L"Cell");
	CToolCell*			pCell		= nullptr;

	// NavigationMesh Create 모드.
	if (TabMap.m_bIsNaviCreateMode)
	{
		_vec3 vPickingPos = CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);

		// Cell이 없을 때 (최초 생성시)
		if (nullptr == pCellList || pCellList->empty())
		{
			++m_iPickingCnt;

			if (POINT_A == m_iPickingCnt)
			{
				m_vPickingPoint[POINT_A] = vPickingPos;

				TabMap.m_fNaviMeshPointA_X = m_vPickingPoint[POINT_A].x;
				TabMap.m_fNaviMeshPointA_Y = m_vPickingPoint[POINT_A].y;
				TabMap.m_fNaviMeshPointA_Z = m_vPickingPoint[POINT_A].z;
				TabMap.m_fNaviMeshPointB_X = 0.0f;
				TabMap.m_fNaviMeshPointB_Y = 0.0f;
				TabMap.m_fNaviMeshPointB_Z = 0.0f;
				TabMap.m_fNaviMeshPointC_X = 0.0f;
				TabMap.m_fNaviMeshPointC_Y = 0.0f;
				TabMap.m_fNaviMeshPointC_Z = 0.0f;
			}

			else if (POINT_B == m_iPickingCnt)
			{
				m_vPickingPoint[POINT_B] = vPickingPos;

				TabMap.m_fNaviMeshPointB_X = m_vPickingPoint[POINT_B].x;
				TabMap.m_fNaviMeshPointB_Y = m_vPickingPoint[POINT_B].y;
				TabMap.m_fNaviMeshPointB_Z = m_vPickingPoint[POINT_B].z;
			}

			else if (POINT_C == m_iPickingCnt)
			{
				m_vPickingPoint[POINT_C] = vPickingPos;

				// Cell 생성.
				pCell = CToolCell::Create(m_pGraphicDevice, m_pCommandList,
										  0,								// Cell Index
										  m_vPickingPoint[POINT_A],			// Point A
										  m_vPickingPoint[POINT_B],			// Point B
										  m_vPickingPoint[POINT_C],			// Point C
										  TabMap.m_iNaviMeshCellOption);	// Option
				m_pObjectMgr->Add_GameObject(L"Layer_Environment", L"Cell", pCell);
				pCellList = Engine::CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_Environment", L"Cell");

				// ListBox에 추가.
				_tchar szTemp[MIN_STR] = L"";
				wsprintf(szTemp, L"Index : %d", (_uint)pCellList->size() - 1);
				TabMap.m_NaviMeshListBox_CellList.AddString(szTemp);

				// EditContorl에 값 추가.
				TabMap.m_fNaviMeshPointC_X = m_vPickingPoint[POINT_C].x;
				TabMap.m_fNaviMeshPointC_Y = m_vPickingPoint[POINT_C].y;
				TabMap.m_fNaviMeshPointC_Z = m_vPickingPoint[POINT_C].z;

				m_vPickingPoint[POINT_A] = _vec3(1000.0f);
				m_vPickingPoint[POINT_B] = _vec3(1000.0f);
				m_vPickingPoint[POINT_C] = _vec3(1000.0f);
				m_iPickingCnt = -1;
			}
		}

		// 1개 이상의 Cell이 있을 때.
		else
		{
			// Picking지점과 가장 인접한 Cell을 찾는다.
			++m_iPickingCnt;
			_vec3* pOut = CMouseMgr::Get_Instance()->Find_NearCellPoint(vPickingPos, &m_pPickingCell);

			if (POINT_A == m_iPickingCnt)
			{
				m_vPickingPoint[POINT_A] = *pOut;
				m_pPickingPoint[POINT_A] = pOut;

				TabMap.m_fNaviMeshPointA_X = m_vPickingPoint[POINT_A].x;
				TabMap.m_fNaviMeshPointA_Y = m_vPickingPoint[POINT_A].y;
				TabMap.m_fNaviMeshPointA_Z = m_vPickingPoint[POINT_A].z;
				TabMap.m_fNaviMeshPointB_X = 0.0f;
				TabMap.m_fNaviMeshPointB_Y = 0.0f;
				TabMap.m_fNaviMeshPointB_Z = 0.0f;
				TabMap.m_fNaviMeshPointC_X = 0.0f;
				TabMap.m_fNaviMeshPointC_Y = 0.0f;
				TabMap.m_fNaviMeshPointC_Z = 0.0f;
			}

			else if (POINT_B == m_iPickingCnt)
			{
				if (TabMap.m_NaviMeshCheck_FindNearPoint.GetCheck())
				{
					m_vPickingPoint[POINT_B] = *pOut;
					m_pPickingPoint[POINT_B] = pOut;
				}
				else
					m_vPickingPoint[POINT_B] = vPickingPos;

				TabMap.m_fNaviMeshPointB_X = m_vPickingPoint[POINT_B].x;
				TabMap.m_fNaviMeshPointB_Y = m_vPickingPoint[POINT_B].y;
				TabMap.m_fNaviMeshPointB_Z = m_vPickingPoint[POINT_B].z;
			}

			else if (POINT_C == m_iPickingCnt)
			{
				m_vPickingPoint[POINT_C] = *pOut;
				m_pPickingPoint[POINT_C] = pOut;

				if (!TabMap.m_NaviMeshCheck_FindNearPoint.GetCheck())
				{
					// Cell 생성 - 두 개의 점 공유받아서 생성.
					pCell = CToolCell::ShareCreate(m_pGraphicDevice, m_pCommandList,
												   (_ulong)pCellList->size(),		// Cell Index
												   m_pPickingPoint[POINT_A],		// Point A
												   m_vPickingPoint[POINT_B],		// Point B
												   m_pPickingPoint[POINT_C],		// Point C
												   TabMap.m_iNaviMeshCellOption,	// Option
												   TabMap.m_NaviMeshCheck_FindNearPoint.GetCheck());
				}
				else
				{
					// Cell 생성 - 모든 점을 공유받아서 생성.
					pCell = CToolCell::ShareCreate(m_pGraphicDevice, m_pCommandList,
												   (_ulong)pCellList->size(),		// Cell Index
												   m_pPickingPoint[POINT_A],		// Point A
												   m_pPickingPoint[POINT_B],		// Point B
												   m_pPickingPoint[POINT_C],		// Point C
												   TabMap.m_iNaviMeshCellOption,	// Option
												   TabMap.m_NaviMeshCheck_FindNearPoint.GetCheck());
				}

				m_pObjectMgr->Add_GameObject(L"Layer_Environment", L"Cell", pCell);
				pCellList = Engine::CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_Environment", L"Cell");

				// ListBox에 추가.
				_tchar szTemp[MIN_STR] = L"";
				wsprintf(szTemp, L"Index : %d", (_uint)pCellList->size() - 1);
				TabMap.m_NaviMeshListBox_CellList.AddString(szTemp);

				// EditContorl에 값 추가.
				TabMap.m_fNaviMeshPointC_X = m_vPickingPoint[POINT_C].x;
				TabMap.m_fNaviMeshPointC_Y = m_vPickingPoint[POINT_C].y;
				TabMap.m_fNaviMeshPointC_Z = m_vPickingPoint[POINT_C].z;

				// 값 초기화.
				m_vPickingPoint[POINT_A] = _vec3(1000.0f);
				m_vPickingPoint[POINT_B] = _vec3(1000.0f);
				m_vPickingPoint[POINT_C] = _vec3(1000.0f);
				m_pPickingPoint[POINT_A] = nullptr;
				m_pPickingPoint[POINT_B] = nullptr;
				m_pPickingPoint[POINT_C] = nullptr;
				m_iPickingCnt = -1;
			}

		}

	}

	// NavigationMesh Modify 모드.
	else if (TabMap.m_bIsNaviModifyMode)
	{
		Engine::OBJLIST* pCellList = Engine::CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_Environment", L"Cell");
		if (nullptr == pCellList || pCellList->empty())
			return;

		// Cell 색상 Green & WireFrame으로 변경.
		for (auto& pCell : *pCellList)
			static_cast<CToolCell*>(pCell)->Reset_CellAndCollider();

		// Picking 지점과 가장 가까운 점을 찾는다.
		_vec3 vPickingPos	= CMouseMgr::Picking_OnTerrain(m_pPickingTerrain);
		m_pNearPoint		= CMouseMgr::Get_Instance()->Find_NearCellPoint(vPickingPos, &m_pPickingCell);
		m_vCurPickingPos = *m_pNearPoint;
	}

	TabMap.UpdateData(FALSE);
}

void CToolSceneStage::KeyInput_TabMapModeChange(CTabMap& TabMap)
{
	TabMap.UpdateData(TRUE);

	// StaticMesh ModeChange
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

			TabMap.m_fStaticMeshColliderScale = 0.0f;
			TabMap.m_fStaticMeshColliderPosX = 0.0f;
			TabMap.m_fStaticMeshColliderPosY = 0.0f;
			TabMap.m_fStaticMeshColliderPosZ = 0.0f;
		}
		// Object 수정 모드일 경우.
		else if (TabMap.m_bIsModifyMode)
		{

		}
	}

	// LightingInfo ModeChange
	else if (TabMap.m_EditCheck_LightingInfo.GetCheck())
	{
		TabMap.m_bIsLightingCreateMode = !TabMap.m_bIsLightingCreateMode;
		TabMap.m_bIsLightingModifyMode = !TabMap.m_bIsLightingModifyMode;

		// TabMap.
		TabMap.m_LightInfoRadio_PL_CreateMode.SetCheck(TabMap.m_bIsLightingCreateMode);
		TabMap.m_LightInfoRadio_PL_ModifyMode.SetCheck(TabMap.m_bIsLightingModifyMode);

		if (TabMap.m_bIsLightingCreateMode)
		{
			TabMap.m_fLightInfo_PL_DiffuseR		= 1.0f;
			TabMap.m_fLightInfo_PL_DiffuseG		= 1.0f;
			TabMap.m_fLightInfo_PL_DiffuseB		= 1.0f;
			TabMap.m_fLightInfo_PL_DiffuseA		= 1.0f;
			TabMap.m_fLightInfo_PL_SpecularR	= 0.5f;
			TabMap.m_fLightInfo_PL_SpecularG	= 0.5f;
			TabMap.m_fLightInfo_PL_SpecularB	= 0.5f;
			TabMap.m_fLightInfo_PL_SpecularA	= 1.0f;
			TabMap.m_fLightInfo_PL_AmbientR		= 0.5f;
			TabMap.m_fLightInfo_PL_AmbientG		= 0.5f;
			TabMap.m_fLightInfo_PL_AmbientB		= 0.5f;
			TabMap.m_fLightInfo_PL_AmbientA		= 1.0f;
			TabMap.m_fLightInfo_PL_PosX			= 0.0f;
			TabMap.m_fLightInfo_PL_PosY			= 0.0f;
			TabMap.m_fLightInfo_PL_PosZ			= 0.0f;
			TabMap.m_fLightInfo_PL_PosW			= 1.0f;
			TabMap.m_fLightInfo_PL_Range		= 10.0f;
		}
	}

	// NavigationMesh ModeChange
	else if (TabMap.m_EditCheck_NavigationMesh.GetCheck())
	{
		TabMap.m_bIsNaviCreateMode = !TabMap.m_bIsNaviCreateMode;
		TabMap.m_bIsNaviModifyMode = !TabMap.m_bIsNaviModifyMode;

		// TabMap.
		TabMap.m_NaviMeshRadio_CreateMode.SetCheck(TabMap.m_bIsNaviCreateMode);
		TabMap.m_NaviMeshRadio_ModifyMode.SetCheck(TabMap.m_bIsNaviModifyMode);

		if (TabMap.m_bIsNaviCreateMode)
		{
			TabMap.m_NaviMeshCheck_FindNearPoint.EnableWindow(TRUE);
			TabMap.m_NaviMeshCheck_FindNearPoint.SetCheck(false);

			TabMap.m_fNaviMeshPointA_X = 0.0f;
			TabMap.m_fNaviMeshPointA_Y = 0.0f;
			TabMap.m_fNaviMeshPointA_Z = 0.0f;
			TabMap.m_fNaviMeshPointB_X = 0.0f;
			TabMap.m_fNaviMeshPointB_Y = 0.0f;
			TabMap.m_fNaviMeshPointB_Z = 0.0f;
			TabMap.m_fNaviMeshPointC_X = 0.0f;
			TabMap.m_fNaviMeshPointC_Y = 0.0f;
			TabMap.m_fNaviMeshPointC_Z = 0.0f;
		}
		else
		{
			TabMap.m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
			TabMap.m_NaviMeshCheck_FindNearPoint.SetCheck(false);
		}

		// 모든 Cell과 Collider Reset.
		Engine::OBJLIST* pCellList = Engine::CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_Environment", L"Cell");
		if (nullptr != pCellList || !pCellList->empty())
		{
			// Cell 색상 Green & WireFrame으로 변경.
			for (auto& pCell : *pCellList)
				static_cast<CToolCell*>(pCell)->Reset_CellAndCollider();
		}
	}

	TabMap.UpdateData(FALSE);
}
#pragma endregion




#pragma region TABUI_TEXSPRITEUV
void CToolSceneStage::KeyInput_TabUITexSpriteUV(CTabTexSpriteUV& TabUITexSprite)
{
	Engine::OBJLIST* m_plstGridRect = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", L"UIGridRect");
	if (nullptr == m_plstGridRect || m_plstGridRect->empty())
		return;

	TabUITexSprite.UpdateData(TRUE);

	CMainFrame* pMainFrame	= static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView*	pToolView	= static_cast<CToolView*>(pMainFrame->m_MainSplit.GetPane(0, 1));

	::POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	for (auto& pGridRect : *m_plstGridRect)
		static_cast<CToolGridRect*>(pGridRect)->m_bIsSelect = false;

	auto iter_begin = m_plstGridRect->begin();
	for (_int i = 0; i < TabUITexSprite.m_iRectSizeY; ++i)
	{
		for (_int j = 0; j < TabUITexSprite.m_iRectSizeX; ++j, ++iter_begin)
		{
			if (PtInRect(&(static_cast<CToolGridRect*>(*iter_begin)->m_tRect), ptMouse))
			{
				static_cast<CToolGridRect*>(*iter_begin)->m_bIsSelect = true;

				TabUITexSprite.m_fCurFrame = (_float)j;
				TabUITexSprite.m_fCurScene = (_float)i;

				TabUITexSprite.UpdateData(FALSE);
				return;
			}
		}
	}

	TabUITexSprite.UpdateData(FALSE);
}
#pragma endregion




#pragma region TABUI_2DUI
void CToolSceneStage::KeyInput_TabUI2DUI(CTab2DUI& TabUI2DUI)
{
	TabUI2DUI.UpdateData(TRUE);

	// UI 생성
	if (TabUI2DUI.m_bIsRootCreateMode)
	{
		if (TabUI2DUI.m_wstrRootDataFileName == L"" ||
			TabUI2DUI.m_wstrRootObjectTag == L"")
			return;

		// ListBox에 동일한 ObjTag가 있는지 검사.
		for (_int i = 0; i < TabUI2DUI.m_ListBoxRootUI.GetCount(); ++i)
		{
			CString wstrRootUITag = L"";
			TabUI2DUI.m_ListBoxRootUI.GetText(i, wstrRootUITag);

			if (wstrRootUITag == TabUI2DUI.m_wstrRootObjectTag)
			{

				AfxMessageBox(L"동일한 RootUI ObjectTag값 존재");
				return;
			}
		}

		Engine::CGameObject* pGameObj = nullptr;

		wstring wstrDataFullPath;
		_tchar szPath[MAX_STR] = L"";
		GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
		PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
		PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
		lstrcat(szPath, L"\\Bin\\ToolData\\");
		wstrDataFullPath = wstring(szPath) + wstring(TabUI2DUI.m_wstrRootDataFileName);
		wstrDataFullPath = CToolFileInfo::ConvertRelativePath(wstrDataFullPath.c_str());

		// _vec3 vPos = _vec3((_float)CMouseMgr::Get_CursorPoint().x, (_float)CMouseMgr::Get_CursorPoint().y, 1.0f);
		TabUI2DUI.m_fRootPosX = (_float)CMouseMgr::Get_CursorPoint().x;
		TabUI2DUI.m_fRootPosY = (_float)CMouseMgr::Get_CursorPoint().y;

		// UI 생성.
		pGameObj = CToolUIRoot::Create(m_pGraphicDevice, m_pCommandList,
									   wstring(TabUI2DUI.m_wstrRootObjectTag),
									   wstrDataFullPath,
									   _vec3(TabUI2DUI.m_fRootPosX, TabUI2DUI.m_fRootPosY, 0.0f),
									   _vec3(TabUI2DUI.m_fRootScaleX, TabUI2DUI.m_fRootScaleY, 1.0f),
									   TabUI2DUI.m_bIsRootAnimation,
									   TabUI2DUI.m_fRootFrameSpeed,
									   _vec3(TabUI2DUI.m_fRootRectPosOffsetX, TabUI2DUI.m_fRootRectPosOffsetY, 0.0f),
									   _vec3(TabUI2DUI.m_fRootRectScaleX, TabUI2DUI.m_fRootRectScaleY, 1.0f),
									   TabUI2DUI.m_RootUIDepth);
		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstring(TabUI2DUI.m_wstrRootObjectTag), pGameObj);

		// ListBox 추가.
		TabUI2DUI.m_ListBoxRootUI.AddString(TabUI2DUI.m_wstrRootObjectTag);
	}

	else if (TabUI2DUI.m_bIsRootModifyMode)
	{
		_int iListBoxSize = TabUI2DUI.m_ListBoxRootUI.GetCount();

		// Rect Render -> false
		m_pPickingRootUI = nullptr;

		for (_int i = 0; i < iListBoxSize; ++i)
		{
			CString wstrRootUITag = L"";
			TabUI2DUI.m_ListBoxRootUI.GetText(i, wstrRootUITag);
			Engine::OBJLIST* pRootUIList = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", wstring(wstrRootUITag));

			for (auto& pRootUI : *pRootUIList)
			{
				static_cast<CToolUIRoot*>(pRootUI)->m_bIsRenderRect = false;

				for(auto& pChildUI : static_cast<CToolUIRoot*>(pRootUI)->m_vecUIChild)
					static_cast<CToolUIChild*>(pChildUI)->m_bIsRenderRect = false;
			}
		}

		// Picking 수행.
		for (_int i = 0; i < iListBoxSize; ++i)
		{
			CString wstrRootUITag = L"";
			TabUI2DUI.m_ListBoxRootUI.GetText(i, wstrRootUITag);
			Engine::OBJLIST* pRootUIList = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", wstring(wstrRootUITag));

			for (auto& pRootUI : *pRootUIList)
			{
				if (PtInRect(&(static_cast<CToolUIRoot*>(pRootUI)->m_tRect), CMouseMgr::Get_CursorPoint()))
				{
					static_cast<CToolUIRoot*>(pRootUI)->m_bIsRenderRect = true;
					m_pPickingRootUI = static_cast<CToolUIRoot*>(pRootUI);

					// ChildUI ListBox 설정.
					TabUI2DUI.m_iChildUISelectIdx = -1;
					TabUI2DUI.m_pChildUISelected = nullptr;
					TabUI2DUI.m_ListBoxChildUI.ResetContent();

					for (auto& pChildUI : m_pPickingRootUI->m_vecUIChild)
						TabUI2DUI.m_ListBoxChildUI.AddString(static_cast<CToolUIChild*>(pChildUI)->m_wstrObjectTag.c_str());


					TabUI2DUI.m_wstrRootUITag     = static_cast<CToolUIRoot*>(pRootUI)->m_wstrObjectTag.c_str();
					TabUI2DUI.m_wstrRootObjectTag = static_cast<CToolUIRoot*>(pRootUI)->m_wstrObjectTag.c_str();
					TabUI2DUI.m_fRootPosX         = pRootUI->Get_Transform()->m_vPos.x;
					TabUI2DUI.m_fRootPosY         = pRootUI->Get_Transform()->m_vPos.y;
					TabUI2DUI.m_fRootScaleX       = pRootUI->Get_Transform()->m_vScale.x;
					TabUI2DUI.m_fRootScaleY       = pRootUI->Get_Transform()->m_vScale.y;
					TabUI2DUI.m_RootUIDepth		  = pRootUI->Get_UIDepth();

					TabUI2DUI.m_fRootFrameSpeed   = static_cast<CToolUIRoot*>(pRootUI)->m_tFrame.fFrameSpeed;
					if (TabUI2DUI.m_fRootFrameSpeed > 0.0f)
					{
						TabUI2DUI.m_bIsRootAnimation = true;
						TabUI2DUI.m_CheckRootIsAnimation.SetCheck(TRUE);
						TabUI2DUI.m_EditRootFrameSpeed.EnableWindow(TRUE);
					}
					else
					{
						TabUI2DUI.m_bIsRootAnimation = false;
						TabUI2DUI.m_CheckRootIsAnimation.SetCheck(FALSE);
						TabUI2DUI.m_EditRootFrameSpeed.EnableWindow(FALSE);
					}
					TabUI2DUI.m_fRootRectPosOffsetX = static_cast<CToolUIRoot*>(pRootUI)->m_vRectOffset.x;
					TabUI2DUI.m_fRootRectPosOffsetY = static_cast<CToolUIRoot*>(pRootUI)->m_vRectOffset.y;
					TabUI2DUI.m_fRootRectScaleX     = static_cast<CToolUIRoot*>(pRootUI)->m_pTransColor->m_vScale.x;
					TabUI2DUI.m_fRootRectScaleY     = static_cast<CToolUIRoot*>(pRootUI)->m_pTransColor->m_vScale.y;

					break;
				}

			}
		}
	}

	TabUI2DUI.UpdateData(FALSE);
}

void CToolSceneStage::KeyInput_Tab2DUIModeChange(CTab2DUI& TabUI2DUI)
{
	TabUI2DUI.UpdateData(TRUE);

	// RootUI
	TabUI2DUI.m_bIsRootCreateMode = !TabUI2DUI.m_bIsRootCreateMode;
	TabUI2DUI.m_bIsRootModifyMode = !TabUI2DUI.m_bIsRootModifyMode;
	TabUI2DUI.m_RadioRootCreateMode.SetCheck(TabUI2DUI.m_bIsRootCreateMode);
	TabUI2DUI.m_RadioRootModifyMode.SetCheck(TabUI2DUI.m_bIsRootModifyMode);
	
	// ChildUI
	TabUI2DUI.m_bIsChildCreateMode = !TabUI2DUI.m_bIsChildCreateMode;
	TabUI2DUI.m_bIsChildModifyMode = !TabUI2DUI.m_bIsChildModifyMode;
	TabUI2DUI.m_RadioChildUICreateMode.SetCheck(TabUI2DUI.m_bIsChildCreateMode);
	TabUI2DUI.m_RadioChildUIModifyMode.SetCheck(TabUI2DUI.m_bIsChildModifyMode);

	if (TabUI2DUI.m_bIsRootCreateMode)
	{
		TabUI2DUI.m_EditRootUITag.EnableWindow(TRUE);
		TabUI2DUI.m_EditDataFileName.EnableWindow(TRUE);
		TabUI2DUI.m_EditObjectTag.EnableWindow(TRUE);
		TabUI2DUI.m_EditChildObjectTag.EnableWindow(TRUE);
	}
	else
	{
		TabUI2DUI.m_EditRootUITag.EnableWindow(FALSE);
		TabUI2DUI.m_EditDataFileName.EnableWindow(FALSE);
		TabUI2DUI.m_EditObjectTag.EnableWindow(FALSE);
		TabUI2DUI.m_EditChildObjectTag.EnableWindow(FALSE);
	}


	TabUI2DUI.UpdateData(FALSE);
}

#pragma endregion



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

	for (auto& pCollider : m_pPickingCollider)
		Engine::Safe_Release(pCollider);

	Engine::CShaderColorInstancing::Get_Instance()->Reset_Instance();
	Engine::CShaderColorInstancing::Get_Instance()->Reset_InstancingConstantBuffer();
}
