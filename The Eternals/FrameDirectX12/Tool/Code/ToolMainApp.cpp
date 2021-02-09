#include "stdafx.h"
#include "ToolMainApp.h"

#include "DirectInput.h"
#include "DirectSound.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "Font.h"
#include "ToolSceneLogo.h"

CToolMainApp::CToolMainApp()
	: m_pDeviceClass(Engine::CGraphicDevice::Get_Instance())
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pRenderer(Engine::CRenderer::Get_Instance())
	, m_pLightMgr(Engine::CLightMgr::Get_Instance())

{
}

HRESULT CToolMainApp::Ready_MainApp()
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

	return S_OK;
}

_int CToolMainApp::Update_MainApp(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pManagement, -1);

	/*__________________________________________________________________________________________________________
	Direct Input
	____________________________________________________________________________________________________________*/
	Engine::CDirectInput::Get_Instance()->SetUp_InputState();

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	Key_Input();

	/*__________________________________________________________________________________________________________
	[ Update Management ]
	____________________________________________________________________________________________________________*/
	m_pLightMgr->Update_Light();
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_pManagement->Update_Management(fTimeDelta);

	return S_OK;
}

_int CToolMainApp::LateUpdate_MainApp(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pManagement, -1);


	/*__________________________________________________________________________________________________________
	[ LateUpdate Management ]
	____________________________________________________________________________________________________________*/
	m_pManagement->LateUpdate_Management(fTimeDelta);

	return S_OK;
}

void CToolMainApp::Render_MainApp(const _float& fTimeDelta)
{
	if (nullptr == m_pDeviceClass || nullptr == m_pManagement)
		return;

	/*__________________________________________________________________________________________________________
	[ Render Management ]
	____________________________________________________________________________________________________________*/
	m_pManagement->Render_Management(fTimeDelta, Engine::RENDERID::SINGLE_THREAD);
}

HRESULT CToolMainApp::SetUp_DefaultSetting(Engine::WINMODE eMode, 
										   const _uint& uiWidth, 
										   const _uint& uiHeight)
{
	Engine::NULL_CHECK_RETURN(m_pDeviceClass, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GraphicDevice 초기화 ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pDeviceClass->Ready_GraphicDevice(g_hWnd, g_hInst, eMode, uiWidth, uiHeight), E_FAIL);

	m_pGraphicDevice = m_pDeviceClass->Get_GraphicDevice();
	Engine::NULL_CHECK_RETURN(m_pGraphicDevice, E_FAIL);

	m_pCommandList = m_pDeviceClass->Get_CommandList(Engine::CMDID::CMD_MAIN);
	Engine::NULL_CHECK_RETURN(m_pCommandList, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DirectInput 장치 초기화 ]
	____________________________________________________________________________________________________________*/
	if (FAILED(Engine::CDirectInput::Get_Instance()->Ready_InputDevice(g_hInst, g_hWnd, DISCL_NONEXCLUSIVE)))
	{
		Engine::CDirectInput::Get_Instance()->Destroy_Instance();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolMainApp::SetUp_Font()
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

	return S_OK;
}

HRESULT CToolMainApp::SetUp_ComponentPrototype()
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
	pComponent = Engine::CTerrainTex::Create(m_pGraphicDevice, m_pCommandList, 128, 128, 1.f, 40.0f);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"TerrainTex128", Engine::ID_STATIC, pComponent), E_FAIL);

	pComponent = Engine::CTerrainTex::Create(m_pGraphicDevice, m_pCommandList, 256, 256, 1.f, 40.0f);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"TerrainTex256", Engine::ID_STATIC, pComponent), E_FAIL);

	pComponent = Engine::CTerrainTex::Create(m_pGraphicDevice, m_pCommandList, 512, 512, 1.f, 40.0f);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"TerrainTex512", Engine::ID_STATIC, pComponent), E_FAIL);

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

	// CoordinateCol
	pComponent = Engine::CCoordinateCol::Create(m_pGraphicDevice, m_pCommandList);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pComponentMgr->Add_ComponentPrototype(L"CoordinateCol", Engine::ID_STATIC, pComponent), E_FAIL);

	return S_OK;
}

HRESULT CToolMainApp::SetUp_LoadingResource()
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
										  L"../../Bin/Resource/ResourceLoading/Texture/Logo/%d.dds", 4);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Logo", Engine::ID_STATIC, pComponent), E_FAIL);


	// Textue - Dissolve
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"Dissolve",
										  L"../../Bin/Resource/ResourceLoading/Texture/Dissolve/%d.dds", 1,
										  Engine::TEXTYPE::TEX_2D);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"ResourcePrototype_TextureDissolve", Engine::ID_STATIC, pComponent), E_FAIL);


	return S_OK;
}

HRESULT CToolMainApp::SetUp_StartScene(Engine::SCENEID eScebeID)
{
	if (nullptr == m_pManagement)
		return E_FAIL;

	Engine::CScene* pScene = nullptr;

	switch (eScebeID)
	{
	case Engine::SCENE_LOGO:
		pScene = CToolSceneLogo::Create(m_pGraphicDevice, m_pCommandList);
		break;
	default:
		break;
	}

	Engine::NULL_CHECK_RETURN(pScene, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pManagement->SetUp_CurrentScene(pScene), E_FAIL);

	return S_OK;
}

void CToolMainApp::Key_Input()
{
	/*__________________________________________________________________________________________________________
	[ Render On/Off ]
	____________________________________________________________________________________________________________*/
	if (Engine::KEY_DOWN(DIK_F1))
		m_pRenderer->Set_RenderOnOff(L"RenderTarget");

	if (Engine::KEY_DOWN(DIK_F2))
		m_pRenderer->Set_RenderOnOff(L"Font");

	if (Engine::KEY_DOWN(DIK_F3))
		m_pRenderer->Set_RenderOnOff(L"Collider");


}

CToolMainApp* CToolMainApp::Create()
{
	CToolMainApp* pInstance = new CToolMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		delete pInstance;
		pInstance = nullptr;
	}

	return pInstance;
}

void CToolMainApp::Free()
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

}
