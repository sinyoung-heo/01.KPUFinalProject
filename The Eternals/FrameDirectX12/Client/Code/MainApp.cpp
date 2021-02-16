#include "stdafx.h"
#include "MainApp.h"

#include "DirectInput.h"
#include "DirectSound.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "Renderer.h"

#include "Scene_Logo.h"
#include "Scene_Menu.h"
#include "Scene_StageVelika.h"
#include "Font.h"


CMainApp::CMainApp()
	: m_pDeviceClass(Engine::CGraphicDevice::Get_Instance())
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
{
}


HRESULT CMainApp::Ready_MainApp()
{
	srand(unsigned int(time(nullptr)));

	Engine::FAILED_CHECK_RETURN(SetUp_DefaultSetting(Engine::WINMODE::MODE_WIN, WINCX, WINCY), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_Font(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_ComponentPrototype(), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ Renderer 초기화 ]
	- Ready Shader
	- Ready RenderTarget
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Ready_Renderer(m_pGraphicDevice, m_pCommandList), E_FAIL);

	Engine::FAILED_CHECK_RETURN(SetUp_LoadingResource(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_StartScene(Engine::SCENEID::SCENE_LOGO), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font 생성 ]
	____________________________________________________________________________________________________________*/
	// FPS
	m_pFont_FPS = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_FPS, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_FPS->Ready_GameObject(L"", _vec2(1520.f, 0.f), D2D1::ColorF::Cornsilk), E_FAIL);

	// PipelineState
	m_pFont_PipelineState = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_PipelineState, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_PipelineState->Ready_GameObject(L"", _vec2(1370.f, 32.f), D2D1::ColorF::Cornsilk), E_FAIL);

	// RenderList
	m_pFont_RenderList = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_RenderList, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_RenderList->Ready_GameObject(L"", _vec2(1370.f, 160.f), D2D1::ColorF::Cornsilk), E_FAIL);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pManagement, -1);

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	Engine::CDirectInput::Get_Instance()->SetUp_InputState();
	Key_Input();

	/*__________________________________________________________________________________________________________
	[ Update Management ]
	____________________________________________________________________________________________________________*/
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_pManagement->Update_Management(fTimeDelta);

	return 0;
}

_int CMainApp::LateUpdate_MainApp(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pManagement, -1);

	/*__________________________________________________________________________________________________________
	[ LateUpdate Management ]
	____________________________________________________________________________________________________________*/
	m_pManagement->LateUpdate_Management(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Debug Font ]
	____________________________________________________________________________________________________________*/
	if (m_pRenderer->Get_RenderOnOff(L"DebugFont"))
		Show_FontLog(fTimeDelta);

	return 0;
}

void CMainApp::Render_MainApp(const _float& fTimeDelta)
{
	if (nullptr == m_pDeviceClass || nullptr == m_pManagement)
		return;

	/*__________________________________________________________________________________________________________
	[ Render Management ]
	____________________________________________________________________________________________________________*/
	m_pManagement->Render_Management(fTimeDelta, Engine::RENDERID::MULTI_THREAD);
}

HRESULT CMainApp::SetUp_DefaultSetting(Engine::WINMODE eMode, const _uint& uiWidth, const _uint& uiHeight)
{
	Engine::NULL_CHECK_RETURN(m_pDeviceClass, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GraphicDevice 초기화 ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pDeviceClass->Ready_GraphicDevice(g_hWnd, g_hInst, eMode, uiWidth, uiHeight), E_FAIL);
	
	m_pGraphicDevice	= m_pDeviceClass->Get_GraphicDevice();
	Engine::NULL_CHECK_RETURN(m_pGraphicDevice, E_FAIL);

	m_pCommandList = m_pDeviceClass->Get_CommandList(Engine::CMDID::CMD_MAIN);
	Engine::NULL_CHECK_RETURN(m_pCommandList, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DirectInput 장치 초기화 ]
	____________________________________________________________________________________________________________*/
	if (FAILED(Engine::CDirectInput::Get_Instance()->Ready_InputDevice(g_hInst, g_hWnd)))
	{
		Engine::CDirectInput::Get_Instance()->Destroy_Instance();
		return E_FAIL;
	}

	/*__________________________________________________________________________________________________________
	[ DirectSound 초기화 ]
	____________________________________________________________________________________________________________*/
	//if (FAILED(Engine::CDirectSound::Get_Instance()->Ready_DirectSound(g_hWnd)))
	//{
	//	CDirectSound::Get_Instance()->Destroy_Instance();
	//	return E_FAIL;
	//}
	//Engine::CDirectSound::Get_Instance()->LoadDirectSoundFile(L"Logo_BGM");

	

	return S_OK;
}

HRESULT CMainApp::SetUp_Font()
{
	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	AddFontResourceEx(L"../../Bin/Font/BinggraeMelona.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/Maplestory Bold.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/Maplestory Light.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/netmarbleB.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/netmarbleL.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/netmarbleM.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/NEXONLv1GothicBold.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/NEXONLv1GothicLight.ttf", FR_PRIVATE, 0);
	AddFontResourceEx(L"../../Bin/Font/NEXONLv1GothicRegular.ttf", FR_PRIVATE, 0);

	/*__________________________________________________________________________________________________________
	[ Font Prototype ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject* pGameObject = nullptr;

	// NetmarbleLight - 24
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"netmarble Light",	// Font Type
												  20.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NetmarbleLight", pGameObject);

	// NetmarbleBold - 32
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"netmarble Bold",	// Font Type
												  32.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NetmarbleBold", pGameObject);

	// BinggraeMelona 12
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"빙그레 메로나체",		// Font Type
												  24.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_Loading", pGameObject);

	return S_OK;
}

HRESULT CMainApp::SetUp_ComponentPrototype()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = Engine::CRcCol::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"RcCol", Engine::ID_STATIC, pComponent), E_FAIL);

	// CubeCol
	pComponent = Engine::CCubeCol::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"CubeCol", Engine::ID_STATIC, pComponent), E_FAIL);

	// RcTex
	pComponent = Engine::CRcTex::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"RcTex", Engine::ID_STATIC, pComponent), E_FAIL);

	// CubeTex
	pComponent = Engine::CCubeTex::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"CubeTex", Engine::ID_STATIC, pComponent), E_FAIL);

	// TerrainTex
	pComponent = Engine::CTerrainTex::Create(m_pGraphicDevice, m_pCommandList, 256, 256, 1.f, 40.0f);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"TerrainTex", Engine::ID_STATIC, pComponent), E_FAIL);

	// ScreenTex
	pComponent = Engine::CScreenTex::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ScreenTex", Engine::ID_STATIC, pComponent), E_FAIL);

	// Collider Sphere
	pComponent = Engine::CColliderSphere::Create(m_pGraphicDevice, m_pCommandList, 10, 10);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ColliderSphere", Engine::ID_DYNAMIC, pComponent), E_FAIL);

	// Collider Box
	pComponent = Engine::CColliderBox::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"ColliderBox", Engine::ID_DYNAMIC, pComponent), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::SetUp_LoadingResource()
{
	Engine::CComponent* pComponent = nullptr;

	// Texture - BackBuffer
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"BackBuffer",
										  L"../../Bin/Resource/ResourceLoading/Texture/BackBuffer/%d.dds", 2);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"BackBuffer", Engine::ID_STATIC, pComponent), E_FAIL);

	// Texture - Logo
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"Logo",
										  L"../../Bin/Resource/ResourceLoading/Texture/Logo/%d.dds", 6);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Logo", Engine::ID_STATIC, pComponent), E_FAIL);


	// Texture - LoadingProgress
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"LoadingProgress",
										  L"../../Bin/Resource/ResourceLoading/Texture/LoadingProgress/%d.dds", 4);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"LoadingProgress", Engine::ID_STATIC, pComponent), E_FAIL);

	// Textue - Dissolve
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"Dissolve",
										  L"../../Bin/Resource/ResourceLoading/Texture/Dissolve/%d.dds", 1,
										  Engine::TEXTYPE::TEX_2D);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"ResourcePrototype_TextureDissolve", Engine::ID_STATIC, pComponent), E_FAIL);


	return S_OK;
}

HRESULT CMainApp::SetUp_StartScene(Engine::SCENEID eScebeID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	Engine::CScene* pScene = nullptr;

	switch (eScebeID)
	{
	case Engine::SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pGraphicDevice, m_pCommandList);
		break;
	case Engine::SCENE_MENU:
		pScene = CScene_Menu::Create(m_pGraphicDevice, m_pCommandList);
		break;
	case Engine::SCENE_STAGE:
		pScene = CScene_StageVelika::Create(m_pGraphicDevice, m_pCommandList);
	default:
		break;
	}

	Engine::NULL_CHECK_RETURN(pScene, E_FAIL);

	Engine::FAILED_CHECK_RETURN(m_pManagement->SetUp_CurrentScene(pScene), E_FAIL);

	return S_OK;
}

void CMainApp::Key_Input()
{
#ifdef SERVER
	if (!g_bIsActive) return;
#endif 

	/*__________________________________________________________________________________________________________
	[ Camera Mode ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_DOWN(DIK_TAB))
		g_bIsOnDebugCaemra = !g_bIsOnDebugCaemra;

	/*__________________________________________________________________________________________________________
	[ Render On/Off ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_DOWN(DIK_F1))
		m_pRenderer->Set_RenderOnOff(L"RenderTarget");

	if (Engine::KEY_DOWN(DIK_F2))
		m_pRenderer->Set_RenderOnOff(L"DebugFont");

	if (Engine::KEY_DOWN(DIK_F3))
		m_pRenderer->Set_RenderOnOff(L"Collider");

	if (Engine::KEY_DOWN(DIK_F4))
		m_pRenderer->Set_RenderOnOff(L"SectorGrid");

}

void CMainApp::Show_FontLog(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Text - FPS ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pFont_FPS)
	{
		m_pFont_FPS->Update_GameObject(fTimeDelta);

		m_fTime += fTimeDelta;
		++m_uiFPS;

		if (m_fTime >= 1.0f)
		{
			wsprintf(m_szFPS, L"FPS : %d", m_uiFPS);
			m_pFont_FPS->Set_Text(wstring(m_szFPS));

			m_fTime = 0.0f;
			m_uiFPS = 0;
		}
	}

	/*__________________________________________________________________________________________________________
	[ Text - PipelineState ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pRenderer)
	{
		m_uiCnt_ShaderFile = m_pRenderer->Get_CntShaderFile();
		m_uiCnt_PipelineState = m_pRenderer->Get_CntPipelineState();
		m_uiCnt_SetPipelineState = m_pRenderer->Get_CntSetPipelineState();
		m_pRenderer->Reset_SetPipelineStateCnt();
	}

	m_wstrPipeline = wstring(L"[ Shader Info ]\n") +
					 wstring(L"Num ShaderFile : %d\n") +
					 wstring(L"Num PipelineState : %d\n") +
					 wstring(L"Num SetPipelineState : %d");

	wsprintf(m_szPipeline, m_wstrPipeline.c_str(),
			 m_uiCnt_ShaderFile,
			 m_uiCnt_PipelineState,
			 m_uiCnt_SetPipelineState);

	if (nullptr != m_pFont_PipelineState)
	{
		m_pFont_PipelineState->Update_GameObject(fTimeDelta);
		m_pFont_PipelineState->Set_Text(wstring(m_szPipeline));
	}

	/*__________________________________________________________________________________________________________
	[ Text RenderList Object Cnt ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pRenderer)
	{
		m_iPrioritySize		= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_PRIORITY);
		m_iNoneAlphaSize	= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_NONALPHA);
		m_iAlphaSize		= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_ALPHA);
		m_iUISize			= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_UI);
		m_iColliderSize		= m_pRenderer->Get_ColliderLstSize();
		m_iFontSize			= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_FONT);
	}
	m_wstrRenderList = wstring(L"[ Render Group ]\n") +
					   wstring(L"PRIORITY\t%d\n") +
					   wstring(L"NONEALPHA\t%d\n") +
					   wstring(L"ALPHA\t\t%d\n") +
					   wstring(L"UI\t\t%d\n") +
					   wstring(L"COLLIDER\t%d\n") +
					   wstring(L"FONT\t\t%d\n") +
					   wstring(L"------------------   %d");

	wsprintf(m_szRenderList, m_wstrRenderList.c_str(),
			 m_iPrioritySize,
			 m_iNoneAlphaSize,
			 m_iAlphaSize,
			 m_iUISize,
			 m_iColliderSize,
			 m_iFontSize,
			 m_iPrioritySize + m_iNoneAlphaSize + m_iAlphaSize + m_iUISize + m_iColliderSize + m_iFontSize);

	if (nullptr != m_pFont_RenderList)
	{
		m_pFont_RenderList->Update_GameObject(fTimeDelta);
		m_pFont_RenderList->Set_Text(wstring(m_szRenderList));
	}
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		delete pInstance;
		pInstance = nullptr;
	}

	return pInstance;
}

void CMainApp::Free()
{
	RemoveFontResourceEx(L"../../Bin/Font/BinggraeMelona.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/Maplestory Bold.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/Maplestory Light.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/netmarbleB.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/netmarbleL.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/netmarbleM.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/NEXONLv1GothicBold.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/NEXONLv1GothicLight.ttf", FR_PRIVATE, 0);
	RemoveFontResourceEx(L"../../Bin/Font/NEXONLv1GothicRegular.ttf", FR_PRIVATE, 0);
	
	Engine::Safe_Release(m_pFont_FPS);
	Engine::Safe_Release(m_pFont_PipelineState);
	Engine::Safe_Release(m_pFont_RenderList);
}
