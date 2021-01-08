#include "stdafx.h"
#include "Loading.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"

CLoading::CLoading(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_bIsFinish(false)
{
	ZeroMemory(m_szLoadingStr, sizeof(_tchar) * 256);
}


HRESULT CLoading::Ready_Loading(LOADINGID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoadingID = eLoadingID;

	return S_OK;
}

_uint CLoading::Loading_For_Stage()
{
	Loading_DynamicMesh();
	Loading_StaticMesh();
	Loading_Texture();

	m_bIsFinish = true;

	lstrcpy(m_szLoadingStr, L"Loading Finish");

#ifdef CLIENT_LOG
	COUT_STR("<< Loading Finish >>");
	COUT_STR("");
#endif

	return 0;
}

HRESULT CLoading::Loading_DynamicMesh()
{
	lstrcpy(m_szLoadingStr, L"Loading DynamicMesh");

	Engine::CComponent* pComponent = nullptr;

	// Popori
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/DynamicMesh/PoporiR19/", 
									   L"PoporiR19.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"PoporiR19", Engine::ID_STATIC, pComponent), E_FAIL);

	// Popori
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/DynamicMesh/PoporiH25/", 
									   L"PoporiH25.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"PoporiH25", Engine::ID_STATIC, pComponent), E_FAIL);

	return S_OK;
}

HRESULT CLoading::Loading_StaticMesh()
{
	lstrcpy(m_szLoadingStr, L"Loading StaticMesh");

	Engine::CComponent* pComponent = nullptr;

	// LandMark
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/StaticMesh/LandMark/",
									   L"LandMark.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"LandMark", Engine::ID_STATIC, pComponent), E_FAIL);

	// LandMarkBoundary
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/StaticMesh/LandMarkBoundary/",
									   L"LandMarkBoundary.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"LandMarkBoundary", Engine::ID_STATIC, pComponent), E_FAIL);

	// LandMarkPlane
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/StaticMesh/LandMarkPlane/",
									   L"LandMarkPlane.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"LandMarkPlane", Engine::ID_STATIC, pComponent), E_FAIL);

	// Status
	pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
									   L"../../Bin/Resource/ResourceStage/StaticMesh/Status/",
									   L"Status.X");
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Status", Engine::ID_STATIC, pComponent), E_FAIL);


	return S_OK;
}

HRESULT CLoading::Loading_Texture()
{
	lstrcpy(m_szLoadingStr, L"Loading Texture");

	Engine::CComponent* pComponent = nullptr;

	// Texture - SkyBox
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"../../Bin/Resource/ResourceStage/Texture/SkyBox/%d.dds", 3, 
										  Engine::TEXTYPE::TEX_CUBE);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"SkyBox", Engine::ID_STATIC, pComponent), E_FAIL);

	// Texture - Terrain
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"../../Bin/Resource/ResourceStage/Texture/Terrain/%d.dds", 1,
										  Engine::TEXTYPE::TEX_2D);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Terrain", Engine::ID_STATIC, pComponent), E_FAIL);

	// Texture - Fire
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"../../Bin/Resource/ResourceStage/Texture/Fire/%d.dds", 1,
										  Engine::TEXTYPE::TEX_2D);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Fire", Engine::ID_STATIC, pComponent), E_FAIL);

	// Textue - Torch
	pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
										  L"../../Bin/Resource/ResourceStage/Texture/Torch/%d.dds", 1,
										  Engine::TEXTYPE::TEX_2D);
	Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"Torch", Engine::ID_STATIC, pComponent), E_FAIL);

	return S_OK;
}



unsigned int CLoading::Thread_Main(void * pArg)
{
	CLoading* pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		pLoading->Loading_For_Stage();

		break;
	default:

		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return 0;
}

CLoading * CLoading::Create(ID3D12Device * pGraphicDevice, 
							ID3D12GraphicsCommandList * pCommandList, 
							LOADINGID eLoadingID)
{
	CLoading* pInstance = new CLoading(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

#ifdef CLIENT_LOG
	COUT_STR("<< Destroy Loading Thread >>");
	COUT_STR("");
#endif
}
