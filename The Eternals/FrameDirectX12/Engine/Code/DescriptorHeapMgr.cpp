#include "DescriptorHeapMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CDescriptorHeapMgr)

HRESULT CDescriptorHeapMgr::Add_DescriptorHeap(wstring wstrHeapTag, ID3D12DescriptorHeap* pDescriptorHeap)
{
	/*__________________________________________________________________________________________________________
	Tag값이 없는 경우에만 Map에 삽입.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapResourceDescriptorHeap.find(wstrHeapTag);
	if (iter_find == m_mapResourceDescriptorHeap.end())
	{
		m_mapResourceDescriptorHeap.emplace(wstrHeapTag, pDescriptorHeap);

		return S_OK;
	}

	return E_FAIL;
}

ID3D12DescriptorHeap* CDescriptorHeapMgr::Find_DescriptorHeap(wstring wstrHeapTag)
{
	auto iter_find = m_mapResourceDescriptorHeap.find(wstrHeapTag);
	if (iter_find != m_mapResourceDescriptorHeap.end())
		return iter_find->second;

	return nullptr;
}

void CDescriptorHeapMgr::Free()
{
	m_mapResourceDescriptorHeap.clear();
}
