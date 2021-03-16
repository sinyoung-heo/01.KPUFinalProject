#include "ShaderLightingInstancing.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CShaderLightingInstancing)


CUploadBuffer<CB_SHADER_LIGHTING>* CShaderLightingInstancing::Get_UploadBuffer_ShaderLighting(const _uint& uiPipelineStatepass)
{
	if (uiPipelineStatepass < m_uiPipelineStateCnt)
		return m_vecCB_ShaderLighting[uiPipelineStatepass];

	return nullptr;
}

HRESULT CShaderLightingInstancing::Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice	= pGraphicDevice;
	m_pCommandList		= pCommandList;

	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	m_uiPipelineStateCnt = (_uint)(m_vecPipelineState.size());
	m_vecTotalInstanceCnt.resize(m_uiPipelineStateCnt);

	SetUp_Instancing();
	SetUp_VIBuffer();

	return S_OK;
}

void CShaderLightingInstancing::SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice)
{
	for (_uint i = 0; i < (_uint)m_vecCB_ShaderLighting.size(); ++i)
	{
		_uint iCnt = m_vecTotalInstanceCnt[i];
		if (0 == iCnt)
			iCnt = 1;

		m_vecCB_ShaderLighting[i] = CUploadBuffer<CB_SHADER_LIGHTING>::Create(pGraphicDevice, iCnt, false);
	}
}

HRESULT CShaderLightingInstancing::SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> pvecTargetTexture)
{
	if (m_bIsSetUpDescruptorHeap)
		return S_OK;

	/*__________________________________________________________________________________________________________
	[ SRV 서술자 힙 ]
	- 텍스처 자원을 성공적으로 생성했다면, 그 다음으로 해야할 일은 그에 대한 SRV 서술자를 생성하는 것이다. 
	그래야 자원을 셰이더 프로그램이 사용할 루트 서명 매개변수 슬롯에 설정할 수 있다.
	____________________________________________________________________________________________________________*/
	m_uiTexSize = _uint(pvecTargetTexture.size());

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc	= {};
	SRV_HeapDesc.NumDescriptors	= m_uiTexSize;	// 텍스처의 개수 만큼 설정.
	SRV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc, 
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	/*__________________________________________________________________________________________________________
	[ SRV 서술자 ]
	- SRV 힙을 생성한 다음에는 실제 서술자들을 생성해야 한다.
	____________________________________________________________________________________________________________*/
	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (_uint i = 0; i < m_uiTexSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format							= pvecTargetTexture[i]->GetDesc().Format;
		SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip		= 0;
		SRV_Desc.Texture2D.MipLevels			= pvecTargetTexture[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(pvecTargetTexture[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	}

	m_bIsSetUpDescruptorHeap = true;

	return S_OK;
}

void CShaderLightingInstancing::Resize_ConstantBuffer(ID3D12Device* pGraphicDevice)
{
	for (_uint i = 0; i < (_uint)m_vecCB_ShaderLighting.size(); ++i)
	{
		if (nullptr == m_vecCB_ShaderLighting[i])
			continue;
		if (m_vecCB_ShaderLighting[i]->GetElementCount() < m_vecTotalInstanceCnt[i])
		{
			_uint iNewSize = (_uint)((_float)m_vecTotalInstanceCnt[i] * 1.25f);

			Safe_Delete(m_vecCB_ShaderLighting[i]);
			m_vecCB_ShaderLighting[i] = CUploadBuffer<CB_SHADER_LIGHTING>::Create(pGraphicDevice, iNewSize, false);
		}
	}
}

void CShaderLightingInstancing::Reset_Instance()
{
	for (auto& tInstancingDesc : m_vecInstancing)
		tInstancingDesc.iInstanceCount = 0;
}

void CShaderLightingInstancing::Reset_InstancingConstantBuffer()
{
	for (auto& uiTotalInstanceCnt : m_vecTotalInstanceCnt)
		uiTotalInstanceCnt = 0;

	for (auto& pUploadBuffer : m_vecCB_ShaderLighting)
	{
		Safe_Delete(pUploadBuffer);
	}
}

void CShaderLightingInstancing::Render_Instance()
{
	/*__________________________________________________________________________________________________________
	[ PipelineStatePass 별로 Rendering 수행 ]
	____________________________________________________________________________________________________________*/
	for (_uint iPipelineStatePass = 0; iPipelineStatePass < (_uint)m_vecInstancing.size(); ++iPipelineStatePass)
	{
		if (0 == m_vecInstancing[iPipelineStatePass].iInstanceCount)
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
		[ SRV를 루트 서술자에 묶는다 ]
		____________________________________________________________________________________________________________*/
		ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_pTexDescriptorHeap };
		m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


		CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		SRV_TexNormalDescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index
													   SRV_TexNormalDescriptorHandle);

		CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		SRV_TexSpecularDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index
													   SRV_TexSpecularDescriptorHandle);

		CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDepthDescriptorHandle(m_pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		SRV_TexDepthDescriptorHandle.Offset(3, m_uiCBV_SRV_UAV_DescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index
													   SRV_TexDepthDescriptorHandle);

		/*__________________________________________________________________________________________________________
		[ SRV, CBV를 루트 서술자에 묶는다 ]
		____________________________________________________________________________________________________________*/
		m_pCommandList->SetGraphicsRootShaderResourceView(3,	// RootParameter Index
			m_vecCB_ShaderLighting[iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

		/*__________________________________________________________________________________________________________
		[ Render Buffer ]
		____________________________________________________________________________________________________________*/
		m_pCommandList->IASetVertexBuffers(0, 1, &m_pBufferCom->Get_VertexBufferView());
		m_pCommandList->IASetIndexBuffer(&m_pBufferCom->Get_IndexBufferView());
		m_pCommandList->IASetPrimitiveTopology(m_pBufferCom->Get_PrimitiveTopology());

		m_pCommandList->DrawIndexedInstanced(m_pBufferCom->Get_SubMeshGeometry().uiIndexCount,
											 m_vecInstancing[iPipelineStatePass].iInstanceCount,
											 m_pBufferCom->Get_SubMeshGeometry().uiStartIndexLocation,
											 m_pBufferCom->Get_SubMeshGeometry().iBaseVertexLocation,
											 0);
	}
}

void CShaderLightingInstancing::SetUp_Instancing()
{
	for (_uint i = 0; i < m_uiPipelineStateCnt; ++i)
		m_vecInstancing.push_back(INSTANCING_DESC());

	m_vecCB_ShaderLighting = vector<CUploadBuffer<CB_SHADER_LIGHTING>*>();
	m_vecCB_ShaderLighting.resize(m_uiPipelineStateCnt);
}

void CShaderLightingInstancing::SetUp_VIBuffer()
{
	m_pBufferCom = static_cast<CScreenTex*>(CComponentMgr::Get_Instance()->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
}

HRESULT CShaderLightingInstancing::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV를 담는 서술자 테이블을 생성 ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[3];
	
	// Tex Normal
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  0,								// 셰이더 인수들의 기준 레지스터 번호. (register t0)
					  0);								// 레지스터 공간.

	// Tex Specular
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  1,								// 셰이더 인수들의 기준 레지스터 번호. (register t1)
					  0);								// 레지스터 공간.

	// Tex Depth
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
					  1,								// 서술자의 개수 - Texture2D의 개수.
					  2,								// 셰이더 인수들의 기준 레지스터 번호. (register t2)
					  0);								// 레지스터 공간.

	/*__________________________________________________________________________________________________________
	- 루트 매개변수는 테이블이거나, 루트 서술자 또는 루트 상수이다.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[4];

	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[3].InitAsShaderResourceView(0, 1);	// register b0.

	auto StaticSamplers = Get_StaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),		// 루트 파라미터 개수. (CBV 1, SRV 3 : 총 4개)
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

HRESULT CShaderLightingInstancing::Create_PipelineState()
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
	- "PS_DIRECTION"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 2;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Shade Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_DIRECTION");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE, 
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);	// LightTarget의 Depth는 false

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 1번 PipelineState Pass ]
	- "VS_MAIN"
	- "PS_POINT"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
	- Z Write	(X)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 2;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Shade Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_MAIN", "PS_POINT");
	PipelineStateDesc.InputLayout			= { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS					= { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS					= { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState			= Create_BlendState(true,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE,
																D3D12_BLEND_OP_ADD,
																D3D12_BLEND_ONE,
																D3D12_BLEND_ONE, 
																D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState		= CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(false);	// LightTarget의 Depth는 false

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderLightingInstancing::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderLightingInstancing.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderLightingInstancing.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return vecInputLayout;
}


void CShaderLightingInstancing::Free()
{
	CShader::Free();

	m_vecInstancing.clear();
	m_vecInstancing.shrink_to_fit();

	for (auto& pUploadBuffer : m_vecCB_ShaderLighting)
		Safe_Delete(pUploadBuffer);

	m_vecCB_ShaderLighting.clear();
	m_vecCB_ShaderLighting.shrink_to_fit();

	Safe_Release(m_pBufferCom);
}