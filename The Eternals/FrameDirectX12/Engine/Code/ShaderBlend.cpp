#include "ShaderBlend.h"

#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderBlend::CShaderBlend(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderBlend::CShaderBlend(const CShaderBlend & rhs)
	: CShader(rhs), m_pCB_ShaderInformation(rhs.m_pCB_ShaderInformation)
{
}

void CShaderBlend::SetUp_ShaderTexture(vector<ComPtr<ID3D12Resource>> pVecTexture)
{

	if (!m_bIsSetTexture)
	{
		m_bIsSetTexture = true;

		Create_DescriptorHeaps(pVecTexture);
		Create_ConstantBuffer();
	}
}

HRESULT CShaderBlend::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	//Cbuffer Shader
	m_pCB_ShaderInformation = CUploadBuffer<CB_SHADER_INFORMATION>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShaderInformation, E_FAIL);
	return S_OK;
}

void CShaderBlend::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint & iIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_pTexDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(0, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index. (Tex Diffuse)
												   SRV_TexDiffuseDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadeDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadeDescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index. (Tex Shade)
												   SRV_TexShadeDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index. (Tex Specular)
												   SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexBlurDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexBlurDescriptorHandle.Offset(3, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index. (Tex Blur)
		SRV_TexBlurDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexEmissiveDescriptorHandle.Offset(4, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index. (Tex Emissive)
		SRV_TexEmissiveDescriptorHandle);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSSAODescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSSAODescriptorHandle.Offset(5, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(5,		// RootParameter Index. (Tex SSAO)
		SRV_TexSSAODescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDistortionDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDistortionDescriptorHandle.Offset(6, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(6,		// RootParameter Index. (Tex Distortion)
		SRV_TexDistortionDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexAveColorDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexAveColorDescriptorHandle.Offset(7, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(7,		// RootParameter Index. (Tex Ave)
		SRV_TexAveColorDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexBlendDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexBlendDescriptorHandle.Offset(8, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(8,		// RootParameter Index. (Tex Blend)
		SRV_TexBlendDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDepthDescriptorHandle.Offset(9, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(9,		// RootParameter Index. (Tex Blend)
		SRV_TexDepthDescriptorHandle);

	//Cbuffer
	m_pCommandList->SetGraphicsRootConstantBufferView(10,	// RootParameter Index
		m_pCB_ShaderInformation->Resource()->GetGPUVirtualAddress());

}

HRESULT CShaderBlend::Create_DescriptorHeaps(vector<ComPtr<ID3D12Resource>> pVecTexture)
{
	/*__________________________________________________________________________________________________________
	[ SRV ������ �� ]
	- �ؽ�ó �ڿ��� ���������� �����ߴٸ�, �� �������� �ؾ��� ���� �׿� ���� SRV �����ڸ� �����ϴ� ���̴�. 
	�׷��� �ڿ��� ���̴� ���α׷��� ����� ��Ʈ ���� �Ű����� ���Կ� ������ �� �ִ�.
	____________________________________________________________________________________________________________*/
	m_uiTexSize = _uint(pVecTexture.size());

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc	= {};
	SRV_HeapDesc.NumDescriptors	= m_uiTexSize;	// �ؽ�ó�� ���� ��ŭ ����.
	SRV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	/*__________________________________________________________________________________________________________
	[ SRV ������ ]
	- SRV ���� ������ �������� ���� �����ڵ��� �����ؾ� �Ѵ�.
	____________________________________________________________________________________________________________*/
	// ���� ������ ����Ű�� �����͸� ��´�.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (_uint i = 0; i < m_uiTexSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format							= pVecTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip		= 0;
		SRV_Desc.Texture2D.MipLevels			= pVecTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(pVecTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// ���� ���� �����ڷ� �Ѿ��.
		SRV_DescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	}

	return S_OK;
}

HRESULT CShaderBlend::Create_ConstantBuffer()
{
	return S_OK;
}

HRESULT CShaderBlend::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV�� ��� ������ ���̺��� ���� ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[10];
	
	// Texture - DiffuseTarget
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  0,								// ���̴� �μ����� ���� �������� ��ȣ. (register t0)
					  0);								// �������� ����.

	// Texture - ShadeTarget
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  1,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
					  0);								// �������� ����.

	// Texture - SpecularTarget
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  2,								// ���̴� �μ����� ���� �������� ��ȣ. (register t2)
					  0);								// �������� ����.

		// Texture - Blur
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		3,								// ���̴� �μ����� ���� �������� ��ȣ. (register t3)
		0);								// �������� ����.
		// Texture - Emissive
	SRV_Table[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		4,								// ���̴� �μ����� ���� �������� ��ȣ. (register t4)
		0);								// �������� ����.

	// Texture - SSAO
	SRV_Table[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		5,								// ���̴� �μ����� ���� �������� ��ȣ. (register t5)
		0);								// �������� ����

	// Texture - Distortion
	SRV_Table[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		6,								// ���̴� �μ����� ���� �������� ��ȣ. (register t6)
		0);								// �������� ����.

		// Texture - average color
	SRV_Table[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		7,								// ���̴� �μ����� ���� �������� ��ȣ. (register t7)
		0);								// �������� ����.
		// Texture - average color
	SRV_Table[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		8,								// ���̴� �μ����� ���� �������� ��ȣ. (register t8)
		0);								// �������� ����.

	SRV_Table[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		9,								// ���̴� �μ����� ���� �������� ��ȣ. (register t9)
		0);								// �������� ����.
	/*__________________________________________________________________________________________________________
	- ��Ʈ �Ű������� ���̺��̰ų�, ��Ʈ ������ �Ǵ� ��Ʈ ����̴�.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[11];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);	// t0
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);	// t1
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);	// t2
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);	// t3
	RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);	// t4
	RootParameter[5].InitAsDescriptorTable(1, &SRV_Table[5], D3D12_SHADER_VISIBILITY_PIXEL);	// t5
	RootParameter[6].InitAsDescriptorTable(1, &SRV_Table[6], D3D12_SHADER_VISIBILITY_PIXEL);	// t6
	RootParameter[7].InitAsDescriptorTable(1, &SRV_Table[7], D3D12_SHADER_VISIBILITY_PIXEL);	// t7
	RootParameter[8].InitAsDescriptorTable(1, &SRV_Table[8], D3D12_SHADER_VISIBILITY_PIXEL);	// t8
	RootParameter[9].InitAsDescriptorTable(1, &SRV_Table[9], D3D12_SHADER_VISIBILITY_PIXEL);	// t9
	RootParameter[10].InitAsConstantBufferView(0); //b0

	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(11,							// ��Ʈ �Ķ���� ����.(SRV 10 : ��10��)
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

HRESULT CShaderBlend::Create_PipelineState()
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
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R16G16B16A16_UNORM;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);	// Blend�� �� Depth�� false

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_FINAL");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(false);	// Blend�� �� Depth�� false

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderBlend::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderBlend.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderBlend.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderBlend::Create_BlendState(const _bool & bIsBlendEnable, 
												 const D3D12_BLEND & SrcBlend, 
												 const D3D12_BLEND & DstBlend, 
												 const D3D12_BLEND_OP & BlendOp, 
												 const D3D12_BLEND & SrcBlendAlpha,
												 const D3D12_BLEND & DstBlendAlpha,
												 const D3D12_BLEND_OP & BlendOpAlpha)
{
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// ���� ����.
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

CComponent * CShaderBlend::Clone()
{
	return new CShaderBlend(*this);
}

CShader * CShaderBlend::Create(ID3D12Device * pGraphicDebice, ID3D12GraphicsCommandList* pCommandList)
{
	CShaderBlend* pInstance = new CShaderBlend(pGraphicDebice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderBlend::Free()
{
	CShader::Free();
	Safe_Release(m_pTexDescriptorHeap);
	if (!m_bIsClone)
		Safe_Delete(m_pCB_ShaderInformation);
}
