#include "ShaderTexture.h"

#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderTexture::CShaderTexture(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderTexture::CShaderTexture(const CShaderTexture & rhs)
	: CShader(rhs)
{
	/*__________________________________________________________________________________________________________
	- Texture Buffer의 경우, 객체마다 사용하는 텍스처 이미지와 개수가 다르기 때문에,
	GameObject::LateInit_GameObject()에서 Set_Shader_Texture() 함수 호출을 통해
	Create_DescriptorHeaps(pVecTexture), Create_ConstantBuffer()를 실행.
	____________________________________________________________________________________________________________*/
}

HRESULT CShaderTexture::SetUp_ShaderConstantBuffer(const _uint& iNumSubset)
{
	m_pCB_ShaderTexture = CUploadBuffer<CB_SHADER_TEXTURE>::Create(m_pGraphicDevice, iNumSubset);
	NULL_CHECK_RETURN(m_pCB_ShaderTexture, E_FAIL);

	return S_OK;
}

HRESULT CShaderTexture::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderTexture::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, 
								  const _uint& iSubsetIdx,
								  const _uint& iTexIdx,
								  const MATRIXID& eID)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_DescriptorHandle.Offset(iTexIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index
												   SRV_DescriptorHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	if (MATRIXID::PROJECTION == eID)
	{
		// 원근 투영.
		m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
														  m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());
	}
	else if (MATRIXID::ORTHO == eID)
	{
		// 직교 투영.
		m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
														  m_pCB_CameraOrthoMatrix->Resource()->GetGPUVirtualAddress());
	}
	else if (MATRIXID::TOP_VIEW == eID)
	{
		m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
														  m_pCB_CameraTopViewMatrix->Resource()->GetGPUVirtualAddress());
	}

	m_pCommandList->SetGraphicsRootConstantBufferView(2,	// RootParameter Index
													  m_pCB_ShaderTexture->Resource()->GetGPUVirtualAddress() +
													  m_pCB_ShaderTexture->GetElementByteSize() * iSubsetIdx);
}


HRESULT CShaderTexture::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV를 담는 서술자 테이블을 생성 ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table;
	SRV_Table.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
				   1,								// 서술자의 개수 - Texture2D의 개수.
				   0,								// 셰이더 인수들의 기준 레지스터 번호. (register t0)
				   0);								// 레지스터 공간.

	/*__________________________________________________________________________________________________________
	- 루트 매개변수는 테이블이거나, 루트 서술자 또는 루트 상수이다.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[3];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table, D3D12_SHADER_VISIBILITY_PIXEL);

	RootParameter[1].InitAsConstantBufferView(0);	// register b0.
	RootParameter[2].InitAsConstantBufferView(1);	// register b1.

	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),	// 루트 파라미터 개수.(CBV 2, SRV 1 : 총 3개)
												  RootParameter,
												  (_uint)StaticSamplers.size(),	// 샘플러 개수.
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

HRESULT CShaderTexture::Create_PipelineState()
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
	[ 1번 PipelineState Pass ]
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
	[ 2번 PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
	- Blend		(O)
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
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(true);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 3번 PipelineState Pass ]
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
	[ 4번 PipelineState Pass ]
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

	/*__________________________________________________________________________________________________________
	[ 5번 PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
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
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 6번 PipelineState Pass ]
	- "VS_GAUAGE"
	- "PS_GAUAGE_SPRITE"
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
	vecInputLayout							= Create_InputLayout("VS_GAUAGE", "PS_GAUAGE_SPRITE");
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
	[ 7번 PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN_ALPHA"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
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
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN_ALPHA");
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
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 8번 PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN_ALPHA"
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
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_MAIN_ALPHA");
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
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 9번 PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
	- Blend		(O)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout = Create_InputLayout("VS_TEXTURE_SPRITE", "PS_SNOW");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ONE,
		D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(true);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//10
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; 
	vecInputLayout = Create_InputLayout("VS_TEXTURE_SPRITE", "PS_TEXTURE_LIGHTING");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ONE,
		D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 11번 PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
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
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_SRC_ALPHA,
																D3D12_BLEND_INV_SRC_ALPHA,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE,
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 12번 PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
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
	vecInputLayout							= Create_InputLayout("VS_TEXTURE_SPRITE", "PS_TEXTURE_SPRITE_ALPHA");
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
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 13번 PipelineState Pass ]
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
	- Blend		(O)
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	vecInputLayout = Create_InputLayout("VS_TEXTURE_SPRITE", "PS_TEXTURE_SPRITE_ALPHA_COOLTIME");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ONE,
		D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState(false);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderTexture::Create_InputLayout(string VS_EntryPoint,
																		   string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderTexture.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderTexture.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderTexture::Create_BlendState(const _bool& bIsBlendEnable,
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
	BlendDesc.AlphaToCoverageEnable					= FALSE;
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


CComponent * CShaderTexture::Clone()
{
	return new CShaderTexture(*this);
}

CShader * CShaderTexture::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CShaderTexture* pInstance = new CShaderTexture(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderTexture::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShaderTexture);
}
