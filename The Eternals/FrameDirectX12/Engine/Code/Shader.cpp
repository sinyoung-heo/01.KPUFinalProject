#include "Shader.h"
#include "GraphicDevice.h"


USING(Engine)

CShader::CShader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CComponent(pGraphicDevice, pCommandList)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pRootSignature(rhs.m_pRootSignature)
	, m_vecPipelineState(rhs.m_vecPipelineState)
	, m_uiCBV_SRV_UAV_DescriptorSize(rhs.m_uiCBV_SRV_UAV_DescriptorSize)
	, m_pVS_ByteCode(rhs.m_pVS_ByteCode)
	, m_pPS_ByteCode(rhs.m_pPS_ByteCode)
	, m_pCB_CameraMatrix(rhs.m_pCB_CameraMatrix)
{
	m_pRootSignature->AddRef();
	
	for (_uint i = 0; i < m_vecPipelineState.size(); ++i)
		m_vecPipelineState[i]->AddRef();

	m_pVS_ByteCode->AddRef();
	m_pPS_ByteCode->AddRef();
}


array<const CD3DX12_STATIC_SAMPLER_DESC, 6> CShader::Get_StaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0,									// shaderRegister
												D3D12_FILTER_MIN_MAG_MIP_POINT,		// filter
												D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressU
												D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressV
												D3D12_TEXTURE_ADDRESS_MODE_WRAP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(1,									// shaderRegister
												 D3D12_FILTER_MIN_MAG_MIP_POINT,	// filter
												 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
												 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
												 D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(2,									// shaderRegister
												 D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// filter
												 D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressU
												 D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// addressV
												 D3D12_TEXTURE_ADDRESS_MODE_WRAP);	// addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(3,								// shaderRegister
												  D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// filter
												  D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressU
												  D3D12_TEXTURE_ADDRESS_MODE_CLAMP,	// addressV
												  D3D12_TEXTURE_ADDRESS_MODE_CLAMP);// addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(4,								// shaderRegister
													  D3D12_FILTER_ANISOTROPIC,			// filter
													  D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
													  D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
													  D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
													  0.0f,                             // mipLODBias
													  8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(5,								// shaderRegister
													   D3D12_FILTER_ANISOTROPIC,		// filter
													   D3D12_TEXTURE_ADDRESS_MODE_CLAMP,// addressU
													   D3D12_TEXTURE_ADDRESS_MODE_CLAMP,// addressV
													   D3D12_TEXTURE_ADDRESS_MODE_CLAMP,// addressW
													   0.0f,                            // mipLODBias
													   8);                              // maxAnisotropy

	return { pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp };
}

ID3D12PipelineState * CShader::Get_PipelineState(const _uint & iIdx)
{
	if (iIdx >= m_vecPipelineState.size())
		return nullptr;

	return m_vecPipelineState[iIdx];
}

HRESULT CShader::Set_PipelineStatePass(const _uint & iIdx)
{
	/*__________________________________________________________________________________________________________
	[ PipelineState 설정 ]
	____________________________________________________________________________________________________________*/
	if (iIdx >= m_vecPipelineState.size())
		return E_FAIL;

	m_pPipelineState = m_vecPipelineState[iIdx];
	NULL_CHECK_RETURN(m_pPipelineState, E_FAIL);

	return S_OK;
}

HRESULT CShader::Ready_Shader()
{
	m_uiCBV_SRV_UAV_DescriptorSize = CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize();

	// Create CameraMatrix ConstantBuffer
	m_pCB_CameraMatrix = CUploadBuffer<CB_CAMERA_MATRIX>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_CameraMatrix, E_FAIL);

	return S_OK;
}

void CShader::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint& iIdx)
{
}

void CShader::Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
						   const _int& iContextIdx,
						   ID3D12DescriptorHeap* pTexDescriptorHeap,
						   const _uint& iIdx)
{
}

XMFLOAT4X4 CShader::Compute_MatrixTranspose(_matrix& mat)
{
	XMFLOAT4X4 matTranspose;

	XMStoreFloat4x4(&matTranspose, XMMatrixTranspose((mat)));

	return matTranspose;
}

HRESULT CShader::Create_DescriptorHeaps()
{
	return S_OK;
}

HRESULT CShader::Create_ConstantBuffer()
{
	return S_OK;
}

HRESULT CShader::Create_RootSignature()
{
	return S_OK;
}

HRESULT CShader::Create_PipelineState()
{
	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShader::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	return vector<D3D12_INPUT_ELEMENT_DESC>();
}

D3D12_BLEND_DESC CShader::Create_BlendState(const _bool & bIsBlendEnable,
											const D3D12_BLEND & SrcBlend, 
											const D3D12_BLEND & DstBlend, 
											const D3D12_BLEND_OP & BlendOp, 
											const D3D12_BLEND & SrcBlendAlpha, 
											const D3D12_BLEND & DstBlendAlpha, 
											const D3D12_BLEND_OP & BlendOpAlpha)
{
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	BlendDesc.RenderTarget[0].BlendEnable		= bIsBlendEnable;
	BlendDesc.RenderTarget[0].SrcBlend			= SrcBlend;
	BlendDesc.RenderTarget[0].DestBlend			= DstBlend;
	BlendDesc.RenderTarget[0].BlendOp			= BlendOp;
	BlendDesc.RenderTarget[0].SrcBlendAlpha		= SrcBlendAlpha;
	BlendDesc.RenderTarget[0].DestBlendAlpha	= DstBlendAlpha;
	BlendDesc.RenderTarget[0].BlendOpAlpha		= BlendOpAlpha;

	return BlendDesc;
}

D3D12_RASTERIZER_DESC CShader::Create_RasterizerState(const D3D12_FILL_MODE & eFillMode, 
													  const D3D12_CULL_MODE & eCullMode)
{
	D3D12_RASTERIZER_DESC RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// 레스터라이저 설정.
	ZeroMemory(&RasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	RasterizerDesc.FillMode					= eFillMode;
	RasterizerDesc.CullMode					= eCullMode;
	RasterizerDesc.FrontCounterClockwise	= FALSE;

	RasterizerDesc.DepthBias				= 0;
	RasterizerDesc.DepthBiasClamp			= 0.0f;
	RasterizerDesc.SlopeScaledDepthBias		= 0.0f;
	RasterizerDesc.DepthClipEnable			= TRUE;

	RasterizerDesc.MultisampleEnable		= TRUE;
	RasterizerDesc.AntialiasedLineEnable	= TRUE;

	RasterizerDesc.ForcedSampleCount		= 0;
	RasterizerDesc.ConservativeRaster		= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return RasterizerDesc;
}

D3D12_DEPTH_STENCIL_DESC CShader::Create_DepthStencilState(const _bool & bIsZWrite)
{
	D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// Depth & Stencil 설정.
	ZeroMemory(&DepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	DepthStencilDesc.DepthEnable					= bIsZWrite;
	DepthStencilDesc.DepthWriteMask					= D3D12_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc						= D3D12_COMPARISON_FUNC_LESS;

	DepthStencilDesc.StencilEnable					= FALSE;
	DepthStencilDesc.StencilReadMask				= 0x00;
	DepthStencilDesc.StencilWriteMask				= 0x00;

	DepthStencilDesc.FrontFace.StencilFailOp		= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp	= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilPassOp		= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;

	DepthStencilDesc.BackFace.StencilFailOp			= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp	= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilPassOp			= D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;

	return DepthStencilDesc;
}

ID3DBlob * CShader::Compile_Shader(const wstring & wstrFilename, 
								   const D3D_SHADER_MACRO * tDefines,
								   const string & strEntrypoint, 
								   const string & strTarget)
{
	_uint uiCompileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)  
	uiCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pByteCode = nullptr;
	ID3DBlob* pErrors;

	FAILED_CHECK_RETURN(D3DCompileFromFile(wstrFilename.c_str(),
										   tDefines, 
										   D3D_COMPILE_STANDARD_FILE_INCLUDE,
										   strEntrypoint.c_str(), 
										   strTarget.c_str(),
										   uiCompileFlags, 
										   0,
										   &pByteCode,
										   &pErrors), 
										   NULL);

	if (pErrors != nullptr)
		OutputDebugStringA((char*)pErrors->GetBufferPointer());

	//ThrowIfFailed(hResult);

	return pByteCode;
}

void CShader::Free()
{
	CComponent::Free();

	Safe_Release(m_pVS_ByteCode);
	Safe_Release(m_pPS_ByteCode);

	Safe_Release(m_pRootSignature);

	for (_uint i = 0; i < m_vecPipelineState.size(); ++i)
		Safe_Release(m_vecPipelineState[i]);

	m_vecPipelineState.clear();
	m_vecPipelineState.shrink_to_fit();

	if (!m_bIsClone)
		Safe_Delete(m_pCB_CameraMatrix);

}

