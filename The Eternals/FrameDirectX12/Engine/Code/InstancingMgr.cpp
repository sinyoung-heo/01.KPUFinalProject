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

CUploadBuffer<CB_SKINNING_MATRIX>* CInstancingMgr::Get_UploadBuffer_SkinningMatrix(const _uint& iContextIdx, 
																				   wstring wstrMeshTag,
																				   const _uint& uiPipelineStatepass)
{
	auto& iter_find = m_mapCB_SkinningMatrix[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapCB_SkinningMatrix[iContextIdx].end())
		return iter_find->second[uiPipelineStatepass];

	return nullptr;
}

HRESULT CInstancingMgr::SetUp_ShaderComponent()
{
	m_pShaderMesh = static_cast<CShaderMesh*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderMesh", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderMesh, E_FAIL);

	m_pShaderShadow = static_cast<CShaderShadow*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderShadow", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderShadow, E_FAIL);

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

			m_mapCB_SkinningMatrix[i][wstrMeshTag] = vector<CUploadBuffer<CB_SKINNING_MATRIX>*>();
			m_mapCB_SkinningMatrix[i][wstrMeshTag].resize(m_uiMeshPipelineStateCnt);
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
				pair.second[j]	= CUploadBuffer<CB_SHADER_MESH>::Create(pGraphicDevice, iInstanceCnt / 4 + 1);
			}

		}

		for (auto& pair : m_mapCB_SkinningMatrix[i])
		{
			for (_uint j = 0; j < m_uiMeshPipelineStateCnt; ++j)
			{
				iInstanceCnt	= (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());

				CVIMesh* pVIMesh		= static_cast<CMesh*>(CComponentMgr::Get_Instance()->Get_Component(pair.first, ID_STATIC))->Get_VIMesh();
				_uint uiSubsetMeshSize	= (_uint)(pVIMesh->Get_SubMeshGeometry().size());
				pair.second[j]			= CUploadBuffer<CB_SKINNING_MATRIX>::Create(pGraphicDevice, uiSubsetMeshSize * (iInstanceCnt/ 4 + 1));
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

void CInstancingMgr::Add_TexInstancingDesc(wstring wstrMeshTag, const _uint& iPipelineStateIdx)
{
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

void CInstancingMgr::Reset_TexInstancingDesc()
{
}

void CInstancingMgr::Render_MeshInstance(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	// first - MeshTag, second - vector<INSTANCING_DESC>
	for (auto& pair : m_mapMeshInstancing[iContextIdx])
	{
		wstring wstrMeshTag = pair.first;

		for (_uint iPipelineStatePass = 0; iPipelineStatePass < pair.second.size(); ++iPipelineStatePass)
		{
			if (!pair.second[iPipelineStatePass].iInstanceCount)
				continue;

			// SetPipelineState
			CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pShaderMesh->Get_PipelineState(iPipelineStatePass), iContextIdx);

			// Set RootSignature.
			pCommandList->SetGraphicsRootSignature(m_pShaderMesh->Get_RootSignature());

			// Set DescriptorHeap.
			ID3D12DescriptorHeap* pDescriptorHeaps[] = { CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrMeshTag) };
			pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


			CVIMesh* pVIMesh = static_cast<CMesh*>(CComponentMgr::Get_Instance()->Get_Component(wstrMeshTag, ID_STATIC))->Get_VIMesh();
			_uint uiSubsetMeshSize = (_uint)(pVIMesh->Get_SubMeshGeometry().size());
			for (_uint iSubMeshIdx = 0; iSubMeshIdx < uiSubsetMeshSize; ++iSubMeshIdx)
			{
				/*__________________________________________________________________________________________________________
				[ SRV를 루트 서술자에 묶는다 ]
				____________________________________________________________________________________________________________*/
				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());
				SRV_TexDiffuseDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_DIFFUSE), m_pShaderMesh->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
															 SRV_TexDiffuseDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());
				SRV_TexNormalDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_NORMAL), m_pShaderMesh->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
															 SRV_TexNormalDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());
				SRV_TexSpecularDescriptorHandle.Offset(iSubMeshIdx + (uiSubsetMeshSize * TEX_SPECULAR), m_pShaderMesh->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
															 SRV_TexSpecularDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());
				SRV_TexShadowDepthDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END, m_pShaderMesh->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
															 SRV_TexShadowDepthDescriptorHandle);

				CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(pDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart());
				SRV_TexEmissiveDescriptorHandle.Offset(uiSubsetMeshSize * TEXTURE_END + 1, m_pShaderMesh->Get_CBV_SRV_UAV_DescriptorSize());
				pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
															 SRV_TexEmissiveDescriptorHandle);
				/*__________________________________________________________________________________________________________
				[ CBV를 루트 서술자에 묶는다 ]
				____________________________________________________________________________________________________________*/
				pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
																m_pShaderMesh->Get_UploadBuffer_CameraProjMatrix()->Resource()->GetGPUVirtualAddress());

				pCommandList->SetGraphicsRootShaderResourceView(6,	// RootParameter Index
					m_mapCB_ShaderMesh[iContextIdx][wstrMeshTag][iPipelineStatePass]->Resource()->GetGPUVirtualAddress());

				pCommandList->SetGraphicsRootShaderResourceView(7,	// RootParameter Index
					m_mapCB_SkinningMatrix[iContextIdx][wstrMeshTag][iPipelineStatePass]->Resource()->GetGPUVirtualAddress() +
					m_mapCB_SkinningMatrix[iContextIdx][wstrMeshTag][iPipelineStatePass]->GetElementByteSize() * iSubMeshIdx);


				// Render Instance
				pCommandList->IASetVertexBuffers(0, 1, &pVIMesh->Get_VertexBufferView(iSubMeshIdx));
				pCommandList->IASetIndexBuffer(&pVIMesh->Get_IndexBufferView(iSubMeshIdx));

				pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	Safe_Release(m_pShaderMesh);
	Safe_Release(m_pShaderShadow);

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

		for (auto& pair : m_mapCB_SkinningMatrix[i])
		{
			for (_uint j = 0; j < m_uiMeshPipelineStateCnt; ++j)
				Safe_Delete(pair.second[j]);
		}
	}


}
