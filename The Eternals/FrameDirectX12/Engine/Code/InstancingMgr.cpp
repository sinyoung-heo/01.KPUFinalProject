#include "InstancingMgr.h"
#include "ObjectMgr.h"
#include "ComponentMgr.h"
#include "DescriptorHeapMgr.h"
#include "Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CInstancingMgr)

CInstancingMgr::CInstancingMgr()
{
}

CUploadBuffer<CB_SHADER_MESH>* CInstancingMgr::Get_UploadBuffer_ShaderMesh(const _uint& iContextIdx, 
																		   wstring wstrMeshTag,
																		   const _uint& uiPipelineStatepass)
{
	auto& iter_find = m_mapCB_ShaderMesh[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapCB_ShaderMesh[iContextIdx].end())
		return iter_find->second[uiPipelineStatepass];

	return nullptr;
}

HRESULT CInstancingMgr::SetUp_ShaderComponent()
{
	m_pShaderMeshInstancing = static_cast<CShaderMeshInstancing*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderMeshInstancing", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderMeshInstancing, E_FAIL);

	return S_OK;
}

void CInstancingMgr::SetUp_MeshInstancing(wstring wstrMeshTag)
{
	for (auto& mapMeshInstancing : m_mapMeshInstancing)
	{
		auto iter_find = mapMeshInstancing.find(wstrMeshTag);

		if (iter_find == mapMeshInstancing.end())
		{
			mapMeshInstancing[wstrMeshTag] = vector<INSTANCING_DESC>();
			mapMeshInstancing[wstrMeshTag].resize(m_uiMeshPipelineStateCnt);

			for (_uint i = 0; i < m_uiMeshPipelineStateCnt; ++i)
				mapMeshInstancing[wstrMeshTag].push_back(INSTANCING_DESC());
		}
	}

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		auto iter_find = m_mapCB_ShaderMesh[i].find(wstrMeshTag);

		if (iter_find == m_mapCB_ShaderMesh[i].end())
		{
			m_mapCB_ShaderMesh[i][wstrMeshTag] = vector<CUploadBuffer<CB_SHADER_MESH>*>();
			m_mapCB_ShaderMesh[i][wstrMeshTag].resize(m_uiMeshPipelineStateCnt);
		}
	}
}

void CInstancingMgr::SetUp_MeshConstantBuffer(ID3D12Device* pGraphicDevice)
{
	_uint iInstanceCnt = 0;

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderMesh[i])
		{
			for (_uint j = 0; j < m_uiMeshPipelineStateCnt; ++j)
			{
				iInstanceCnt	= (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());
				pair.second[j]	= CUploadBuffer<CB_SHADER_MESH>::Create(pGraphicDevice, iInstanceCnt / 4 + 1, false);
			}

		}
	}

}

void CInstancingMgr::Add_MeshInstance(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStateIdx)
{
	auto iter_find = m_mapMeshInstancing[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapMeshInstancing[iContextIdx].end())
		++(m_mapMeshInstancing[iContextIdx][wstrMeshTag][iPipelineStateIdx].iInstanceCount);
}

void CInstancingMgr::Reset_MeshInstancing()
{
	for (auto& mapMeshInstancing : m_mapMeshInstancing)
	{
		for (auto& pair : mapMeshInstancing)
		{
			for (auto& tInstancingDesc : pair.second)
				tInstancingDesc.iInstanceCount = 0;
		}
	}
}

void CInstancingMgr::Render_MeshInstance(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	first	: MeshTag
	second	: vector<INSTANCING_DESC>
	____________________________________________________________________________________________________________*/
	for (auto& pair : m_mapMeshInstancing[iContextIdx])
	{
		wstring wstrMeshTag = pair.first;

		/*__________________________________________________________________________________________________________
		[ PipelineStatePass 별로 Rendering 수행 ]
		____________________________________________________________________________________________________________*/
		for (_uint iPipelineStatePass = 0; iPipelineStatePass < pair.second.size(); ++iPipelineStatePass)
		{
			if (!pair.second[iPipelineStatePass].iInstanceCount)
				continue;

			// Set RootSignature.
			pCommandList->SetGraphicsRootSignature(m_pShaderMeshInstancing->Get_RootSignature());

			// SetPipelineState
			m_pShaderMeshInstancing->Set_PipelineStatePass(iPipelineStatePass);
			CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pShaderMeshInstancing->Get_PipelineStatePass(), iContextIdx);

			// Set DescriptorHeap.
			ID3D12DescriptorHeap* pTexDescriptorHeap = CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstring(wstrMeshTag) + L".X");
			ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
			pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

			pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
				m_pShaderMeshInstancing->Get_UploadBuffer_CameraProjMatrix()->Resource()->GetGPUVirtualAddress());

			/*__________________________________________________________________________________________________________
			[ Render Mesh ]
			____________________________________________________________________________________________________________*/
			CMesh*		pMesh	= static_cast<CMesh*>(CObjectMgr::Get_Instance()->Get_GameObject(L"Layer_GameObject", wstrMeshTag)->Get_Component(L"Com_Mesh", ID_STATIC));
			CVIMesh*	pVIMesh	= pMesh->Get_VIMesh();
			_uint uiSubsetMeshSize = (_uint)(pVIMesh->Get_SubMeshGeometry().size());

			for (_uint iSubMeshIdx = 0; iSubMeshIdx < uiSubsetMeshSize; ++iSubMeshIdx)
			{
				/*__________________________________________________________________________________________________________
				[ SRV를 루트 서술자에 묶는다 ]
				____________________________________________________________________________________________________________*/
				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexDiffuseDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_DIFFUSE), m_pShaderMeshInstancing->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
															 SRV_TexDiffuseDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexNormalDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_NORMAL), m_pShaderMeshInstancing->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
															 SRV_TexNormalDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexSpecularDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_SPECULAR), m_pShaderMeshInstancing->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
															 SRV_TexSpecularDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexShadowDepthDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END, m_pShaderMeshInstancing->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
															 SRV_TexShadowDepthDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				SRV_TexEmissiveDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END + 1, m_pShaderMeshInstancing->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
															 SRV_TexEmissiveDescriptorHandle);

				/*__________________________________________________________________________________________________________
				[ CBV를 루트 서술자에 묶는다 ]
				____________________________________________________________________________________________________________*/
					pCommandList->SetGraphicsRootShaderResourceView(5,	// RootParameter Index
						m_mapCB_ShaderMesh[iContextIdx][wstrMeshTag][iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

				// Render Instance
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


void CInstancingMgr::Free()
{
	Safe_Release(m_pShaderMeshInstancing);

	for (auto& mapMeshInstancing : m_mapMeshInstancing)
	{
		for (auto& pair : mapMeshInstancing)
		{
			pair.second.clear();
			pair.second.shrink_to_fit();
		}

		mapMeshInstancing.clear();
	}

	for (_uint i = 0; i < CONTEXT::CONTEXT_END; ++i)
	{
		for (auto& pair : m_mapCB_ShaderMesh[i])
		{
			for (_uint j  =0; j < m_uiMeshPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}


}
