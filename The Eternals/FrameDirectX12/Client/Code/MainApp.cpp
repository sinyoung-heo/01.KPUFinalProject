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
#include "Scene_MainStage.h"
#include "Font.h"
#include "QuestMgr.h"
#include "SoundMgr.h"
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

	SetUp_SystemInfo();
	SetUp_WindowMouseSetting();
	Engine::FAILED_CHECK_RETURN(SetUp_DefaultSetting(Engine::WINMODE::MODE_FULL, WINCX, WINCY), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_Font(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_ComponentPrototype(), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ Renderer ÃÊ±âÈ­ ]
	- Ready Shader
	- Ready RenderTarget
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Ready_Renderer(m_pGraphicDevice, m_pCommandList), E_FAIL);

	Engine::FAILED_CHECK_RETURN(SetUp_LoadingResource(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(SetUp_StartScene(Engine::SCENEID::SCENE_LOGO), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font »ý¼º ]
	____________________________________________________________________________________________________________*/
	// FPS
	m_pFont_FPS = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_FPS, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_FPS->Ready_GameObject(L"", _vec2(1520.f, 0.f), D2D1::ColorF::Cornsilk), E_FAIL);

	// PipelineState
	m_pFont_PipelineState = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_PipelineState, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_PipelineState->Ready_GameObject(L"", _vec2(1370.f, 32.f), D2D1::ColorF::Lime), E_FAIL);

	// RenderList
	m_pFont_RenderList = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_RenderList, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_RenderList->Ready_GameObject(L"", _vec2(1370.f, 160.0f), D2D1::ColorF::Lime), E_FAIL);

	// System
	m_pFont_System = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont_System, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont_System->Ready_GameObject(L"", _vec2(25.0f, 150.0f), D2D1::ColorF::Lime), E_FAIL);


	return S_OK;
}

void CMainApp::Process_PacketFromServer()
{
	if (nullptr == m_pManagement)
		return;

	m_pManagement->Process_PacketFromServer();
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
	// UpdateMouseCursor
	CMouseCursorMgr::Get_Instance()->Update_MouseCursorMgr(fTimeDelta);

	m_pManagement->Update_Management(fTimeDelta);
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_pSoundMgr->Update_Sound();
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

	Show_FontFPS(fTimeDelta);

	return 0;
}

void CMainApp::Send_PacketToServer()
{
	if (nullptr == m_pManagement)
		return;

	m_pManagement->Send_PacketToServer();
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
	[ GraphicDevice ÃÊ±âÈ­ ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pDeviceClass->Ready_GraphicDevice(g_hWnd, g_hInst, eMode, uiWidth, uiHeight), E_FAIL);
	
	m_pGraphicDevice	= m_pDeviceClass->Get_GraphicDevice();
	Engine::NULL_CHECK_RETURN(m_pGraphicDevice, E_FAIL);

	m_pCommandList = m_pDeviceClass->Get_CommandList(Engine::CMDID::CMD_MAIN);
	Engine::NULL_CHECK_RETURN(m_pCommandList, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DirectInput ÀåÄ¡ ÃÊ±âÈ­ ]
	____________________________________________________________________________________________________________*/
	if (FAILED(Engine::CDirectInput::Get_Instance()->Ready_InputDevice(g_hInst, g_hWnd)))
	{
		Engine::CDirectInput::Get_Instance()->Destroy_Instance();
		return E_FAIL;
	}

	/*__________________________________________________________________________________________________________
	[ DirectSound ÃÊ±âÈ­ ]
	____________________________________________________________________________________________________________*/
	//if (FAILED(Engine::CDirectSound::Get_Instance()->Ready_DirectSound(g_hWnd)))
	//{
	//	CDirectSound::Get_Instance()->Destroy_Instance();
	//	return E_FAIL;
	//}
	//Engine::CDirectSound::Get_Instance()->LoadDirectSoundFile(L"Logo_BGM");


	//EffectMgr »ý¼º
	CEffectMgr::Get_Instance();

	m_pSoundMgr = Engine::CSoundMgr::Get_Instance();
	Engine::FAILED_CHECK_RETURN(m_pSoundMgr->Ready_SoundMgr(), E_FAIL);
	m_pSoundMgr->Load_Sound("../../Bin/Sound/BGM/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/A_Skill/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/G_Skill/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/P_Skill/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/Boss/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/Monster/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/Cinematic/*.*");
	m_pSoundMgr->Load_Sound("../../Bin/Sound/NPC/*.*");

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

	// NetmarbleBold - 48
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"netmarble Bold",	// Font Type
												  48.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NetmarbleBold48", pGameObject);

	// NetmarbleBold - 60
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"netmarble Bold",	// Font Type
												  60.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NetmarbleBold60", pGameObject);

	// NetmarbleBold - 72
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"netmarble Bold",	// Font Type
												  72.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NetmarbleBold72", pGameObject);


	// ³Ø½¼Lv1°íµñ Bold - 24
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"³Ø½¼Lv1°íµñ Bold",	// Font Type
												  24.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NexonBold24", pGameObject);

	// ³Ø½¼Lv1°íµñ Bold - 32
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"³Ø½¼Lv1°íµñ Bold",	// Font Type
												  32.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NexonBold32", pGameObject);

	// ³Ø½¼Lv1°íµñ Bold - 72
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"³Ø½¼Lv1°íµñ Bold",	// Font Type
												  72.f,					// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_NexonBold72", pGameObject);



	// BinggraeMelona 5
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  5.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona5", pGameObject);

	// BinggraeMelona 11
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  11.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona11", pGameObject);

	// BinggraeMelona 16
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  16.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona16", pGameObject);

	// BinggraeMelona 20
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  20.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona20", pGameObject);

	// BinggraeMelona 24
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  24.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona24", pGameObject);

	// BinggraeMelona 32
	pGameObject = Engine::CFont::Create_Prototype(m_pGraphicDevice, m_pCommandList,
												  L"ºù±×·¹ ¸Þ·Î³ªÃ¼",		// Font Type
												  32.0f,				// Font Size
												  D2D1::ColorF::White);	// Font Color
	Engine::NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pObjectMgr->Add_GameObjectPrototype(L"Font_BinggraeMelona32", pGameObject);

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

	// TrailTex
	pComponent = Engine::CTrailTex::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"TrailTex", Engine::ID_STATIC, pComponent), E_FAIL);


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
		pScene = CScene_MainStage::Create(m_pGraphicDevice, m_pCommandList);
	default:
		break;
	}

	Engine::NULL_CHECK_RETURN(pScene, E_FAIL);

	Engine::FAILED_CHECK_RETURN(m_pManagement->SetUp_CurrentScene(pScene), E_FAIL);

	return S_OK;
}

void CMainApp::SetUp_WindowMouseSetting()
{
	//ShowCursor(false);

	RECT rc, rc2;
	POINT p1, p2;

	GetClientRect(g_hWnd, &rc);

	p1.x = rc.left;
	p1.y = rc.top;
	p2.x = rc.right;
	p2.y = rc.bottom;

	ClientToScreen(g_hWnd, &p1);
	ClientToScreen(g_hWnd, &p2);

	rc2.left   = p1.x;
	rc2.top    = p1.y;
	rc2.right  = p2.x;
	rc2.bottom = p2.y;

	ClipCursor(&rc2);
}

void CMainApp::SetUp_SystemInfo()
{
	//·¹Áö½ºÆ®¸®¸¦ Á¶»çÇÏ¿© ÇÁ·Î¼¼¼­ÀÇ ¸ðµ¨¸íÀ» ¾ò¾î³À´Ï´Ù.
	wchar_t Cpu_info[100];
	HKEY hKey;
	int i = 0;
	long result = 0;
	DWORD c_size = sizeof(Cpu_info);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);
	RegQueryValueEx(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)Cpu_info, &c_size);
	RegCloseKey(hKey);
	m_wstrCPUName = Cpu_info;

	// CPU's Thread
	SYSTEM_INFO tSystemInfo;
	ZeroMemory(&tSystemInfo, sizeof(SYSTEM_INFO));
	GetNativeSystemInfo(&tSystemInfo);
	m_iCPUThreads = tSystemInfo.dwNumberOfProcessors;

	// CPU's Core
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pPI = NULL;
	DWORD length = 0;

	BOOL _result = GetLogicalProcessorInformation(pPI, &length);
	if (!_result)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			pPI = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) new BYTE[length];
	}

	_result = GetLogicalProcessorInformation(pPI, &length);

	for (int i = 0; i < length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i)
	{
		if (pPI[i].Relationship == RelationProcessorCore)
			++m_iCPUCores;
	}

	delete[] pPI;
	pPI = nullptr;
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
	[ MouseCursor ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_DOWN(DIK_LCONTROL) && !g_bIsOnDebugCaemra)
		CMouseCursorMgr::Get_Instance()->Is_ActiveMouse();

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

	if (Engine::KEY_DOWN(DIK_F5))
	{
		// SubQuest
		CQuestMgr::Get_Instance()->Set_IsAcceptQuest(true);
		CQuestMgr::Get_Instance()->Set_IsCompleteSubQuest(true);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsActive(true);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsChildActive(true);
		CQuestMgr::Get_Instance()->Get_MainQuestMiniCanvas()->Set_IsActive(true);
	}
	if (Engine::KEY_DOWN(DIK_F6))
	{
		CQuestMgr::Get_Instance()->Set_IsAcceptQuest(true);
		CQuestMgr::Get_Instance()->Set_IsCompleteSubQuest(true);
		CQuestMgr::Get_Instance()->Set_IsCompleteMainQuest(true);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsActive(true);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsChildActive(true);
		CQuestMgr::Get_Instance()->Get_MainQuestMiniCanvas()->Set_IsActive(true);
		CQuestMgr::Get_Instance()->Get_MainQuestMiniCanvas()->Set_IsChildActive(true);
	}
	if (Engine::KEY_DOWN(DIK_F7))
	{
		g_bIsCinemaStart = false;
		CQuestMgr::Get_Instance()->Set_IsAcceptQuest(false);
		CQuestMgr::Get_Instance()->Set_IsCompleteSubQuest(false);
		CQuestMgr::Get_Instance()->Set_IsCompleteMainQuest(false);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsActive(false);
		CQuestMgr::Get_Instance()->Get_SubQuestMiniCanvas()->Set_IsChildActive(false);
		CQuestMgr::Get_Instance()->Get_MainQuestMiniCanvas()->Set_IsActive(false);
		CQuestMgr::Get_Instance()->Get_MainQuestMiniCanvas()->Set_IsChildActive(false);
	}
}

void CMainApp::Show_FontLog(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Text :: PipelineState ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pRenderer)
	{
		m_uiCnt_ShaderFile			= m_pRenderer->Get_CntShaderFile();
		m_uiCnt_PipelineState		= m_pRenderer->Get_CntPipelineState();
		m_pRenderer->Reset_SetPipelineStateCnt();
	}

	m_wstrPipeline = wstring(L"[ Shader Info ]\n") +
					 wstring(L"Num ShaderFile : %d\n") +
					 wstring(L"Num PipelineState : %d\n");

	wsprintf(m_szPipeline, m_wstrPipeline.c_str(),
			 m_uiCnt_ShaderFile,
			 m_uiCnt_PipelineState);

	if (nullptr != m_pFont_PipelineState)
	{
		m_pFont_PipelineState->Update_GameObject(fTimeDelta);
		m_pFont_PipelineState->Set_Text(wstring(m_szPipeline));
	}

	/*__________________________________________________________________________________________________________
	[ Text :: RenderList Object ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pRenderer)
	{
		m_iPrioritySize		= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_PRIORITY);
		m_iNoneAlphaSize	= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_NONALPHA);
		m_iDistortionSize   = m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_DISTORTION);
		m_iEdgeSize         = m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_EDGE);
		m_iCrossFilterSize  = m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_CROSSFILTER);
		m_iMagicCircleSize  = m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_MAGICCIRCLE);
		m_iAlphaSize		= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_ALPHA);
		m_iUISize			= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_UI);
		m_iColliderSize		= m_pRenderer->Get_ColliderLstSize();
		m_iFontSize			= m_pRenderer->Get_RenderLstSize(m_pRenderer->RENDER_FONT);
	}
	m_wstrRenderList = wstring(L"[ Render Group ]\n") +
					   wstring(L"PRIORITY\t%d\n") +
					   wstring(L"NONEALPHA\t%d\n") +
					   wstring(L"DISTORTION\t%d\n") +
					   wstring(L"EDGE\t\t%d\n") +
					   wstring(L"CROSS FILTER\t%d\n") +
					   wstring(L"MAGIC CIRCLE\t%d\n") +
					   wstring(L"ALPHA\t\t%d\n") +
					   wstring(L"UI\t\t%d\n") +
					   wstring(L"COLLIDER\t%d\n") +
					   wstring(L"FONT\t\t%d\n") +
					   wstring(L"------------------   %d");

	_int iSum = m_iPrioritySize + m_iNoneAlphaSize + m_iDistortionSize + m_iEdgeSize + m_iCrossFilterSize + m_iMagicCircleSize + m_iAlphaSize + m_iUISize + m_iColliderSize + m_iFontSize;

	wsprintf(m_szRenderList, m_wstrRenderList.c_str(),
			 m_iPrioritySize,
			 m_iNoneAlphaSize,
			 m_iDistortionSize,
			 m_iEdgeSize,
			 m_iCrossFilterSize,
			 m_iMagicCircleSize,
			 m_iAlphaSize,
			 m_iUISize,
			 m_iColliderSize,
			 m_iFontSize,
			 iSum);

	if (nullptr != m_pFont_RenderList)
	{
		m_pFont_RenderList->Update_GameObject(fTimeDelta);
		m_pFont_RenderList->Set_Text(wstring(m_szRenderList));
	}

	/*__________________________________________________________________________________________________________
	[ Text :: System ]
	____________________________________________________________________________________________________________*/
	m_wstrSystem = wstring(L"[ System Infomation ] \n") +
				   wstring(L"GPU Name\t") + Engine::CGraphicDevice::Get_Instance()->Get_GraphicDeviceName() + wstring(L"\n") +
				   wstring(L"CPU Name\t") + m_wstrCPUName + wstring(L"\n") +
				   wstring(L"CPU's Core\t%d \n") +
				   wstring(L"CPU's Thread\t%d");

	wsprintf(m_szSystem, m_wstrSystem.c_str(),
			 m_iCPUCores,
			 m_iCPUThreads);

	if (nullptr != m_pFont_System)
	{
		m_pFont_System->Update_GameObject(fTimeDelta);
		m_pFont_System->Set_Text(wstring(m_szSystem));
	}
}

void CMainApp::Show_FontFPS(const _float& fTimeDelta)
{
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
	Engine::Safe_Release(m_pFont_System);
}
