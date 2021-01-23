#include "ShaderColor.h"

#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderColor::CShaderColor(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderColor::CShaderColor(const CShaderColor & rhs)
	: CShader(rhs)
{
	Create_DescriptorHeaps();
	Create_ConstantBuffer();
}


HRESULT CShaderColor::Ready_Shader()
{
	CShader::Ready_Shader();

	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderColor::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, 
								const _uint& iSubsetIdx,
								const MATRIXID& eID)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	if (MATRIXID::PROJECTION == eID)
	{
		m_pCommandList->SetGraphicsRootConstantBufferView(0,	// RootParameter Index
														  m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());
	}
	else if (MATRIXID::ORTHO == eID)
	{
		m_pCommandList->SetGraphicsRootConstantBufferView(0,	// RootParameter Index
														  m_pCB_CameraOrthoMatrix->Resource()->GetGPUVirtualAddress());
	}

	m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
													  m_pCB_ShaderColor->Resource()->GetGPUVirtualAddress());
}

HRESULT CShaderColor::Create_DescriptorHeaps()
{
	/*__________________________________________________________________________________________________________
	CBV만 사용하는 셰이더의 경우, 서술자 힙을 만들지 않아도 된다.
	____________________________________________________________________________________________________________*/

	return S_OK;
}

HRESULT CShaderColor::Create_ConstantBuffer()
{
	m_pCB_ShaderColor = CUploadBuffer<CB_SHADER_COLOR>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShaderColor, E_FAIL);

	return S_OK;
}

HRESULT CShaderColor::Create_RootSignature()
{
	/*____________________________________________________________________
	 일반적으로 셰이더 프로그램은 특정 자원들(상수 버퍼, 텍스처, 샘플러) 등이 입력된다고 기대한다.
	루트 서명은 셰이더 프로그램이 기대하는 자원들을 정의한다. 
	셰이더 프로그램은 본질적으로 하나의 함수이고, 셰이더에 입력되는 자원들은 함수의 매개변수들에 해당하므로 
	루트 서명은 곧 함수 서명을 정의하는 수단이라 할 수 있다.
	______________________________________________________________________*/

	/*____________________________________________________________________
	- Create root CBV.
	______________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[2];

	RootParameter[0].InitAsConstantBufferView(0);	// register b0 - cbCamreaMatrix
	RootParameter[1].InitAsConstantBufferView(1);	// register b1 - cbShaderColor

	/*____________________________________________________________________
	- 루트 서명은 루트 매개변수들의 배열이다.
	______________________________________________________________________*/
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(2,		// 루트 파라미터 개수. (cbCamreaMatrix, cbShaderColor)
												  RootParameter, 
												  0,		// Texture가 없으므로 0.
												  nullptr,	// Texture가 없으므로 nullptr.
												  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

HRESULT CShaderColor::Create_PipelineState()
{
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
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout();
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= CShader::Create_BlendState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID);

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
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout();
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= CShader::Create_BlendState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_WIREFRAME);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 2번 PipelineState Pass ]
	- "VS_MAIN_RANDOM"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN_RANDOM");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= CShader::Create_BlendState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 3번 PipelineState Pass ]
	- D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
	- "VS_MAIN_RANDOM"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	PipelineStateDesc.NumRenderTargets		= 1;
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN_RANDOM");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= CShader::Create_BlendState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderColor::Create_InputLayout(string VS_EntryPoint,
																  string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderColor.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderColor.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;

	vecInputLayout =
	{
		{
			"POSITION", 								// 시멘틱 이름.
			0, 											// 시멘티 인덱스.
			DXGI_FORMAT_R32G32B32_FLOAT,				// DXGI 포맷.
			0, 											// Input Slot.
			uiOffset,									// 정점 구조체의 시작 위치와, 이 정점 성분 시작 위치 사이의 거리.
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// Input Slot Class.
			0 											// Instance Data Step Rate.
		},

		{
			"COLOR",									// 시멘틱 이름.
			0, 											// 시멘티 인덱스.
			DXGI_FORMAT_R32G32B32A32_FLOAT,				// DXGI 포맷.
			0, 											// Input Slot.
			uiOffset += sizeof(_vec3),					// 정점 구조체의 시작 위치와, 이 정점 성분 시작 위치 사이의 거리.
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input Slot Class.
			0 											// Instance Data Step Rate.
		}
	};

	return vecInputLayout;
}

CComponent * CShaderColor::Clone()
{
	return new CShaderColor(*this);
}

CShaderColor * CShaderColor::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CShaderColor* pInstance = new CShaderColor(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderColor::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShaderColor);
}