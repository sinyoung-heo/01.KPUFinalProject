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
	- 1. ã���� �ϴ� Layer Tag�� Ž���Ѵ�
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. ���� ��� nullptr�� ��ȯ�Ѵ�
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapLayer.end())
		return nullptr;


	return iter_find->second;
}

OBJLIST * CObjectMgr::Get_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- 1. ã���� �ϴ� Layer Tag���� Ž���Ѵ�
	____________________________________________________________________________________________________________*/
	CLayer* pInstance = Get_Layer(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. ���� ���, nullptr�� ��ȯ�Ѵ�
	____________________________________________________________________________________________________________*/
	if (pInstance == nullptr)
		return nullptr;


	return pInstance->Get_OBJLIST(wstrObjTag);
}

CGameObject * CObjectMgr::Get_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- 1. ã���� �ϴ� Layer Tag���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	CLayer* pInstance = Get_Layer(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. ���� ���, nullptr�� ��ȯ�Ѵ�.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pInstance, nullptr);


	return pInstance->Get_GameObject(wstrObjTag, iIdx);
}

HRESULT CObjectMgr::Add_GameObjectPrototype(wstring wstrPrototypeTag, CGameObject * pGameObject)
{
	if (nullptr != pGameObject)
	{
		/*__________________________________________________________________________________________________________
		Prototype Tag���� ���� ��쿡�� map�� �־��ش�.
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
	- 1. Layer�� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find_layer = m_mapLayer.find(wstrLayerTag);

	/*__________________________________________________________________________________________________________
	- 2. �ش� Layer�� ���� ��� ����.
	____________________________________________________________________________________________________________*/
	if (iter_find_layer == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- 3. Layer�� GameObject �߰�.
	____________________________________________________________________________________________________________*/
	return iter_find_layer->second->Add_GameObject(wstrObjTag, pGameObject);

	return S_OK;
}

_int CObjectMgr::Update_ObjectMgr(const _float & fTimeDelta)
{
	_int	iEnd = 0;

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

	for (auto& iter : m_mapLayer)
	{
		iEnd = iter.second->LateUpdate_Layer(fTimeDelta);

		if (iEnd & 0x80000000)
			return -1;
	}

	return NO_EVENT;
}

void CObjectMgr::Render_ObjectMgr(const _float & fTimeDelta)
{
	for (auto& iter : m_mapLayer)
		iter.second->Render_Layer(fTimeDelta);
}

HRESULT CObjectMgr::Delete_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- Layer Tag���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	if (iter_find == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Ž���� ������ ��� �ش� Key���� OBJLIST���� �ش� index����.
	____________________________________________________________________________________________________________*/
	return iter_find->second->Delete_GameObject(wstrObjTag, iIdx);
}

HRESULT CObjectMgr::Clear_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- Layer Tag���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapLayer.find(wstrLayerTag);

	if (iter_find == m_mapLayer.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Ž���� ������ ��� �ش� OBJLIST ����.
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

	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	for_each(m_mapObjectPrototype.begin(), m_mapObjectPrototype.end(), CDeleteMap());
	m_mapObjectPrototype.clear();
}
