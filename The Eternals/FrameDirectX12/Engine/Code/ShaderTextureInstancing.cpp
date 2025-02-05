#include "ShaderTextureInstancing.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "DescriptorHeapMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CShaderTextureInstancing)

CUploadBuffer<CB_SHADER_TEXTURE>* CShaderTextureInstancing::Get_UploadBuffer_ShaderTexture(wstring wstrTextureTag,
																						   const INSTANCE& eInstance,
																						   const _uint& uiPipelineStatepass)
{
	auto& iter_find = m_mapCB_ShaderTexture[eInstance].find(wstrTextureTag);

	if (iter_find != m_mapCB_ShaderTexture[eInstance].end())
		return iter_find->second[uiPipelineStatepass];

	return nullptr;
}

HRESULT CShaderTextureInstancing::Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice	= pGraphicDevice;
	m_pCommandList		= pCommandList;

	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	m_uiPipelineStateCnt = (_uint)(m_vecPipelineState.size());

	return S_OK;
}

void CShaderTextureInstancing::SetUp_Instancing(const INSTANCE& eInstance, wstring wstrTextureTag)
{
	auto iter_find = m_mapInstancing[eInstance].find(wstrTextureTag);

	if (iter_find == m_mapInstancing[eInstance].end())
	{
		m_mapInstancing[eInstance][wstrTextureTag] = vector<INSTANCING_DESC>();
		m_mapInstancing[eInstance][wstrTextureTag].resize(m_uiPipelineStateCnt);

		for (_uint i = 0; i < m_uiPipelineStateCnt; ++i)
			m_mapInstancing[eInstance][wstrTextureTag].push_back(INSTANCING_DESC());


		m_mapCB_ShaderTexture[eInstance][wstrTextureTag] = vector<CUploadBuffer<CB_SHADER_TEXTURE>*>();
		m_mapCB_ShaderTexture[eInstance][wstrTextureTag].resize(m_uiPipelineStateCnt);
	}
}

void CShaderTextureInstancing::SetUp_ConstantBuffer(const INSTANCE& eInstance, ID3D12Device* pGraphicDevice)
{
	_uint iInstanceCnt = 0;

	for (auto& pair : m_mapCB_ShaderTexture[eInstance])
	{
		for (_uint i = 0; i < m_uiPipelineStateCnt; ++i)
		{
			iInstanceCnt = (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());

			pair.second[i] = CUploadBuffer<CB_SHADER_TEXTURE>::Create(pGraphicDevice, iInstanceCnt, false);
		}

	}
}

void CShaderTextureInstancing::Add_Instance(wstring wstrTextureTag, const INSTANCE& eInstance, const _uint& iPipelineStateIdx)
{
	auto iter_find = m_mapInstancing[eInstance].find(wstrTextureTag);

	if (iter_find != m_mapInstancing[eInstance].end())
		++(m_mapInstancing[eInstance][wstrTextureTag][iPipelineStateIdx].iInstanceCount);
}

void CShaderTextureInstancing::Reset_Instance()
{
	for (auto& mapInstancing : m_mapInstancing)
	{
		for (auto& pair : mapInstancing)
		{
			for (auto& tInstancingDesc : pair.second)
				tInstancingDesc.iInstanceCount = 0;
		}
	}
}

void CShaderTextureInstancing::Reset_InstancingContainer()
{
	// Texture Instancing
	for (auto& mapInstancing : m_mapInstancing)
	{
		for (auto& pair : mapInstancing)
		{
			pair.second.clear();
			pair.second.shrink_to_fit();
		}

		mapInstancing.clear();
	}
}

void CShaderTextureInstancing::Reset_InstancingConstantBuffer()
{
	for (_uint i = 0; i < INSTANCE::INSTANCE_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderTexture[i])
		{
			for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}
}

void CShaderTextureInstancing::Render_Instance(const INSTANCE& eInstance)
{
	/*__________________________________________________________________________________________________________
	first	: TextureTag
	second	: vector<INSTANCING_DESC>
	____________________________________________________________________________________________________________*/
	for (auto& pair : m_mapInstancing[eInstance])
	{
		wstring wstrTextureTag = pair.first;

		/*__________________________________________________________________________________________________________
		[ PipelineStatePass 별로 Rendering 수행 ]
		____________________________________________________________________________________________________________*/
		for (_uint iPipelineStatePass = 0; iPipelineStatePass < pair.second.size(); ++iPipelineStatePass)
		{
			if (!pair.second[iPipelineStatePass].iInstanceCount)
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
			[ Set DescriptorHeap ]
			____________________________________________________________________________________________________________*/
			ID3D12DescriptorHeap* pTexDescriptorHeap = CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
			ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
			m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

			CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			SRV_DescriptorHandle.Offset(0, m_uiCBV_SRV_UAV_DescriptorSize);
			m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index
														   SRV_DescriptorHandle);

			/*__________________________________________________________________________________________________________
			[ SRV, CBV를 루트 서술자에 묶는다 ]
			____________________________________________________________________________________________________________*/
			m_pCommandList->SetGraphicsRootShaderResourceView(1,	// RootParameter Index
				m_mapCB_ShaderTexture[eInstance][wstrTextureTag][iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

			m_pCommandList->SetGraphicsRootConstantBufferView(2,	// RootParameter Index
				m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

			/*__________________________________________________________________________________________________________
			[ Render Buffer ]
			____________________________________________________________________________________________________________*/
			CVIBuffer* pVIBuffer = nullptr;
			pVIBuffer = static_cast<CVIBuffer*>(CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", wstrTextureTag)->Get_Component(L"Com_Buffer", ID_STATIC));


			m_pCommandList->IASetVertexBuffers(0, 1, &pVIBuffer->Get_VertexBufferView());
			m_pCommandList->IASetIndexBuffer(&pVIBuffer->Get_IndexBufferView());
			m_pCommandList->IASetPrimitiveTopology(pVIBuffer->Get_PrimitiveTopology());

			m_pCommandList->DrawIndexedInstanced(pVIBuffer->Get_SubMeshGeometry().uiIndexCount,
												 pair.second[iPipelineStatePass].iInstanceCount,
												 pVIBuffer->Get_SubMeshGeometry().uiStartIndexLocation,
												 pVIBuffer->Get_SubMeshGeometry().iBaseVertexLocation,
												 0);
		}
	}


}

HRESULT CShaderTextureInstancing::Create_RootSignature()
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
	RootParameter[1].InitAsShaderResourceView(0, 1);	// register t0, space1.
	RootParameter[2].InitAsConstantBufferView(0);		// register b0.

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

HRESULT CShaderTextureInstancing::Create_PipelineState()
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
	- "VS_TEXTURE_SPRITE"
	- "PS_TEXTURE_SPRITE"
	- FILL_MODE_SOLID
	- CULL_MODE_NONE
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
	PipelineStateDesc.DepthStencilState		= CShader::Create_DepthStencilState(true);

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderTextureInstancing::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderTextureInstancing.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderTextureInstancing.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderTextureInstancing::Create_BlendState(const _bool& bIsBlendEnable, 
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

void CShaderTextureInstancing::Free()
{
	CShader::Free();

	// Mesh Instancing
	for (auto& mapInstancing : m_mapInstancing)
	{
		for (auto& pair : mapInstancing)
		{
			pair.second.clear();
			pair.second.shrink_to_fit();
		}

		mapInstancing.clear();
	}

	for (_uint i = 0; i < INSTANCE::INSTANCE_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderTexture[i])
		{
			for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}

}
