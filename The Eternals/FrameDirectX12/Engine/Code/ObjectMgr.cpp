#include "ObjectMgr.h"
#include "Font.h"

USING(Engine)
IMPLEMENT_SINGLETON(CObjectMgr)

CObjectMgr::CObjectMgr()
{
}


CLayer * CObjectMgr::Get_Layer(wstring wstrLayerTag)
{
	/*__________________________________________________________________________________________________________
	- 1. 찾고자 하는 Layer Tag를 탐색한다
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. 없을 경우 nullptr을 반환한다
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapLayer.end())
		return nullptr;


	return iter_find->second;
}

OBJLIST * CObjectMgr::Get_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- 1. 찾고자 하는 Layer Tag값을 탐색한다
	____________________________________________________________________________________________________________*/
	CLayer* pInstance = Get_Layer(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. 없을 경우, nullptr을 반환한다
	____________________________________________________________________________________________________________*/
	if (pInstance == nullptr)
		return nullptr;


	return pInstance->Get_OBJLIST(wstrObjTag);
}

OBJLIST* CObjectMgr::Get_StaticOBJLIST(wstring wstrObjTag)
{
	auto iter_find = m_mapStaticObject[STAGEID::STAGE_END].find(wstrObjTag);

	if (m_mapStaticObject[STAGEID::STAGE_END].end() == iter_find)
		return nullptr;

	return &(iter_find->second);
}

CGameObject * CObjectMgr::Get_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- 1. 찾고자 하는 Layer Tag값을 탐색한다.
	____________________________________________________________________________________________________________*/
	CLayer* pInstance = Get_Layer(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. 없을 경우, nullptr을 반환한다.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pInstance, nullptr);


	return pInstance->Get_GameObject(wstrObjTag, iIdx);
}

CGameObject* CObjectMgr::Get_StaticObject(wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- 1. map에서 찾고자 하는 OBJLIST의 Key값을 탐색한다.
	____________________________________________________________________________________________________________*/
	OBJLIST* pObjLst = Get_StaticOBJLIST(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- 2. 찾고자 하는 OBJLIST가 없다면 nullptr을 반환.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pObjLst, nullptr);

	/*__________________________________________________________________________________________________________
	- 3. 찾고자하는 GameObject를 OBJLIST에서 탐색.
	____________________________________________________________________________________________________________*/
	if (pObjLst->size() <= (_ulong)iIdx)
		return nullptr;

	OBJITER iter_begin = pObjLst->begin();

	for (_int i = 0; i < iIdx; ++i)
		++iter_begin;

	return (*iter_begin);
}

CGameObject* CObjectMgr::Get_ServerObject(wstring wstrLayerTag, wstring wstrObjTag, int num)
{
	/*__________________________________________________________________________________________________________
	- 1. 찾고자 하는 Layer Tag값을 탐색한다.
	____________________________________________________________________________________________________________*/
	CLayer* pInstance = Get_Layer(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. 없을 경우, nullptr을 반환한다.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pInstance, nullptr);


	return pInstance->Get_ServerObject(wstrObjTag, num);
}

HRESULT CObjectMgr::Add_GameObjectPrototype(wstring wstrPrototypeTag, CGameObject * pGameObject)
{
	if (nullptr != pGameObject)
	{
		/*__________________________________________________________________________________________________________
		Prototype Tag값이 없을 경우에만 map에 넣어준다.
		____________________________________________________________________________________________________________*/
		auto iter = m_mapObjectPrototype.find(wstrPrototypeTag);

		if (m_mapObjectPrototype.end() != iter)
			return E_FAIL;

		m_mapObjectPrototype.emplace(wstrPrototypeTag, pGameObject);
	}

	return S_OK;
}

CGameObject * CObjectMgr::Clone_GameObjectPrototype(wstring wstrPrototypeTag)
{
	auto iter = m_mapObjectPrototype.find(wstrPrototypeTag);

	if (m_mapObjectPrototype.end() == iter)
		return nullptr;

	return iter->second->Clone_GameObject();
}


HRESULT CObjectMgr::Add_GameObject(wstring wstrLayerTag, wstring wstrObjTag, CGameObject* pGameObject)
{
	/*__________________________________________________________________________________________________________
	- 1. Layer를 탐색한다.
	____________________________________________________________________________________________________________*/
	auto iter_find_layer = m_mapLayer.find(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. 해당 Layer가 없을 경우 종료.
	____________________________________________________________________________________________________________*/
	if (iter_find_layer == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- 3. Layer에 GameObject 추가.
	____________________________________________________________________________________________________________*/
	return iter_find_layer->second->Add_GameObject(wstrObjTag, pGameObject);

	return S_OK;
}

HRESULT CObjectMgr::Add_GameObject(const STAGEID& eID, wstring wstrObjTag, CGameObject* pGameObject)
{
	if (nullptr != pGameObject)
	{
		m_mapStaticObject[eID][wstrObjTag].emplace_back(pGameObject);
		m_mapStaticObject[STAGEID::STAGE_END][wstrObjTag].emplace_back(pGameObject);

		return S_OK;
	}

	return E_FAIL;
}

_int CObjectMgr::Update_ObjectMgr(const _float & fTimeDelta)
{
	_int	iEnd = 0;

	for (auto& pair : m_mapStaticObject[m_eCurrentStage])
	{
		for (auto& pStaticObject : pair.second)
		{
			pStaticObject->Update_GameObject(fTimeDelta);
		}
	}

	for (auto& iter : m_mapLayer)
	{
		iEnd = iter.second->Update_Layer(fTimeDelta);

		if (iEnd & 0x80000000)
			return -1;
	}

	return NO_EVENT;
}

_int CObjectMgr::LateUpdate_ObjectMgr(const _float & fTimeDelta)
{
	_int	iEnd = 0;

	for (auto& pair : m_mapStaticObject[m_eCurrentStage])
	{
		for (auto& pStaticObject : pair.second)
		{
			pStaticObject->LateUpdate_GameObject(fTimeDelta);
		}
	}

	for (auto& iter : m_mapLayer)
	{
		iEnd = iter.second->LateUpdate_Layer(fTimeDelta);

		if (iEnd & 0x80000000)
			return -1;
	}

	return NO_EVENT;
}

void CObjectMgr::Send_PacketToServer()
{
	for (auto& iter : m_mapLayer)
		iter.second->Send_PacketToServer();
}

void CObjectMgr::Render_ObjectMgr(const _float & fTimeDelta)
{
	for (auto& iter : m_mapLayer)
		iter.second->Render_Layer(fTimeDelta);
}

HRESULT CObjectMgr::Delete_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- Layer Tag값을 탐색한다.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	if (iter_find == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- 탐색에 성공할 경우 해당 Key값의 OBJLIST에서 해당 index삭제.
	____________________________________________________________________________________________________________*/
	return iter_find->second->Delete_GameObject(wstrObjTag, iIdx);
}

HRESULT CObjectMgr::Delete_ServerObject(wstring wstrLayerTag, wstring wstrObjTag, int num)
{
	/*__________________________________________________________________________________________________________
	- Layer Tag값을 탐색한다.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	if (iter_find == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- 탐색에 성공할 경우 해당 Key값의 OBJLIST에서 해당 index삭제.
	____________________________________________________________________________________________________________*/
	return iter_find->second->Delete_ServerObject(wstrObjTag, num);
}

HRESULT CObjectMgr::Clear_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- Layer Tag값을 탐색한다.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	if (iter_find == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- 탐색에 성공할 경우 해당 OBJLIST 삭제.
	____________________________________________________________________________________________________________*/
	return iter_find->second->Clear_OBJLIST(wstrObjTag);
}

void CObjectMgr::Clear_Layer()
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();
}

void CObjectMgr::Clear_Prototype()
{
	for_each(m_mapObjectPrototype.begin(), m_mapObjectPrototype.end(), CDeleteMap());
	m_mapObjectPrototype.clear();
}

void CObjectMgr::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy ObjectMgr");
#endif
	for (_uint i = 0; i < STAGEID::STAGE_END; ++i)
	{
		for (auto& pair : m_mapStaticObject[i])
		{
			for (auto& object : pair.second)
				Safe_Release(object);

			pair.second.clear();
		}

		m_mapStaticObject[i].clear();
	}

	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	for_each(m_mapObjectPrototype.begin(), m_mapObjectPrototype.end(), CDeleteMap());
	m_mapObjectPrototype.clear();

}
