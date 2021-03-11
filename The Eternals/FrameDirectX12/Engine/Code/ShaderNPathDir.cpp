#include "ShaderNPathDir.h"

#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderNPathDir::CShaderNPathDir(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderNPathDir::CShaderNPathDir(const CShaderNPathDir & rhs)
	: CShader(rhs),m_pCB_ShaderVariable(rhs.m_pCB_ShaderVariable)
{
}



void CShaderNPathDir::SetUp_ShaderTexture(vector<ComPtr<ID3D12Resource>> pVecTexture)
{

	if (!m_bIsSetTexture)
	{
		m_bIsSetTexture = true;

		Create_DescriptorHeaps(pVecTexture);
		Create_ConstantBuffer();

		
	}
	
}

HRESULT CShaderNPathDir::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	//Cbuffer Shader
	m_pCB_ShaderVariable = CUploadBuffer<CB_SHADER_VARIABLE>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShaderVariable, E_FAIL);

	return S_OK;
}

void CShaderNPathDir::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint & iIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_pTexDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexEmissiveDescriptorHandle.Offset(0, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index. (Tex Emissive)
		SRV_TexEmissiveDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexCrossFilterObjectDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexCrossFilterObjectDescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index. (Tex Emissive)
		SRV_TexCrossFilterObjectDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDepthDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index. (Tex Emissive)
		SRV_TexDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexCrossFilterDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexCrossFilterDepthDescriptorHandle.Offset(3, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index. (Tex Emissive)
		SRV_TexCrossFilterDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSkyBoxDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSkyBoxDescriptorHandle.Offset(4, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index. (Tex Emissive)
		SRV_TexSkyBoxDescriptorHandle);
	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSkyBoxBrightDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSkyBoxBrightDescriptorHandle.Offset(5, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(5,		// RootParameter Index. (Tex Emissive)
		SRV_TexSkyBoxBrightDescriptorHandle);
	/*__________________________________________________________________________________________________________
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderVariable->Resource()->GetGPUVirtualAddress());
}

HRESULT CShaderNPathDir::Create_DescriptorHeaps(vector<ComPtr<ID3D12Resource>> pVecTexture)
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

HRESULT CShaderNPathDir::Create_ConstantBuffer()
{
	return S_OK;
}

HRESULT CShaderNPathDir::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV�� ��� ������ ���̺��� ���� ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[6];
	
	// Texture - Emissive
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  0,								// ���̴� �μ����� ���� �������� ��ȣ. (register t0)
					  0);								// �������� ����.
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		1,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		2,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		3,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
	SRV_Table[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		4,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
	SRV_Table[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		5,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
	/*__________________________________________________________________________________________________________
	- ��Ʈ �Ű������� ���̺��̰ų�, ��Ʈ ������ �Ǵ� ��Ʈ ����̴�.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[7];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);	// t0
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);	// t1
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);	// t2
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);	// t2
	RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);	// t2RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);	// t2
	RootParameter[5].InitAsDescriptorTable(1, &SRV_Table[5], D3D12_SHADER_VISIBILITY_PIXEL);	// t2
	RootParameter[6].InitAsConstantBufferView(0);		// register b0.
	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(7,							// ��Ʈ �Ķ���� ����.(SRV 1 : �Ѱ�)
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

HRESULT CShaderNPathDir::Create_PipelineState()
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
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
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
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_HIGHLIGHT");
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

vector<D3D12_INPUT_ELEMENT_DESC> CShaderNPathDir::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderNPathDir.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderNPathDir.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderNPathDir::Create_BlendState(const _bool & bIsBlendEnable, 
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

CComponent * CShaderNPathDir::Clone()
{
	return new CShaderNPathDir(*this);
}

CShader * CShaderNPathDir::Create(ID3D12Device * pGraphicDebice, ID3D12GraphicsCommandList* pCommandList)
{
	CShaderNPathDir* pInstance = new CShaderNPathDir(pGraphicDebice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderNPathDir::Free()
{
	CShader::Free();
	Safe_Release(m_pTexDescriptorHeap);

	if(!m_bIsClone)
		Safe_Delete(m_pCB_ShaderVariable);
}
