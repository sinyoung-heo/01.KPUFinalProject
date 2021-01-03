#include "ComponentMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CComponentMgr)

CComponentMgr::CComponentMgr()
{

}



CComponent * CComponentMgr::Get_Component(wstring wstrPrototypeTag, COMPONENTID eID)
{
	CComponent* pComponent = Find_Component(wstrPrototypeTag, eID);
	NULL_CHECK_RETURN(pComponent, nullptr);

	return pComponent;
}

HRESULT CComponentMgr::Add_ComponentPrototype(wstring wstrPrototypeTag, const COMPONENTID & eID, CComponent* pComponent)
{
	if (eID > COMPONENTID::ID_END)
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Tag값이 존재하면 map에 넣지 않는다.
	____________________________________________________________________________________________________________*/
	auto iter = m_mapComponent[eID].find(wstrPrototypeTag);
	if (iter != m_mapComponent[eID].end())
		return E_FAIL;

	m_mapComponent[eID].emplace(wstrPrototypeTag, pComponent);

	return S_OK;
}

CComponent * CComponentMgr::Clone_Component(wstring wstrPrototypeTag, const COMPONENTID & eID)
{
	if (eID > COMPONENTID::ID_END)
		return nullptr;

	CComponent* pComponent = Find_Component(wstrPrototypeTag, eID);
	NULL_CHECK_RETURN(pComponent, nullptr);

	return pComponent->Clone();
}

void CComponentMgr::Release_UploadBuffer()
{
	for (_int i = 0; i < COMPONENTID::ID_END; ++i)
	{
		for (auto& pComponent : m_mapComponent[i])
			pComponent.second->Release_UploadBuffer();
	}

}

CComponent * CComponentMgr::Find_Component(wstring wstrPrototypeTag, const COMPONENTID & eID)
{
	auto iter_find = m_mapComponent[eID].find(wstrPrototypeTag);

	if (iter_find == m_mapComponent[eID].end())
		return nullptr;

	return iter_find->second;
}

void CComponentMgr::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy ComponentMgr");
#endif

	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}
}
