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
#include "PCSelectBackground.h"
#include "PCSelectJob.h"
#include "PCSelectFrame.h"
#include "PCSelectButton.h"
#include "LoginSelectButton.h"
#include "LoginIDBackground.h"
#include "LoginIDInputString.h"
#include "LoginIDFont.h"
#include "LoginIDButton.h"

char g_cJob = -1;

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
	m_pFont_LoadingStr = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_BinggraeMelona24"));
	Engine::NULL_CHECK_RETURN(m_pFont_LoadingStr, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_LoadingStr->Ready_GameObject(L"", 
																	 _vec2(WINCX / 2.0f - 275.0f, WINCY - 90.0f),
																	 D2D1::ColorF::RosyBrown), E_FAIL);

	g_cJob = PC_GLADIATOR;
	m_pPCSelectFrameWarrior->Set_IsUpdate(true);

	m_pLoginSelectPCButtonClicked->Set_IsActive(true);
	m_pLoginSelectPCButtonClicked->Set_IsRender(true);
	m_pLoginSelectPCButtonClicked->Get_Font()->Set_Text(L"Login Character");

	m_pLoginSelectPCButton->Set_IsActive(false);
	m_pLoginSelectPCButton->Set_IsRender(false);
	m_pLoginSelectPCButton->Get_Font()->Set_Text(L"Login Character");

	m_pLoginSelectIDButtonClicked->Set_IsActive(false);
	m_pLoginSelectIDButtonClicked->Set_IsRender(false);
	m_pLoginSelectIDButtonClicked->Get_Font()->Set_Text(L"Login ID");

	m_pLoginSelectIDButton->Set_IsActive(true);
	m_pLoginSelectIDButton->Set_IsRender(true);
	m_pLoginSelectIDButton->Get_Font()->Set_Text(L"Login ID");



	m_pLoginIDBackground->Set_IsActive(false);
	m_pLoginIDInputStringID->Set_IsActive(false);
	m_pLoginIDInputStringPWD->Set_IsActive(false);
	m_pLoginIDFont_ID->Set_IsActive(false);
	m_pLoginIDFont_PWD->Set_IsActive(false);
	m_pLoginIDButton->Set_IsActive(false);

	return S_OK;
}

_int CScene_Logo::Update_Scene(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Loading Text ]
	____________________________________________________________________________________________________________*/
	if (g_bIsLoadingFinish && nullptr != m_pFont_LoadingStr)
		m_pFont_LoadingStr->Set_DeadGameObject();
	
	if (nullptr != m_pFont_LoadingStr && !m_bIsCreateFadeInOut && !g_bIsLoadingFinish)
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

		// Create FadeIn 
		Engine::CGameObject* pGameObj = nullptr;
		pGameObj = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::FADE_IN);
		Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_UI", L"FadeInOut", pGameObj), E_FAIL);
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CScene_Logo::LateUpdate_Scene(const _float & fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ PCSelect KeyInput ]
	____________________________________________________________________________________________________________*/
	if (g_bIsLoadingFinish)
	{
		KeyInput_LoginSelect(fTimeDelta);
		KeyInput_PCSelect(fTimeDelta);
	}

	return NO_EVENT;
}

HRESULT CScene_Logo::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	// Scene 穿発.
	if (m_pLoading->Get_Finish())
	{
		if (g_bIsGameStart)
		{
			if (!m_bIsCreateFadeInOut)
			{
				m_bIsCreateFadeInOut = true;

				if (nullptr != m_pFont_LoadingStr)
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


void CScene_Logo::KeyInput_LoginSelect(const _float& fTimeDelta)
{
	if (!CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse())
		return;
	if (g_bIsGameStart)
		return;

	// LoginSelect PC
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pLoginSelectPCButton->Get_Rect()) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_pLoginSelectPCButtonClicked->Set_IsActive(true);
		m_pLoginSelectPCButtonClicked->Set_IsRender(true);
		m_pLoginSelectPCButton->Set_IsActive(false);
		m_pLoginSelectPCButton->Set_IsRender(false);
		m_pLoginSelectIDButtonClicked->Set_IsActive(false);
		m_pLoginSelectIDButtonClicked->Set_IsRender(false);
		m_pLoginSelectIDButton->Set_IsActive(true);
		m_pLoginSelectIDButton->Set_IsRender(true);

		m_bIsLoginPC = true;
		m_bIsLoginID = false;

		// PCSelect Active On
		m_pPCSelectBackground->Set_IsActive(true);
		m_pPCSelectJobWarrior->Set_IsActive(true);
		m_pPCSelectJobArcher->Set_IsActive(true);
		m_pPCSelectJobPriest->Set_IsActive(true);
		m_pPCSelectFrameWarrior->Set_IsActive(true);
		m_pPCSelectFrameArcher->Set_IsActive(true);
		m_pPCSelectFramePriest->Set_IsActive(true);
		m_pPCSelectButton->Set_IsActive(true);
		m_pPCSelectButtonClicked->Set_IsActive(true);

		// LoginID Active Off
		m_pLoginIDBackground->Set_IsActive(false);
		m_pLoginIDInputStringID->Set_IsActive(false);
		m_pLoginIDInputStringPWD->Set_IsActive(false);
		m_pLoginIDFont_ID->Set_IsActive(false);
		m_pLoginIDFont_PWD->Set_IsActive(false);
		m_pLoginIDButton->Set_IsActive(false);
	}

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pLoginSelectIDButton->Get_Rect()) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_pLoginSelectPCButtonClicked->Set_IsActive(false);
		m_pLoginSelectPCButtonClicked->Set_IsRender(false);
		m_pLoginSelectPCButton->Set_IsActive(true);
		m_pLoginSelectPCButton->Set_IsRender(true);
		m_pLoginSelectIDButtonClicked->Set_IsActive(true);
		m_pLoginSelectIDButtonClicked->Set_IsRender(true);
		m_pLoginSelectIDButton->Set_IsActive(false);
		m_pLoginSelectIDButton->Set_IsRender(false);

		m_bIsLoginPC = false;
		m_bIsLoginID = true;

		// PCSelect Active Off
		m_pPCSelectBackground->Set_IsActive(false);
		m_pPCSelectJobWarrior->Set_IsActive(false);
		m_pPCSelectJobArcher->Set_IsActive(false);
		m_pPCSelectJobPriest->Set_IsActive(false);
		m_pPCSelectFrameWarrior->Set_IsActive(false);
		m_pPCSelectFrameArcher->Set_IsActive(false);
		m_pPCSelectFramePriest->Set_IsActive(false);
		m_pPCSelectButton->Set_IsActive(false);
		m_pPCSelectButtonClicked->Set_IsActive(false);

		// LoginID Active On
		m_pLoginIDBackground->Set_IsActive(true);
		m_pLoginIDInputStringID->Set_IsActive(true);
		m_pLoginIDInputStringPWD->Set_IsActive(true);
		m_pLoginIDFont_ID->Set_IsActive(true);
		m_pLoginIDFont_PWD->Set_IsActive(true);
		m_pLoginIDButton->Set_IsActive(true);
	}
}

void CScene_Logo::KeyInput_PCSelect(const _float& fTimeDelta)
{
	if (!CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse())
		return;
	if (g_bIsGameStart)
		return;
	if (m_bIsLoginID)
		return;

	// PCSelectJobWarrior
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pPCSelectJobWarrior->Get_Rect()) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_pPCSelectFrameWarrior->Set_IsUpdate(true);
		m_pPCSelectFrameArcher->Set_IsUpdate(false);
		m_pPCSelectFramePriest->Set_IsUpdate(false);

		g_cJob = PC_GLADIATOR;
	}

	// PCSelectJobArcher
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pPCSelectJobArcher->Get_Rect()) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_pPCSelectFrameWarrior->Set_IsUpdate(false);
		m_pPCSelectFrameArcher->Set_IsUpdate(true);
		m_pPCSelectFramePriest->Set_IsUpdate(false);

		g_cJob = PC_ARCHER;
	}

	// PCSelectJobPriest
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pPCSelectJobPriest->Get_Rect()) &&
		Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_pPCSelectFrameWarrior->Set_IsUpdate(false);
		m_pPCSelectFrameArcher->Set_IsUpdate(false);
		m_pPCSelectFramePriest->Set_IsUpdate(true);

		g_cJob = PC_PRIEST;
	}

	// PCSelectButton
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pPCSelectButton->Get_Rect()) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		g_bIsGameStart = true;
		m_pPCSelectButton->Set_IsRenderUI(true);
		m_pPCSelectButtonClicked->Set_IsRenderUI(false);
	}

	m_bIsKeyPressing = false;
	m_pPCSelectButton->Set_IsRenderUI(true);
	m_pPCSelectButtonClicked->Set_IsRenderUI(false);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_pPCSelectButton->Get_Rect()) &&
		Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
	{
		m_bIsKeyPressing = true;
		m_pPCSelectButton->Set_IsRenderUI(false);
		m_pPCSelectButtonClicked->Set_IsRenderUI(true);
	}
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
	[ GameObject 持失 ]
	____________________________________________________________________________________________________________*/
	// LoadingProgress
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

	/*__________________________________________________________________________________________________________
	[ UI - Login ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(SetUp_UIPCSelect(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_UILoginID(), E_FAIL);

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

HRESULT CScene_Logo::SetUp_UIPCSelect()
{
	{
		wifstream fin { L"../../Bin/ToolData/2DUIPCLogingCharacterClicked.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CLoginSelectButton::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pLoginSelectPCButtonClicked = static_cast<CLoginSelectButton*>(pRootUI);
		}
	}

	{
		wifstream fin { L"../../Bin/ToolData/2DUIPCLogingCharacterNormal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CLoginSelectButton::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pLoginSelectPCButton = static_cast<CLoginSelectButton*>(pRootUI);
		}
	}

	{
		wifstream fin { L"../../Bin/ToolData/2DUIPCLogingIDClicked.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CLoginSelectButton::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pLoginSelectIDButtonClicked = static_cast<CLoginSelectButton*>(pRootUI);
		}
	}

	{
		wifstream fin { L"../../Bin/ToolData/2DUIPCLogingIDNormal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CLoginSelectButton::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pLoginSelectIDButton = static_cast<CLoginSelectButton*>(pRootUI);
		}
	}

	/*__________________________________________________________________________________________________________
	[ UI - PCSelect ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUIPCSelect.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CPCSelectBackground::Create(m_pGraphicDevice, m_pCommandList,
												  wstrRootObjectTag,
												  wstrDataFilePath,
												  vPos,
												  vScale,
												  bIsSpriteAnimation,
												  fFrameSpeed,
												  vRectPosOffset,
												  vRectScale,
												  UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pPCSelectBackground = static_cast<CPCSelectBackground*>(pRootUI);

			// UIChild 持失.
			Engine::CGameObject* pChildUI = nullptr;
			for (_int i = 0; i < iChildUISize; ++i)
			{
				if (L"PCSelectWarrior" == vecObjectTag[i] || 
					L"PCSelectArcher" == vecObjectTag[i] || 
					L"PCSelectPriest" == vecObjectTag[i])
				{
					pChildUI = CPCSelectJob::Create(m_pGraphicDevice, m_pCommandList,
													wstrRootObjectTag,				// RootObjectTag
													vecObjectTag[i],				// ObjectTag
													vecDataFilePath[i],				// DataFilePath
													vecPos[i],						// Pos
													vecScale[i],					// Scane
													(_bool)vecIsSpriteAnimation[i],// Is Animation
													vecFrameSpeed[i],				// FrameSpeed
													vecRectPosOffset[i],			// RectPosOffset
													vecRectScale[i],				// RectScaleOffset
													vecUIDepth[i]);					// UI Depth

					if (L"PCSelectWarrior" == vecObjectTag[i])
						m_pPCSelectJobWarrior = static_cast<CPCSelectJob*>(pChildUI);
					else if (L"PCSelectArcher" == vecObjectTag[i])
						m_pPCSelectJobArcher = static_cast<CPCSelectJob*>(pChildUI);
					else if (L"PCSelectPriest" == vecObjectTag[i])
						m_pPCSelectJobPriest = static_cast<CPCSelectJob*>(pChildUI);
				}
				else if (L"PCSelectWarriorFrame" == vecObjectTag[i] ||
						 L"PCSelectArcherFrame" == vecObjectTag[i] || 
						 L"PCSelectPriestFrame" == vecObjectTag[i])
				{
					pChildUI = CPCSelectFrame::Create(m_pGraphicDevice, m_pCommandList,
													  wstrRootObjectTag,				// RootObjectTag
													  vecObjectTag[i],					// ObjectTag
													  vecDataFilePath[i],				// DataFilePath
													  vecPos[i],						// Pos
													  vecScale[i],						// Scane
													  (_bool)vecIsSpriteAnimation[i],	// Is Animation
													  vecFrameSpeed[i],					// FrameSpeed
													  vecRectPosOffset[i],				// RectPosOffset
													  vecRectScale[i],					// RectScaleOffset
													  vecUIDepth[i]);					// UI Depth

					if (L"PCSelectWarriorFrame" == vecObjectTag[i])
						m_pPCSelectFrameWarrior = static_cast<CPCSelectFrame*>(pChildUI);
					else if (L"PCSelectArcherFrame" == vecObjectTag[i])
						m_pPCSelectFrameArcher = static_cast<CPCSelectFrame*>(pChildUI);
					else if (L"PCSelectPriestFrame" == vecObjectTag[i])
						m_pPCSelectFramePriest = static_cast<CPCSelectFrame*>(pChildUI);
				}
				else if (L"PCSelectButtonNormal" == vecObjectTag[i] ||
						 L"PCSelectButtonCliecked" == vecObjectTag[i])
				{
					pChildUI = CPCSelectButton::Create(m_pGraphicDevice, m_pCommandList,
													   wstrRootObjectTag,				// RootObjectTag
													   vecObjectTag[i],					// ObjectTag
													   vecDataFilePath[i],				// DataFilePath
													   vecPos[i],						// Pos
													   vecScale[i],						// Scane
													   (_bool)vecIsSpriteAnimation[i],	// Is Animation
													   vecFrameSpeed[i],				// FrameSpeed
													   vecRectPosOffset[i],				// RectPosOffset
													   vecRectScale[i],					// RectScaleOffset
													   vecUIDepth[i]);					// UI Depth

					if (L"PCSelectButtonNormal" == vecObjectTag[i])
						m_pPCSelectButton = static_cast<CPCSelectButton*>(pChildUI);
					else if (L"PCSelectButtonCliecked" == vecObjectTag[i])
						m_pPCSelectButtonClicked = static_cast<CPCSelectButton*>(pChildUI);
				}
				m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
				static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
			}
		}
	}

	return S_OK;
}

HRESULT CScene_Logo::SetUp_UILoginID()
{
	{
		wifstream fin { L"../../Bin/ToolData/2DUIIDLogin.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
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
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 持失.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CLoginIDBackground::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
			m_pLoginIDBackground = static_cast<CLoginIDBackground*>(pRootUI);

			// UIChild 持失.
			Engine::CGameObject* pChildUI = nullptr;
			for (_int i = 0; i < iChildUISize; ++i)
			{
				if (L"InputStringID" == vecObjectTag[i] || L"InputStringPwd" == vecObjectTag[i])
				{
					pChildUI = CLoginIDInputString::Create(m_pGraphicDevice, m_pCommandList,
														   wstrRootObjectTag,				// RootObjectTag
														   vecObjectTag[i],					// ObjectTag
														   vecDataFilePath[i],				// DataFilePath
														   vecPos[i],						// Pos
														   vecScale[i],						// Scane
														   (_bool)vecIsSpriteAnimation[i],	// Is Animation
														   vecFrameSpeed[i],				// FrameSpeed
														   vecRectPosOffset[i],				// RectPosOffset
														   vecRectScale[i],					// RectScaleOffset
														   vecUIDepth[i]);					// UI Depth

					if (L"InputStringID" == vecObjectTag[i])
						m_pLoginIDInputStringID = static_cast<CLoginIDInputString*>(pChildUI);

					else if (L"InputStringPwd" == vecObjectTag[i])
						m_pLoginIDInputStringPWD = static_cast<CLoginIDInputString*>(pChildUI);
				}
				else if (L"FontID" == vecObjectTag[i] || L"FontPwd" == vecObjectTag[i])
				{
					pChildUI = CLoginIDFont::Create(m_pGraphicDevice, m_pCommandList,
													wstrRootObjectTag,					// RootObjectTag
													vecObjectTag[i],					// ObjectTag
													vecDataFilePath[i],					// DataFilePath
													vecPos[i],							// Pos
													vecScale[i],						// Scane
													(_bool)vecIsSpriteAnimation[i],		// Is Animation
													vecFrameSpeed[i],					// FrameSpeed
													vecRectPosOffset[i],				// RectPosOffset
													vecRectScale[i],					// RectScaleOffset
													vecUIDepth[i]);						// UI Depth

					if (L"FontID" == vecObjectTag[i])
						m_pLoginIDFont_ID = static_cast<CLoginIDFont*>(pChildUI);

					else if (L"FontPwd" == vecObjectTag[i])
						m_pLoginIDFont_PWD = static_cast<CLoginIDFont*>(pChildUI);
				}
				else if (L"LoginButton" == vecObjectTag[i])
				{
					pChildUI = CLoginIDButton::Create(m_pGraphicDevice, m_pCommandList,
													  wstrRootObjectTag,				// RootObjectTag
													  vecObjectTag[i],					// ObjectTag
													  vecDataFilePath[i],				// DataFilePath
													  vecPos[i],						// Pos
													  vecScale[i],						// Scane
													  (_bool)vecIsSpriteAnimation[i],	// Is Animation
													  vecFrameSpeed[i],					// FrameSpeed
													  vecRectPosOffset[i],				// RectPosOffset
													  vecRectScale[i],					// RectScaleOffset
													  vecUIDepth[i]);					// UI Depth

					m_pLoginIDButton = static_cast<CLoginIDButton*>(pChildUI);
				}

				m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
				static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
			}
		}
	}

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

	m_pLoginSelectPCButtonClicked = nullptr;
	m_pLoginSelectPCButton        = nullptr;
	m_pLoginSelectIDButtonClicked = nullptr;
	m_pLoginSelectIDButton        = nullptr;

	m_pPCSelectBackground     = nullptr;
	m_pPCSelectJobWarrior     = nullptr;
	m_pPCSelectJobArcher      = nullptr;
	m_pPCSelectJobPriest      = nullptr;
	m_pPCSelectFrameWarrior   = nullptr;
	m_pPCSelectFrameArcher    = nullptr;
	m_pPCSelectFramePriest    = nullptr;
	m_pPCSelectButton         = nullptr;
	m_pPCSelectButtonClicked  = nullptr;

	m_pLoginIDBackground     = nullptr;
	m_pLoginIDInputStringID  = nullptr;
	m_pLoginIDInputStringPWD = nullptr;
	m_pLoginIDFont_ID        = nullptr;
	m_pLoginIDFont_PWD       = nullptr;
	m_pLoginIDButton         = nullptr;
}
