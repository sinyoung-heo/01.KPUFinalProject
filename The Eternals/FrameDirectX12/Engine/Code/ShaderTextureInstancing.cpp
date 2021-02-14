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
	/*__________________________________________________________________________________________________________
	[ SRV�� ��� ������ ���̺��� ���� ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table;
	SRV_Table.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
				   1,								// �������� ���� - Texture2D�� ����.
				   0,								// ���̴� �μ����� ���� �������� ��ȣ. (register t0)
				   0);								// �������� ����.

	/*__________________________________________________________________________________________________________
	- ��Ʈ �Ű������� ���̺��̰ų�, ��Ʈ ������ �Ǵ� ��Ʈ ����̴�.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[3];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table, D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsShaderResourceView(0, 1);	// register t0, space1.
	RootParameter[2].InitAsConstantBufferView(0);		// register b0.

	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),	// ��Ʈ �Ķ���� ����.(CBV 2, SRV 1 : �� 3��)
												  RootParameter,
												  (_uint)StaticSamplers.size(),	// ���÷� ����.
												  StaticSamplers.data(),		// ���÷� ������.
												  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	
	/*__________________________________________________________________________________________________________
	[ RootSignature�� ���� ]
	____________________________________________________________________________________________________________*/
	ID3DBlob* pSignatureBlob	= nullptr;
	ID3DBlob* pErrorBlob		= nullptr;

	FAILED_CHECK_RETURN(D3D12SerializeRootSignature(&RootSignatureDesc,
													D3D_ROOT_SIGNATURE_VERSION_1,
													&pSignatureBlob,
													&pErrorBlob), E_FAIL);

	if (nullptr != pErrorBlob)
	{
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		return E_FAIL;
	}

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateRootSignature(0,
															  pSignatureBlob->GetBufferPointer(),
															  pSignatureBlob->GetBufferSize(),
															  IID_PPV_ARGS(&m_pRootSignature)),
															  E_FAIL);
	Safe_Release(pSignatureBlob);
	Safe_Release(pErrorBlob);

	return S_OK;
}

HRESULT CShaderTextureInstancing::Create_PipelineState()
{
	/*__________________________________________________________________________________________________________
	[ PipelineState �⺻ ���� ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState*				pPipelineState = nullptr;
	vector<D3D12_INPUT_ELEMENT_DESC>	vecInputLayout;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	PipelineStateDesc;
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	/*__________________________________________________________________________________________________________
	[ 0�� PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 1�� PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN"
	- FILL_MODE_WIREFRAME
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState();
	PipelineStateDesc.RasterizerState		= Create_RasterizerState(D3D12_FILL_MODE_WIREFRAME);
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 2�� PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout							= Create_InputLayout("VS_TEXTURE_SPRITE", "PS_TEXTURE_SPRITE");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_SRC_ALPHA,
																D3D12_BLEND_INV_SRC_ALPHA,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE,
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(true);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 3�� PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_SRC_ALPHA,
																D3D12_BLEND_INV_SRC_ALPHA,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ZERO,
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 4�� PipelineState Pass ]
	- "VS_GAUAGE"
	- "PS_GAUAGE"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout							= Create_InputLayout("VS_GAUAGE", "PS_GAUAGE");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_SRC_ALPHA,
																D3D12_BLEND_INV_SRC_ALPHA,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ZERO,
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

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
