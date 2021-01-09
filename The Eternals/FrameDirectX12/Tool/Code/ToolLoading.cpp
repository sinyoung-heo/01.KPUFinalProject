#include "stdafx.h"
#include "ToolLoading.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"


CToolLoading::CToolLoading(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_bIsFinish(false)
{
	ZeroMemory(m_szLoadingStr, sizeof(_tchar) * 256);
	lstrcpy(m_szLoadingStr, L"Waiting for Loading Thread");
}

HRESULT CToolLoading::Ready_Loading(LOADINGID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoadingID = eLoadingID;

	return S_OK;
}

_uint CToolLoading::Loading_For_Stage()
{
	Loading_MeshFromFilePath();
	Loading_TextureFromFilePath();

	m_bIsFinish			= true;
	g_bIsLoadingFinish	= true;

	lstrcpy(m_szLoadingStr, L"Resource Loading is Finish");

	return 0;
}

HRESULT CToolLoading::Loading_MeshFromFilePath()
{
	wifstream fin { L"../../Bin/ToolData/PathFind_Mesh.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar szMeshTag[MAX_STR]	= L"";
	_tchar szFileName[MAX_STR]	= L"";
	_tchar szPath[MAX_STR]		= L"";

	wstring	wstrMeshTag		= L"";
	wstring	wstrFileName	= L"";
	wstring	wstrPath		= L"";
	Engine::CComponent* pComponent = nullptr;

	while (true)
	{
		fin.getline(szMeshTag, MAX_STR, '|');
		fin.getline(szFileName, MAX_STR, '|');
		fin.getline(szPath, MAX_STR);

		if (fin.eof())
			break;

		lstrcpy(m_szLoadingStr, szMeshTag);
		lstrcat(m_szLoadingStr, L" Mesh Loading");

		wstrMeshTag		= szMeshTag;
		wstrFileName	= szFileName;
		wstrPath		= szPath;
		wstrPath		+= L"\\";

		pComponent = Engine::CMesh::Create(m_pGraphicDevice, m_pCommandList,
										   wstrPath,
										   wstrFileName);
		Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
		Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(wstrMeshTag, Engine::ID_STATIC, pComponent), E_FAIL);

		++m_iCurLoadingCnt;
	}

	fin.close();

	return S_OK;
}

HRESULT CToolLoading::Loading_TextureFromFilePath()
{
	wifstream fin { L"../../Bin/ToolData/PathFind_Texture.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar szTextureTag[MAX_STR]	= L"";
	_tchar szCount[MAX_STR]			= L"";
	_tchar szPath[MAX_STR]			= L"";

	wstring	wstrTextureTag	= L"";
	wstring	wstrPath		= L"";
	Engine::CComponent* pComponent = nullptr;

	while (true)
	{
		fin.getline(szTextureTag, MAX_STR, '|');
		fin.getline(szCount, MAX_STR, '|');
		fin.getline(szPath, MAX_STR);

		if (fin.eof())
			break;

		lstrcpy(m_szLoadingStr, szTextureTag);
		lstrcat(m_szLoadingStr, L" Texture Loading");

		_int iTexSize = _ttoi(szCount);

		Engine::TEXTYPE eTexType;
		if (!lstrcmp(szTextureTag, L"SkyBox"))
			eTexType = Engine::TEXTYPE::TEX_CUBE;
		else
			eTexType = Engine::TEXTYPE::TEX_2D;

		wstrTextureTag = szTextureTag;
		wstrPath = szPath;
		wstrPath += L"\\";

		pComponent = Engine::CTexture::Create(m_pGraphicDevice, m_pCommandList,
											  wstrPath, 
											  iTexSize, 
											  eTexType);
		Engine::NULL_CHECK_RETURN(pComponent, E_FAIL);
		Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(wstrTextureTag, Engine::ID_STATIC, pComponent), E_FAIL);

		++m_iCurLoadingCnt;
	}

	fin.close();

	return S_OK;
}


unsigned int __stdcall CToolLoading::Thread_Main(void* pArg)
{
	CToolLoading* pLoading = (CToolLoading*)pArg;
	_uint iFlag = 0;


	while(true)
	{
		cout << g_bIsLoadingStart << endl;
		if (g_bIsLoadingStart)
		{
			// EnterCriticalSection(pLoading->Get_Crt());

			switch (pLoading->Get_LoadingID())
			{
			case LOADING_STAGE:
				pLoading->Loading_For_Stage();

				break;
			default:
				break;
			}

			// LeaveCriticalSection(pLoading->Get_Crt());

			break;
		}

	}

	return 0;
}

CToolLoading* CToolLoading::Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* 
								   pCommandList, LOADINGID 
								   eLoadingID)
{
	CToolLoading* pInstance = new CToolLoading(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CToolLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
}
