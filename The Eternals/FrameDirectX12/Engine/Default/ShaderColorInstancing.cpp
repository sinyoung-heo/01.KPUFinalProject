#include "ShaderColorInstancing.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CShaderColorInstancing)

CUploadBuffer<CB_SHADER_COLOR>* CShaderColorInstancing::Get_UploadBuffer_ShaderColor(const COLOR_BUFFER& eBuffer, 
																					 const _uint& uiPipelineStatepass)
{
	if (uiPipelineStatepass < m_uiPipelineStateCnt)
		return m_vecCB_ShaderColor[eBuffer][uiPipelineStatepass];

	return nullptr;
}

HRESULT CShaderColorInstancing::Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice	= pGraphicDevice;
	m_pCommandList		= pCommandList;

	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	m_uiPipelineStateCnt = (_uint)(m_vecPipelineState.size());
	ZeroMemory(m_uiTotalInstanceCnt, sizeof(_uint) * COLOR_BUFFER::COLORBUFFER_END);
	SetUp_Instancing();
	SetUp_VIBuffer();

	return S_OK;
}

void CShaderColorInstancing::SetUp_VIBuffer()
{
	m_arrVIBuffer[BUFFER_RECT]   = static_cast<CRcCol*>(CComponentMgr::Get_Instance()->Clone_Component(L"RcCol", COMPONENTID::ID_STATIC));
	m_arrVIBuffer[BUFFER_CUBE]   = static_cast<CCubeCol*>(CComponentMgr::Get_Instance()->Clone_Component(L"CubeCol", COMPONENTID::ID_STATIC));
	m_arrVIBuffer[BUFFER_BOX]    = static_cast<CColliderBox*>(CComponentMgr::Get_Instance()->Clone_Component(L"ColliderBox", COMPONENTID::ID_DYNAMIC));
	m_arrVIBuffer[BUFFER_SPHERE] = static_cast<CColliderSphere*>(CComponentMgr::Get_Instance()->Clone_Component(L"ColliderSphere", COMPONENTID::ID_DYNAMIC));
}

void CShaderColorInstancing::SetUp_Instancing()
{
	for (auto& vecInstancing : m_vecInstancing)
	{
		for (_uint i = 0; i < m_uiPipelineStateCnt; ++i)
			vecInstancing.push_back(INSTANCING_DESC());
	}

	for (auto& vecCB_ShaderColor : m_vecCB_ShaderColor)
	{
		vecCB_ShaderColor = vector<CUploadBuffer<CB_SHADER_COLOR>*>();
		vecCB_ShaderColor.resize(m_uiPipelineStateCnt);
	}
}

void CShaderColorInstancing::SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice)
{
	for (_uint i = 0; i < COLOR_BUFFER::COLORBUFFER_END; ++i)
	{
		for (auto& pUploadBuffer : m_vecCB_ShaderColor[i])
			pUploadBuffer = CUploadBuffer<CB_SHADER_COLOR>::Create(pGraphicDevice, m_uiTotalInstanceCnt[i], false);
	}

}

void CShaderColorInstancing::Reset_Instance()
{
	for (auto& vecInstancing : m_vecInstancing)
	{
		for (auto& tInstancingDesc : vecInstancing)
			tInstancingDesc.iInstanceCount = 0;
	}
}


void CShaderColorInstancing::Reset_InstancingConstantBuffer()
{
	ZeroMemory(m_uiTotalInstanceCnt, sizeof(_uint) * COLOR_BUFFER::COLORBUFFER_END);

	for (auto& vecCB_ShaderColor : m_vecCB_ShaderColor)
	{
		for (auto& pUploadBuffer : vecCB_ShaderColor)
			Safe_Delete(pUploadBuffer);
	}
}

void CShaderColorInstancing::Render_Instance()
{
	/*__________________________________________________________________________________________________________
	[ BufferType 별로 Rendering 수행 ]
	____________________________________________________________________________________________________________*/
	for (_uint iBufferID = 0; iBufferID < COLOR_BUFFER::COLORBUFFER_END; ++iBufferID)
	{
		/*__________________________________________________________________________________________________________
		[ PipelineStatePass 별로 Rendering 수행 ]
		____________________________________________________________________________________________________________*/
		for (_uint iPipelineStatePass = 0; iPipelineStatePass < (_uint)m_vecInstancing[iBufferID].size(); ++iPipelineStatePass)
		{
			if (!m_vecInstancing[iBufferID][iPipelineStatePass].iInstanceCount)
				continue;

			/*__________________________________________________________________________________________________________
			[ Set RootSignature ]
			____________________________________________________________________________________________________________*/
			m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

			/*__________________________________________________________________________________________________________
			[ Set PipelineState ]
			____________________________________________________________________________________________________________*/
			Set_PipelineStatePass(iPipelineStatePass);
			CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);

			/*__________________________________________________________________________________________________________
			[ SRV, CBV를 루트 서술자에 묶는다 ]
			____________________________________________________________________________________________________________*/
			m_pCommandList->SetGraphicsRootShaderResourceView(0,	// RootParameter Index
				m_vecCB_ShaderColor[iBufferID][iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

			m_pCommandList->SetGraphicsRootConstantBufferView(1,	// RootParameter Index
				m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

			/*__________________________________________________________________________________________________________
			[ Render Buffer ]
			____________________________________________________________________________________________________________*/
			m_pCommandList->IASetVertexBuffers(0, 1, &m_arrVIBuffer[iBufferID]->Get_VertexBufferView());
			m_pCommandList->IASetIndexBuffer(&m_arrVIBuffer[iBufferID]->Get_IndexBufferView());
			m_pCommandList->IASetPrimitiveTopology(m_arrVIBuffer[iBufferID]->Get_PrimitiveTopology());

			m_pCommandList->DrawIndexedInstanced(m_arrVIBuffer[iBufferID]->Get_SubMeshGeometry().uiIndexCount,
												 m_vecInstancing[iBufferID][iPipelineStatePass].iInstanceCount,
												 m_arrVIBuffer[iBufferID]->Get_SubMeshGeometry().uiStartIndexLocation,
												 m_arrVIBuffer[iBufferID]->Get_SubMeshGeometry().iBaseVertexLocation,
												 0);
		}
	}
}

HRESULT CShaderColorInstancing::Create_RootSignature()
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
	RootParameter[0].InitAsShaderResourceView(0, 1);	// register t0, space1.
	RootParameter[1].InitAsConstantBufferView(0);		// register b0 - cbCamreaMatrix

	/*____________________________________________________________________
	- 루트 서명은 루트 매개변수들의 배열이다.
	______________________________________________________________________*/
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),	// 루트 파라미터 개수. (cbCamreaMatrix, cbShaderColor)
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

HRESULT CShaderColorInstancing::Create_PipelineState()
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

vector<D3D12_INPUT_ELEMENT_DESC> CShaderColorInstancing::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderColorInstancing.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderColorInstancing.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

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

void CShaderColorInstancing::Free()
{
	CShader::Free();

	for (auto& vecInstancing : m_vecInstancing)
	{
		vecInstancing.clear();
		vecInstancing.shrink_to_fit();
	}

	for (auto& vecCB_ShaderColor : m_vecCB_ShaderColor)
	{
		for (auto& pUploadBuffer : vecCB_ShaderColor)
			Safe_Delete(pUploadBuffer);

		vecCB_ShaderColor.clear();
		vecCB_ShaderColor.shrink_to_fit();
	}

	for (auto& pVIBuffer : m_arrVIBuffer)
		Safe_Release(pVIBuffer);
}
