#include "Texture.h"
#include "GraphicDevice.h"
#include "DescriptorHeapMgr.h"

USING(Engine)

CTexture::CTexture(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CComponent(pGraphicDevice, pCommandList)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_vecResource(rhs.m_vecResource)
	, m_pTexDescriptorHeap(rhs.m_pTexDescriptorHeap)
	, m_uiTexSize(rhs.m_uiTexSize)
	, m_wstrFileName(rhs.m_wstrFileName)
{
	//for (auto& pTexture : m_vecResource)
	//	pTexture->AddRef();

	if(nullptr != m_pTexDescriptorHeap)
		m_pTexDescriptorHeap->AddRef();
}

HRESULT CTexture::Ready_Texture(wstring wstrFileName,
								wstring wstrFilePath,
								const _uint & iCnt,
								const TEXTYPE& eType)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(CMDID::CMD_LOADING);

	m_uiTexSize = iCnt;
	m_vecResource.reserve(iCnt);
	m_vecUpload.reserve(iCnt);

	m_wstrFileName = wstrFileName;

	_tchar szFullPath[MAX_PATH] = L"";

	for (_uint i = 0; i < iCnt; ++i)
	{
		/*__________________________________________________________________________________________________________
		L"../../Bin/Resource/ ''' /%d.dds" : %d => Texture Idx번호를 넣어서 문자열 완성.
		____________________________________________________________________________________________________________*/
		_tchar szFullPath[MAX_PATH] = L"";
		wsprintf(szFullPath, wstrFilePath.c_str(), i);

		ComPtr<ID3D12Resource> pResource	= nullptr;
		ComPtr<ID3D12Resource> pUpload		= nullptr;

		FAILED_CHECK_RETURN(CreateDDSTextureFromFile12(m_pGraphicDevice,
													   CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING),
													   szFullPath,	// 파일 경로. 
													   pResource,	// 텍스처가 담길 매개변수.
													   pUpload),	// 업로드 리소스.
													   E_FAIL);

		m_vecResource.emplace_back(pResource);
		m_vecUpload.emplace_back(pUpload);
	}

	// CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();
	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(CMDID::CMD_LOADING);

	// Create Texture DescriptorHeap.
	FAILED_CHECK_RETURN(Create_TextureDescriptorHeap(eType), E_FAIL);

	return S_OK;
}

HRESULT CTexture::Create_TextureDescriptorHeap(const TEXTYPE& eType)
{
	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors             = m_uiTexSize;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	if (TEXTYPE::TEX_2D == eType)
	{
		for (_uint i = 0; i < m_uiTexSize; ++i)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
			SRV_Desc.Format                        = m_vecResource[i]->GetDesc().Format;
			SRV_Desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRV_Desc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRV_Desc.Texture2D.MostDetailedMip     = 0;
			SRV_Desc.Texture2D.MipLevels           = m_vecResource[i]->GetDesc().MipLevels;
			SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

			m_pGraphicDevice->CreateShaderResourceView(m_vecResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

			// 힙의 다음 서술자로 넘어간다.
			SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
		}

	}
	else if (TEXTYPE::TEX_CUBE == eType)
	{
		for (_uint i = 0; i < m_uiTexSize; ++i)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
			SRV_Desc.Format                          = m_vecResource[i]->GetDesc().Format;
			SRV_Desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
			SRV_Desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			SRV_Desc.Texture2D.MostDetailedMip       = 0;
			SRV_Desc.Texture2D.MipLevels             = m_vecResource[i]->GetDesc().MipLevels;
			SRV_Desc.Texture2D.ResourceMinLODClamp   = 0.0f;

			m_pGraphicDevice->CreateShaderResourceView(m_vecResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

			// 힙의 다음 서술자로 넘어간다.
			SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
		}
	}


	// Add DescriptorHeap
	CDescriptorHeapMgr::Get_Instance()->Add_DescriptorHeap(m_wstrFileName, m_pTexDescriptorHeap);

	return S_OK;
}

void CTexture::Release_UploadBuffer()
{
	for (auto& pUpload : m_vecUpload)
		Safe_Release(pUpload);

	m_vecUpload.clear();
	m_vecUpload.shrink_to_fit();
}

CComponent * CTexture::Clone()
{
	return new CTexture(*this);
}

CTexture * CTexture::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
							wstring wstrFileName,
							wstring wstrFilePath,
							const _uint & iCnt,
							const TEXTYPE& eType)
{
	CTexture* pInstance = new CTexture(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Texture(wstrFileName, wstrFilePath, iCnt, eType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTexture::Free()
{
	//for (auto& pTexture : m_vecResource)
	//	pTexture->Release();

	Safe_Release(m_pTexDescriptorHeap);

	m_vecResource.clear();
	m_vecResource.shrink_to_fit();

}
