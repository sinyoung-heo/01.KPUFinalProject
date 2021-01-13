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

void CShaderColor::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint& iIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(0,	// RootParameter Index
													  m_pCB_MatrixDesc->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
													  m_pCB_ColorDesc->Resource()->GetGPUVirtualAddress());
}

HRESULT CShaderColor::Create_DescriptorHeaps()
{
	/*__________________________________________________________________________________________________________
	CBV�� ����ϴ� ���̴��� ���, ������ ���� ������ �ʾƵ� �ȴ�.
	____________________________________________________________________________________________________________*/

	return S_OK;
}

HRESULT CShaderColor::Create_ConstantBuffer()
{
	m_pCB_MatrixDesc = CUploadBuffer<CB_MATRIX_DESC>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_MatrixDesc, E_FAIL);

	m_pCB_ColorDesc = CUploadBuffer<CB_COLOR_DESC>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ColorDesc, E_FAIL);

	return S_OK;
}

HRESULT CShaderColor::Create_RootSignature()
{
	/*____________________________________________________________________
	 �Ϲ������� ���̴� ���α׷��� Ư�� �ڿ���(��� ����, �ؽ�ó, ���÷�) ���� �Էµȴٰ� ����Ѵ�.
	��Ʈ ������ ���̴� ���α׷��� ����ϴ� �ڿ����� �����Ѵ�. 
	���̴� ���α׷��� ���������� �ϳ��� �Լ��̰�, ���̴��� �ԷµǴ� �ڿ����� �Լ��� �Ű������鿡 �ش��ϹǷ� 
	��Ʈ ������ �� �Լ� ������ �����ϴ� �����̶� �� �� �ִ�.
	______________________________________________________________________*/

	/*____________________________________________________________________
	- Create root CBV.
	______________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[2];

	RootParameter[0].InitAsConstantBufferView(0);	// register b0 - cbMatrixInfo
	RootParameter[1].InitAsConstantBufferView(1);	// register b1 - cbColorInfo

	/*____________________________________________________________________
	- ��Ʈ ������ ��Ʈ �Ű��������� �迭�̴�.
	______________________________________________________________________*/
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(2,		// ��Ʈ �Ķ���� ����. (cbMatrixInfo, cbColorInfo)
												  RootParameter, 
												  0,		// Texture�� �����Ƿ� 0.
												  nullptr,	// Texture�� �����Ƿ� nullptr.
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
	Engine::Safe_Release(pSignatureBlob);
	Engine::Safe_Release(pErrorBlob);

	return S_OK;
}

HRESULT CShaderColor::Create_PipelineState()
{
	/*__________________________________________________________________________________________________________
	[ PipelineState �⺻ ���� ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState*				pPipelineState = nullptr;
	vector<D3D12_INPUT_ELEMENT_DESC>	vecInputLayout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	PipelineStateDesc;
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));


	// 0�� PipelineState Pass
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
	PipelineStateDesc.BlendState			= Create_BlendState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState();
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc,
																	  IID_PPV_ARGS(&pPipelineState)), 
																	  E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	// 1�� PipelineState Pass
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_WIREFRAME);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc,
																	  IID_PPV_ARGS(&pPipelineState)),
																	  E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	// 2�� PipelineState Pass
	vecInputLayout							= Create_InputLayout("VS_MAIN_RANDOM");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc,
																	  IID_PPV_ARGS(&pPipelineState)), 
																	  E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	// 3�� PipelineState Pass (��ǥ��)
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc,
																	  IID_PPV_ARGS(&pPipelineState)), 
																	  E_FAIL);
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
			"POSITION", 								// �ø�ƽ �̸�.
			0, 											// �ø�Ƽ �ε���.
			DXGI_FORMAT_R32G32B32_FLOAT,				// DXGI ����.
			0, 											// Input Slot.
			uiOffset,									// ���� ����ü�� ���� ��ġ��, �� ���� ���� ���� ��ġ ������ �Ÿ�.
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// Input Slot Class.
			0 											// Instance Data Step Rate.
		},

		{
			"COLOR",									// �ø�ƽ �̸�.
			0, 											// �ø�Ƽ �ε���.
			DXGI_FORMAT_R32G32B32A32_FLOAT,				// DXGI ����.
			0, 											// Input Slot.
			uiOffset += sizeof(_vec3),					// ���� ����ü�� ���� ��ġ��, �� ���� ���� ���� ��ġ ������ �Ÿ�.
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // Input Slot Class.
			0 											// Instance Data Step Rate.
		}
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderColor::Create_BlendState(const _bool& bIsBlendEnable,
														const D3D12_BLEND& SrcBlend,
														const D3D12_BLEND& DstBlend,
														const D3D12_BLEND_OP& BlendOp,
														const D3D12_BLEND& SrcBlendAlpha,
														const D3D12_BLEND& DstBlendAlpha,
														const D3D12_BLEND_OP& BlendOpAlpha)
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

CComponent * CShaderColor::Clone()
{
	return new CShaderColor(*this);
}

CShaderColor * CShaderColor::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CShaderColor* pInstance = new CShaderColor(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CShaderColor::Free()
{
	CShader::Free();

	Engine::Safe_Delete(m_pCB_MatrixDesc);
	Engine::Safe_Delete(m_pCB_ColorDesc);
}