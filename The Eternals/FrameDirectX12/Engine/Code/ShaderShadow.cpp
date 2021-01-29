#include "ShaderShadow.h"
#include "GraphicDevice.h"
#include "Renderer.h"


USING(Engine)

CShaderShadow::CShaderShadow(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderShadow::CShaderShadow(const CShaderShadow & rhs)
	: CShader(rhs)
{
	/*__________________________________________________________________________________________________________
	- Texture Buffer의 경우, 객체마다 사용하는 텍스처 이미지와 개수가 다르기 때문에,
	GameObject::LateInit_GameObject()에서 Set_Shader_Texture() 함수 호출을 통해
	Create_DescriptorHeaps(pvecTexDiffuse), Create_ConstantBuffer()를 실행.
	____________________________________________________________________________________________________________*/
}

HRESULT CShaderShadow::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderShadow::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint & iIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(0,	// RootParameter Index
													  m_pCB_ShadowShader->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
													  m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() + 
													  m_pCB_SkinningMatrix->GetElementByteSize() * iIdx);
}

void CShaderShadow::Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
								 const _int& iContextIdx,
								 ID3D12DescriptorHeap* pTexDescriptorHeap,
								 const _uint & iIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(0,	// RootParameter Index
													m_pCB_ShadowShader->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
													m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() + 
													m_pCB_SkinningMatrix->GetElementByteSize() * iIdx);
}

void CShaderShadow::SetUp_ShaderConstantBuffer(const _uint& iNumSubsetMesh)
{
	Create_ConstantBuffer(iNumSubsetMesh);
}

HRESULT CShaderShadow::Create_DescriptorHeaps()
{
	return S_OK;
}

HRESULT CShaderShadow::Create_ConstantBuffer(const _uint& iNumSubsetMesh)
{
	m_pCB_ShadowShader = CUploadBuffer<CB_SHADER_SHADOW>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShadowShader, E_FAIL);

	CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();

	// SubsetMesh 개수만큼 만들어준다.
	m_pCB_SkinningMatrix = CUploadBuffer<CB_SKINNING_MATRIX>::Create(m_pGraphicDevice, iNumSubsetMesh);
	NULL_CHECK_RETURN(m_pCB_SkinningMatrix, E_FAIL);

	return S_OK;
}

HRESULT CShaderShadow::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	- 루트 매개변수는 테이블이거나, 루트 서술자 또는 루트 상수이다.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[2];

	RootParameter[0].InitAsConstantBufferView(0);		// register b0.
	RootParameter[1].InitAsConstantBufferView(1);		// register b1.

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(2,	// 루트 파라미터 개수. (CBV 2)
												  RootParameter,
												  0,	
												  nullptr,
												  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	/*__________________________________________________________________________________________________________
	[ RootSignature를 생성 ]
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

HRESULT CShaderShadow::Create_PipelineState()
{
	/*__________________________________________________________________________________________________________
	[ PipelineState 기본 설정 ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState*				pPipelineState = nullptr;
	vector<D3D12_INPUT_ELEMENT_DESC>	vecInputLayout;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	PipelineStateDesc;
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));


	/*__________________________________________________________________________________________________________
	[ 0번 PipelineState Pass ]
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
	PipelineStateDesc.NumRenderTargets		= 1;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// ShadowDepth
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderShadow::Create_InputLayout(string VS_EntryPoint,
																  string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderShadow.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderShadow.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec2), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderShadow::Create_BlendState(const _bool& bIsBlendEnable,
												 const D3D12_BLEND& SrcBlend,
												 const D3D12_BLEND& DstBlend,
												 const D3D12_BLEND_OP& BlendOp,
												 const D3D12_BLEND& SrcBlendAlpha,
												 const D3D12_BLEND& DstBlendAlpha,
												 const D3D12_BLEND_OP& BlendOpAlpha)
{
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// 블렌드 설정.
	ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable					= TRUE;
	BlendDesc.IndependentBlendEnable				= FALSE;
	BlendDesc.RenderTarget[0].BlendEnable			= bIsBlendEnable;
	BlendDesc.RenderTarget[0].LogicOpEnable			= FALSE;
	BlendDesc.RenderTarget[0].SrcBlend				= SrcBlend;
	BlendDesc.RenderTarget[0].DestBlend				= DstBlend;
	BlendDesc.RenderTarget[0].BlendOp				= BlendOp;
	BlendDesc.RenderTarget[0].SrcBlendAlpha			= SrcBlendAlpha;
	BlendDesc.RenderTarget[0].DestBlendAlpha		= DstBlendAlpha;
	BlendDesc.RenderTarget[0].BlendOpAlpha			= BlendOpAlpha;
	BlendDesc.RenderTarget[0].LogicOp				= D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;

	return BlendDesc;
}


CComponent * CShaderShadow::Clone()
{
	return new CShaderShadow(*this);
}

CShader* CShaderShadow::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CShaderShadow* pInstance = new CShaderShadow(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderShadow::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShadowShader);
	Safe_Delete(m_pCB_SkinningMatrix);

}