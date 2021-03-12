#include "ShaderMeshInstancing.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "DescriptorHeapMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CShaderMeshInstancing)

CUploadBuffer<CB_SHADER_MESH>* CShaderMeshInstancing::Get_UploadBuffer_ShaderMesh(const _uint& iContextIdx, 
																				  wstring wstrMeshTag, 
																				  const _uint& uiPipelineStatepass)
{
	auto& iter_find = m_mapCB_ShaderMesh[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapCB_ShaderMesh[iContextIdx].end())
		return iter_find->second[uiPipelineStatepass];

	return nullptr;
}

HRESULT CShaderMeshInstancing::Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_pGraphicDevice	= pGraphicDevice;
	m_pCommandList		= pCommandList;

	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	m_uiPipelineStateCnt = (_uint)(m_vecPipelineState.size());

	return S_OK;
}

void CShaderMeshInstancing::SetUp_Instancing(wstring wstrMeshTag)
{
	for (auto& mapInstancing : m_mapInstancing)
	{
		auto iter_find = mapInstancing.find(wstrMeshTag);

		if (iter_find == mapInstancing.end())
		{
			mapInstancing[wstrMeshTag] = vector<INSTANCING_DESC>();
			mapInstancing[wstrMeshTag].resize(m_uiPipelineStateCnt);

			for (_uint i = 0; i < m_uiPipelineStateCnt; ++i)
				mapInstancing[wstrMeshTag].push_back(INSTANCING_DESC());
		}
	}

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		auto iter_find = m_mapCB_ShaderMesh[i].find(wstrMeshTag);

		if (iter_find == m_mapCB_ShaderMesh[i].end())
		{
			m_mapCB_ShaderMesh[i][wstrMeshTag] = vector<CUploadBuffer<CB_SHADER_MESH>*>();
			m_mapCB_ShaderMesh[i][wstrMeshTag].resize(m_uiPipelineStateCnt);
		}
	}
}

void CShaderMeshInstancing::SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice)
{
	_uint iInstanceCnt = 0;

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderMesh[i])
		{
			for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
			{
				iInstanceCnt = (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());
				pair.second[j] = CUploadBuffer<CB_SHADER_MESH>::Create(pGraphicDevice, iInstanceCnt / 4 + 1, false);
			}

		}
	}
}

void CShaderMeshInstancing::Add_Instance(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStateIdx)
{
	auto iter_find = m_mapInstancing[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapInstancing[iContextIdx].end())
		++(m_mapInstancing[iContextIdx][wstrMeshTag][iPipelineStateIdx].iInstanceCount);
}

void CShaderMeshInstancing::Reset_Instance()
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

void CShaderMeshInstancing::Reset_InstancingContainer()
{
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
}

void CShaderMeshInstancing::Reset_InstancingConstantBuffer()
{
	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderMesh[i])
		{
			for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}
}

void CShaderMeshInstancing::Render_Instance(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	first	: MeshTag
	second	: vector<INSTANCING_DESC>
	____________________________________________________________________________________________________________*/
	for (auto& pair : m_mapInstancing[iContextIdx])
	{
		wstring wstrMeshTag = pair.first;

		/*__________________________________________________________________________________________________________
		[ PipelineStatePass ���� Rendering ���� ]
		____________________________________________________________________________________________________________*/
		for (_uint iPipelineStatePass = 0; iPipelineStatePass < pair.second.size(); ++iPipelineStatePass)
		{
			if (!pair.second[iPipelineStatePass].iInstanceCount)
				continue;

			/*__________________________________________________________________________________________________________
			[ Set RootSignature ]
			____________________________________________________________________________________________________________*/
			pCommandList->SetGraphicsRootSignature(m_pRootSignature);

			/*__________________________________________________________________________________________________________
			[ Set PipelineState ]
			____________________________________________________________________________________________________________*/
			Set_PipelineStatePass(iPipelineStatePass);
			CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

			/*__________________________________________________________________________________________________________
			[ Set DescriptorHeap ]
			____________________________________________________________________________________________________________*/
			ID3D12DescriptorHeap* pTexDescriptorHeap = CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstring(wstrMeshTag) + L".X");
			ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
			pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


			/*__________________________________________________________________________________________________________
			[ SRV, CBV�� ��Ʈ �����ڿ� ���´� ]
			____________________________________________________________________________________________________________*/
			pCommandList->SetGraphicsRootShaderResourceView(5,	// RootParameter Index
				m_mapCB_ShaderMesh[iContextIdx][wstrMeshTag][iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

			pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
				m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

			/*__________________________________________________________________________________________________________
			[ Render Buffer ]
			____________________________________________________________________________________________________________*/
			CMesh*		pMesh				= static_cast<CMesh*>(CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", wstrMeshTag)->Get_Component(L"Com_Mesh", ID_STATIC));
			CVIMesh*	pVIMesh				= pMesh->Get_VIMesh();
			_uint		uiSubsetMeshSize	= (_uint)(pVIMesh->Get_SubMeshGeometry().size());

			for (_uint iSubMeshIdx = 0; iSubMeshIdx < uiSubsetMeshSize; ++iSubMeshIdx)
			{
				/*__________________________________________________________________________________________________________
				[ SRV�� ��Ʈ �����ڿ� ���´� ]
				____________________________________________________________________________________________________________*/
				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexDiffuseDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_DIFFUSE), m_uiCBV_SRV_UAV_DescriptorSize);
				pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
															 SRV_TexDiffuseDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexNormalDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_NORMAL), m_uiCBV_SRV_UAV_DescriptorSize);
				pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
															 SRV_TexNormalDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexSpecularDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_SPECULAR), m_uiCBV_SRV_UAV_DescriptorSize);
				pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
															 SRV_TexSpecularDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexShadowDepthDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END, m_uiCBV_SRV_UAV_DescriptorSize);
				pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
															 SRV_TexShadowDepthDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexEmissiveDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END + 1, m_uiCBV_SRV_UAV_DescriptorSize);
				pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
															 SRV_TexEmissiveDescriptorHandle);

				pCommandList->IASetVertexBuffers(0, 1, &pVIMesh->Get_VertexBufferView(iSubMeshIdx));
				pCommandList->IASetIndexBuffer(&pVIMesh->Get_IndexBufferView(iSubMeshIdx));
				pCommandList->IASetPrimitiveTopology(pVIMesh->Get_PrimitiveTopology());

				pCommandList->DrawIndexedInstanced(pVIMesh->Get_SubMeshGeometry()[iSubMeshIdx].uiIndexCount,
												   pair.second[iPipelineStatePass].iInstanceCount,
												   pVIMesh->Get_SubMeshGeometry()[iSubMeshIdx].uiStartIndexLocation,
												   pVIMesh->Get_SubMeshGeometry()[iSubMeshIdx].iBaseVertexLocation,
												   0);
			}
		}
	}
}


HRESULT CShaderMeshInstancing::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV�� ��� ������ ���̺��� ���� ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[6];

	// TexDiffuse
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  0,								// ���̴� �μ����� ���� �������� ��ȣ. (register t0)
					  0);								// �������� ����.

	// TexNormal
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  1,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
					  0);								// �������� ����.
	// Specular
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  2,								// ���̴� �μ����� ���� �������� ��ȣ. (register t2)
					  0);								// �������� ����.

	// ShadowDepth
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  3,								// ���̴� �μ����� ���� �������� ��ȣ. (register t3)
					  0);								// �������� ����.

	// Dissolve
	SRV_Table[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
					  1,								// �������� ���� - Texture2D�� ����.
					  4,								// ���̴� �μ����� ���� �������� ��ȣ. (register t4)
					  0);								// �������� ����.
	/*__________________________________________________________________________________________________________
	- ��Ʈ �Ű������� ���̺��̰ų�, ��Ʈ ������ �Ǵ� ��Ʈ ����̴�.
	____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[7];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[5].InitAsShaderResourceView(0, 1);	// register t0, space1.
	RootParameter[6].InitAsConstantBufferView(0);		// register b0.

	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),		// ��Ʈ �Ķ���� ����.
												  RootParameter,
												  (UINT)StaticSamplers.size(),	// ���÷� ����.
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

HRESULT CShaderMeshInstancing::Create_PipelineState()
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
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 6;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

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

	/*__________________________________________________________________________________________________________
	[ 1�� PipelineState Pass ]
	- "VS_SHADOW_MAIN"
	- "PS_SHADOW_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 6;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_SHADOW_MAIN", "PS_SHADOW_MAIN");
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
	[ 2�� PipelineState Pass ]
	- "VS_SHADOW_MAIN"
	- "PS_TERRAIN_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature		= m_pRootSignature;
	PipelineStateDesc.SampleMask			= UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets		= 6;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3]			= DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5]			= DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

	PipelineStateDesc.SampleDesc.Count		= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality	= CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout							= Create_InputLayout("VS_SHADOW_MAIN", "PS_TERRAIN_MAIN");
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

vector<D3D12_INPUT_ELEMENT_DESC> CShaderMeshInstancing::Create_InputLayout(string VS_EntryPoint, string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMeshInstancing.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMeshInstancing.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec2), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderMeshInstancing::Create_BlendState(const _bool& bIsBlendEnable, 
														  const D3D12_BLEND& SrcBlend, 
														  const D3D12_BLEND& DstBlend, 
														  const D3D12_BLEND_OP& BlendOp, 
														  const D3D12_BLEND& SrcBlendAlpha,
														  const D3D12_BLEND& DstBlendAlpha, 
														  const D3D12_BLEND_OP& BlendOpAlpha)
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

void CShaderMeshInstancing::Free()
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

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderMesh[i])
		{
			for (_uint j = 0; j < m_uiPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}
}
