#include "ShaderBumpTerrain.h"
#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderBumpTerrain::CShaderBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderBumpTerrain::CShaderBumpTerrain(const CShaderBumpTerrain& rhs)
	: CShader(rhs)
{
}

HRESULT CShaderBumpTerrain::SetUp_ShaderConstantBuffer()
{
	m_pCB_ShaderMesh = CUploadBuffer<CB_SHADER_MESH>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShaderMesh, E_FAIL);

	return S_OK;
}

HRESULT CShaderBumpTerrain::SetUp_ShaderDescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture)
{
	m_uiTexSize = vecTexture.size();

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
	SRV_HeapDesc.NumDescriptors             = m_uiTexSize;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (_uint i = 0; i < m_uiTexSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format                        = vecTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip     = 0;
		SRV_Desc.Texture2D.MipLevels           = vecTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(vecTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}


	return S_OK;
}

HRESULT CShaderBumpTerrain::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderBumpTerrain::Begin_Shader()
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_pTexDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(TEX_DIFFUSE, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
												   SRV_TexDiffuseDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(TEX_NORMAL, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
												   SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(TEX_SPECULAR, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
												   SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(TEXTURE_END, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
												   SRV_TexShadowDepthDescriptorHandle);


	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(4,	// RootParameter Index
													  m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
													  m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());
}

void CShaderBumpTerrain::Begin_Shader(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_pTexDescriptorHeap };
	pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(TEX_DIFFUSE, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
												 SRV_TexDiffuseDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(TEX_NORMAL, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
												 SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(TEX_SPECULAR, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
												 SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(TEXTURE_END, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
												 SRV_TexShadowDepthDescriptorHandle);


	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(4,	// RootParameter Index
													m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
													m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());
}

HRESULT CShaderBumpTerrain::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV를 담는 서술자 테이블을 생성 ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[4];

	// TexDiffuse
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  0,								// 셰이더 인수들의 기준 레지스터 번호. (register t0)
					  0);								// 레지스터 공간.

	// TexNormal
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  1,								// 셰이더 인수들의 기준 레지스터 번호. (register t1)
					  0);								// 레지스터 공간.
	// Specular
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  2,								// 셰이더 인수들의 기준 레지스터 번호. (register t2)
					  0);								// 레지스터 공간.

	// ShadowDepth
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  3,								// 셰이더 인수들의 기준 레지스터 번호. (register t3)
					  0);								// 레지스터 공간.

	/*__________________________________________________________________________________________________________
	- 루트 매개변수는 테이블이거나, 루트 서술자 또는 루트 상수이다.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[6];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[4].InitAsConstantBufferView(0);	// register b0.
	RootParameter[5].InitAsConstantBufferView(1);	// register b1.

	auto StaticSamplers = Get_StaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),		// 루트 파라미터 개수.
												  RootParameter,
												  (UINT)StaticSamplers.size(),	// 샘플러 개수.
												  StaticSamplers.data(),		// 샘플러 데이터.
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

HRESULT CShaderBumpTerrain::Create_PipelineState()
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
	PipelineStateDesc.NumRenderTargets		= 5;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
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

	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderBumpTerrain::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderBumpTerrain.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderBumpTerrain.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderBumpTerrain::Create_BlendState(const _bool& bIsBlendEnable, const D3D12_BLEND& SrcBlend, const D3D12_BLEND& DstBlend, const D3D12_BLEND_OP& BlendOp, const D3D12_BLEND& SrcBlendAlpha, const D3D12_BLEND& DstBlendAlpha, const D3D12_BLEND_OP& BlendOpAlpha)
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

CComponent* CShaderBumpTerrain::Clone()
{
	return new CShaderBumpTerrain(*this);
}

CShader* CShaderBumpTerrain::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CShaderBumpTerrain* pInstance = new CShaderBumpTerrain(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderBumpTerrain::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShaderMesh);
}
