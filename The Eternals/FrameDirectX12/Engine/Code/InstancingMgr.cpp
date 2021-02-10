#include "InstancingMgr.h"
#include "ObjectMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CInstancingMgr)

CInstancingMgr::CInstancingMgr()
{
}

CUploadBuffer<CB_SHADER_MESH>* CInstancingMgr::Get_UploadBuffer_ShaderMesh(const _uint& iContextIdx, wstring wstrMeshTag)
{
	auto& iter_find = m_mapCB_ShaderMesh[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapCB_ShaderMesh[iContextIdx].end())
		return iter_find->second;

	return nullptr;
}

CUploadBuffer<CB_SKINNING_MATRIX>* CInstancingMgr::Get_UploadBuffer_SkinningMatrix(const _uint& iContextIdx, wstring wstrMeshTag)
{
	auto& iter_find = m_mapCB_SkinningMatrix[iContextIdx].find(wstrMeshTag);

	if (iter_find != m_mapCB_SkinningMatrix[iContextIdx].end())
		return iter_find->second;

	return nullptr;
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
			m_mapCB_ShaderMesh[i][wstrMeshTag]     = nullptr;
			m_mapCB_SkinningMatrix[i][wstrMeshTag] = nullptr;
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
			iInstanceCnt = (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());
			pair.second	 = CUploadBuffer<CB_SHADER_MESH>::Create(pGraphicDevice, iInstanceCnt / 4 + 1);
		}

		for (auto& pair : m_mapCB_SkinningMatrix[i])
		{
			iInstanceCnt = (_uint)(CObjectMgr::Get_Instance()->Get_OBJLIST(L"Layer_GameObject", pair.first)->size());
			pair.second = CUploadBuffer<CB_SKINNING_MATRIX>::Create(pGraphicDevice, iInstanceCnt/ 4 + 1);
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


void CInstancingMgr::Free()
{
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
			Safe_Delete(pair.second);

		for (auto& pair : m_mapCB_SkinningMatrix[i])
			Safe_Delete(pair.second);
	}


}
