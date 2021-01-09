#include "stdafx.h"
#include "ToolSceneLogo.h"

#include "Management.h"
#include "DirectInput.h"
#include "Font.h"
#include "ToolSceneStage.h"

CToolSceneLogo::CToolSceneLogo(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}

HRESULT CToolSceneLogo::Ready_Scene()
{
	Engine::FAILED_CHECK_RETURN(Ready_LayerEnvironment(L"Layer_Environment"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerCamera(L"Layer_Camera"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerGameObject(L"Layer_GameObject"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerUI(L"Layer_UI"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerFont(L"Layer_Font"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_MaxLoadingCount(), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ Loading Thread 持失 ]
	____________________________________________________________________________________________________________*/
	m_pLoading = CToolLoading::Create(m_pGraphicDevice, m_pCommandList, CToolLoading::LOADING_STAGE);
	Engine::NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Loading Font ]
	____________________________________________________________________________________________________________*/
	m_pFont_LoadingStr = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_NetmarbleBold"));
	Engine::NULL_CHECK_RETURN(m_pFont_LoadingStr, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_LoadingStr->Ready_GameObject(L"", _vec2(16.0f, 16.0f), D2D1::ColorF::SpringGreen), E_FAIL);

	m_pFont_Stage = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_Stage, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_Stage->Ready_GameObject(L"", _vec2(1390.f, 0.f), D2D1::ColorF::SpringGreen), E_FAIL);

	return S_OK;
}

_int CToolSceneLogo::Update_Scene(const _float& fTimeDelta)
{

	/*__________________________________________________________________________________________________________
	[ Stage Text ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pFont_Stage)
	{
		m_pFont_Stage->Update_GameObject(fTimeDelta);
		wsprintf(m_szFPS, L"LoadingScene FPS : %d", g_iFPS);
		m_pFont_Stage->Set_Text(wstring(m_szFPS));
	}

	/*__________________________________________________________________________________________________________
	[ Loading Text ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pFont_LoadingStr)
		m_pFont_LoadingStr->Update_GameObject(fTimeDelta);

	if (g_bIsLoadingStart)
	{
		_int	iCurLoadingCnt	= m_pLoading->Get_CurLoadingCnt();
		_float	fRatio			= (_float)iCurLoadingCnt / (_float)m_iMaxFileCount;
		_int	iPercent		= (_int)(fRatio * 100.0f);

		lstrcpy(m_szLoadingStr, (m_pLoading->Get_LoadingString()));
		_tchar szPercent[MIN_STR] = L"%d %% \n";
		wsprintf(szPercent, L"Now Loading %d %% Complete\n", iPercent);
		wstring wstrResult	= szPercent;
		wstring wstrLoading = m_szLoadingStr;
		wstrResult += wstrLoading;

		m_pFont_LoadingStr->Set_Text(wstrResult);
	}
	else
		m_pFont_LoadingStr->Set_Text(m_pLoading->Get_LoadingString());

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CToolSceneLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CToolSceneLogo::Render_Scene(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta), E_FAIL);

	if (/*Engine::KEY_DOWN(DIK_RETURN) &&*/ m_pLoading->Get_Finish())
	{
		m_pObjectMgr->Clear_Layer();

		Engine::CScene* pNewScene = CToolSceneStage::Create(m_pGraphicDevice, m_pCommandList);
		Engine::CManagement::Get_Instance()->SetUp_CurrentScene(pNewScene);
	}

	return S_OK;
}

HRESULT CToolSceneLogo::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	return S_OK;
}

HRESULT CToolSceneLogo::Ready_LayerCamera(wstring wstrLayerTag)
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

HRESULT CToolSceneLogo::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	return S_OK;
}

HRESULT CToolSceneLogo::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	return S_OK;
}

HRESULT CToolSceneLogo::Ready_LayerFont(wstring wstrLayerTag)
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

HRESULT CToolSceneLogo::SetUp_MaxLoadingCount()
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
		m_iMaxFileCount += iCnt;
	}

	fin.close();

	return S_OK;
}

CToolSceneLogo* CToolSceneLogo::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CToolSceneLogo* pInstance = new CToolSceneLogo(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolSceneLogo::Free()
{
	Engine::CScene::Free();

	if (nullptr != m_pFont_LoadingStr)
		Engine::Safe_Release(m_pFont_LoadingStr);

	if (nullptr != m_pFont_Stage)
		Engine::Safe_Release(m_pFont_Stage);

	if (nullptr != m_pLoading)
		Engine::Safe_Release(m_pLoading);

	Engine::CRenderer::Get_Instance()->Set_bIsLoadingFinish();
	Engine::CComponentMgr::Get_Instance()->Release_UploadBuffer();
}
