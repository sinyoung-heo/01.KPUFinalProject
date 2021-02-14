#include "ShaderTextureInstancing.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "DescriptorHeapMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CShaderTextureInstancing)

CUploadBuffer<CB_SHADER_TEXTURE>* CShaderTextureInstancing::Get_UploadBuffer_ShaderMesh(wstring wstrTextureTag, const _uint& uiPipelineStatepass)
{
	auto& iter_find = m_mapCB_ShaderTexture.find(wstrTextureTag);

	if (iter_find != m_mapCB_ShaderTexture.end())
		return iter_find->second[uiPipelineStatepass];

	return nullptr;
}

HRESULT CShaderTextureInstancing::Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice	= pGraphicDevice;
	m_pCommandList		= pCommandList;

	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	m_uiPipelineStateCnt = (_uint)(m_vecPipelineState.size());

	return S_OK;
}

void CShaderTextureInstancing::SetUp_Instancing(wstring wstrTextureTag)
{
}

void CShaderTextureInstancing::SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice)
{
}

void CShaderTextureInstancing::Add_Instance(wstring wstrTextureTag, const _uint& iPipelineStateIdx)
{
}

void CShaderTextureInstancing::Reset_Instance()
{
}

void CShaderTextureInstancing::Reset_InstancingContainer()
{
}

void CShaderTextureInstancing::Reset_InstancingConstantBuffer()
{
}

void CShaderTextureInstancing::Render_Instance()
{
}

HRESULT CShaderTextureInstancing::Create_RootSignature()
{
	return S_OK;
}

HRESULT CShaderTextureInstancing::Create_PipelineState()
{
	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderTextureInstancing::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	return vector<D3D12_INPUT_ELEMENT_DESC>();
}

D3D12_BLEND_DESC CShaderTextureInstancing::Create_BlendState(const _bool& bIsBlendEnable, 
															 const D3D12_BLEND& SrcBlend, 
															 const D3D12_BLEND& DstBlend,
															 const D3D12_BLEND_OP& BlendOp, 
															 const D3D12_BLEND& SrcBlendAlpha,
															 const D3D12_BLEND& DstBlendAlpha,
															 const D3D12_BLEND_OP& BlendOpAlpha)
{
	return D3D12_BLEND_DESC();
}

void CShaderTextureInstancing::Free()
{
	CShader::Free();

	for (auto& pair : m_mapInstancing)
	{
		pair.second.clear();
		pair.second.shrink_to_fit();
	}

	for (auto& pair : m_mapCB_ShaderTexture)
	{
		for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
			Safe_Delete(pair.second[j]);
	}

}
