#include "stdafx.h"
#include "Scene_Logo.h"
#include "Renderer.h"
#include "DirectInput.h"
#include "Management.h"
#include "Scene_Menu.h"
#include "Scene_MainStage.h"
#include "StageLDH.h"
#include "StagePJO.h"
#include "StageHSY.h"
#include "LogoBack.h"
#include "LoadingProgressBack.h"
#include "LoadingProgress.h"
#include "Font.h"
#include "FadeInOut.h"

CScene_Logo::CScene_Logo(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}


HRESULT CScene_Logo::Ready_Scene()
{
#ifdef CLIENT_LOG
	COUT_STR("");
	COUT_STR("<< Ready Scene_Logo >>");
	COUT_STR("");
#endif

	Engine::FAILED_CHECK_RETURN(Ready_LayerEnvironment(L"Layer_Environment"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerCamera(L"Layer_Camera"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerGameObject(L"Layer_GameObject"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerUI(L"Layer_UI"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerFont(L"Layer_Font"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_MaxLoadingCount(), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Loading Thread 持失 ]
	____________________________________________________________________________________________________________*/
	m_pLoading = CLoading::Create(m_pGraphicDevice, m_pCommandList, CLoading::LOADING_STAGE);
	Engine::NULL_CHECK_RETURN(m_pLoading, E_FAIL);


	/*__________________________________________________________________________________________________________
	[ Loading Font ]
	____________________________________________________________________________________________________________*/
	m_pFont_LoadingStr = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_Loading"));
	Engine::NULL_CHECK_RETURN(m_pFont_LoadingStr, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_LoadingStr->Ready_GameObject(L"", 
																	 _vec2(WINCX / 2.0f - 275.0f, WINCY - 100.0f),
																	 D2D1::ColorF::RosyBrown), E_FAIL);

	return S_OK;
}

_int CScene_Logo::Update_Scene(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Loading Text ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pFont_LoadingStr && !m_bIsCreateFadeInOut)
	{
		m_pFont_LoadingStr->Update_GameObject(fTimeDelta);

		_int	iCurLoadingCnt = m_pLoading->Get_CurLoadingCnt();
		_float	fRatio = (_float)iCurLoadingCnt / (_float)m_iMaxFileCount;
		_int	iPercent = (_int)(fRatio * 100.0f);

		lstrcpy(m_szLoadingStr, (m_pLoading->Get_LoadingString()));
		_tchar szPercent[MIN_STR] = L"";
		wsprintf(szPercent, L"Now Loading %d %% ", iPercent);
		wstring wstrResult = szPercent;
		wstring wstrLoading = m_szLoadingStr;
		wstrResult += wstrLoading;

		m_pFont_LoadingStr->Set_Text(wstrResult);
		static_cast<CLoadingProgress*>(m_pLoadingProgress)->Set_LoadingPercent(fRatio);
	}

	/*__________________________________________________________________________________________________________
	[ MouseCursorMgr ]
	____________________________________________________________________________________________________________*/
	if (m_pLoading->Get_Finish() && !m_bIsReadyMouseCursorMgr)
	{
		m_bIsReadyMouseCursorMgr = true;
		CMouseCursorMgr::Get_Instance()->Ready_MouseCursorMgr();
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CScene_Logo::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CScene_Logo::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	// Scene 穿発.
	if (m_pLoading->Get_Finish())
	{
		if (Engine::KEY_DOWN(DIK_RETURN))
		{
			if (!m_bIsCreateFadeInOut)
			{
				m_bIsCreateFadeInOut = true;
				m_pFont_LoadingStr->Set_DeadGameObject();
				Engine::CGameObject* pGameObj = nullptr;
				pGameObj = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CAHNGE_LOGO_STAGE);
				Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
				Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_UI", L"FadeInOut", pGameObj), E_FAIL);
			}
		}

		if (m_bIsSceneChange)
		{
			m_pObjectMgr->Clear_Layer();
			CMouseCursorMgr::Get_Instance()->Reset_MouseCursor();
			Engine::CScene* pNewScene = nullptr;
			pNewScene = CScene_MainStage::Create(m_pGraphicDevice, m_pCommandList);
			Engine::CManagement::Get_Instance()->SetUp_CurrentScene(pNewScene);
		}



		if (Engine::KEY_DOWN(DIK_1))
		{
			m_pObjectMgr->Clear_Layer();
			CMouseCursorMgr::Get_Instance()->Reset_MouseCursor();

			Engine::CScene* pNewScene = nullptr;
			pNewScene = CStageLDH::Create(m_pGraphicDevice, m_pCommandList);
			Engine::CManagement::Get_Instance()->SetUp_CurrentScene(pNewScene);
		}
		if (Engine::KEY_DOWN(DIK_2))
		{
			m_pObjectMgr->Clear_Layer();
			CMouseCursorMgr::Get_Instance()->Reset_MouseCursor();

			Engine::CScene* pNewScene = nullptr;
			pNewScene = CStagePJO::Create(m_pGraphicDevice, m_pCommandList);
			Engine::CManagement::Get_Instance()->SetUp_CurrentScene(pNewScene);
		}
		if (Engine::KEY_DOWN(DIK_3))
		{
			m_pObjectMgr->Clear_Layer();
			CMouseCursorMgr::Get_Instance()->Reset_MouseCursor();

			Engine::CScene* pNewScene = nullptr;
			pNewScene = CStageHSY::Create(m_pGraphicDevice, m_pCommandList);
			Engine::CManagement::Get_Instance()->SetUp_CurrentScene(pNewScene);
		}
	}


	return S_OK;
}


HRESULT CScene_Logo::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	/*__________________________________________________________________________________________________________
	[ GameObject 持失 ]
	m_pObjectMgr->Add_GameObject(wstrLayerTag, wstrObjTag);
	____________________________________________________________________________________________________________*/


	return S_OK;
}

HRESULT CScene_Logo::Ready_LayerCamera(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Camera Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CScene_Logo::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	/*__________________________________________________________________________________________________________
	[ GameObject 持失 ]
	m_pObjectMgr->Add_GameObject(wstrLayerTag, wstrObjTag);
	____________________________________________________________________________________________________________*/

	return S_OK;
}

HRESULT CScene_Logo::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ GameObject 持失 ]Logo   LoadingProgress
	____________________________________________________________________________________________________________*/
	pGameObj = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::FADE_IN);
	Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"FadeInOut", pGameObj), E_FAIL);
	
	pGameObj = CLogoBack::Create(m_pGraphicDevice, m_pCommandList, L"Logo");
	Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"LogoBack", pGameObj), E_FAIL);

	pGameObj = CLoadingProgressBack::Create(m_pGraphicDevice, m_pCommandList, L"LoadingProgress");
	Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"LoadingProgressBack", pGameObj), E_FAIL);

	m_pLoadingProgress = CLoadingProgress::Create(m_pGraphicDevice, m_pCommandList, L"LoadingProgress");
	Engine::NULL_CHECK_RETURN(m_pLoadingProgress, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"LoadingProgressBack", m_pLoadingProgress), E_FAIL);

	return S_OK;
}

HRESULT CScene_Logo::Ready_LayerFont(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);



	return S_OK;
}

HRESULT CScene_Logo::SetUp_MaxLoadingCount()
{
	_tchar	szLineCnt[MIN_STR] = L"";
	_int	iCnt = 0;

	wifstream fin { L"../../Bin/ToolData/PathFind_Mesh_Count.txt" };
	if (fin.fail())
		return E_FAIL;

	while (true)
	{
		fin.getline(szLineCnt, MIN_STR);

		if (fin.eof())
			break;

		int iCnt = _ttoi(szLineCnt);
		m_iMaxFileCount += iCnt;
	}

	fin.close();


	fin.open(L"../../Bin/ToolData/PathFind_Texture_Count.txt");
	if (fin.fail())
		return E_FAIL;

	while (true)
	{
		fin.getline(szLineCnt, MIN_STR);

		if (fin.eof())
			break;

		int iCnt = _ttoi(szLineCnt);
		m_iMaxFileCount += (_float)iCnt;
	}

	fin.close();

	return S_OK;
}

CScene_Logo * CScene_Logo::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CScene_Logo::Free()
{
	Engine::CScene::Free();

	if (nullptr != m_pFont_LoadingStr)
		Engine::Safe_Release(m_pFont_LoadingStr);

	if (nullptr != m_pLoading)
		Engine::Safe_Release(m_pLoading);

	Engine::CRenderer::Get_Instance()->Set_bIsLoadingFinish();

	// Release UploadBuffer
	Engine::CComponentMgr::Get_Instance()->Release_UploadBuffer();
}
