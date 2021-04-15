#include "Layer.h"

USING(Engine)

CLayer::CLayer()
{
}


OBJLIST * CLayer::Get_OBJLIST(wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- 1. map���� ã���� �ϴ� OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapObjLst.find(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- 2. ã���� �ϴ� OBJLIST�� ���ٸ� nullptr�� ��ȯ.
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapObjLst.end())
		return nullptr;

	return &(iter_find->second);
}

CGameObject * CLayer::Get_GameObject(wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- 1. map���� ã���� �ϴ� OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	OBJLIST* pObjLst = Get_OBJLIST(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- 2. ã���� �ϴ� OBJLIST�� ���ٸ� nullptr�� ��ȯ.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pObjLst, nullptr);

	/*__________________________________________________________________________________________________________
	- 3. ã�����ϴ� GameObject�� OBJLIST���� Ž��.
	____________________________________________________________________________________________________________*/
	if (pObjLst->size() <= (_ulong)iIdx)
		return nullptr;

	OBJITER iter_begin = pObjLst->begin();

	for (_int i = 0; i < iIdx; ++i)
		++iter_begin;

	return (*iter_begin);
}

CGameObject* CLayer::Get_ServerObject(wstring wstrObjTag, int num)
{
	/*__________________________________________________________________________________________________________
	- 1. map���� ã���� �ϴ� OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	OBJLIST* pObjLst = Get_OBJLIST(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- 2. ã���� �ϴ� OBJLIST�� ���ٸ� nullptr�� ��ȯ.
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(pObjLst, nullptr);

	/*__________________________________________________________________________________________________________
	- 3. ã�����ϴ� GameObject�� OBJLIST���� Ž��.
	____________________________________________________________________________________________________________*/

	OBJITER iter_begin = pObjLst->begin();

	for (iter_begin; iter_begin != pObjLst->end();)
	{
		if ((*iter_begin)->Get_ServerNumber() == num)
			return (*iter_begin);
		else
			++iter_begin;
	}

	return nullptr;
}


HRESULT CLayer::Add_GameObject(wstring wstrObjTag, CGameObject * pGameObject)
{
	if (pGameObject != nullptr)
	{
		/*__________________________________________________________________________________________________________
		- 1. map�� ObjTag���� �ִ��� Ž���Ѵ�.
		____________________________________________________________________________________________________________*/
		auto iter = m_mapObjLst.find(wstrObjTag);

		/*__________________________________________________________________________________________________________
		- 2. ���� ���, �ӽ� OBJLIST��ü�� �������� �Ŀ� GameObject�� �־��ش�.
		____________________________________________________________________________________________________________*/
		if (iter == m_mapObjLst.end())
			m_mapObjLst[wstrObjTag] = OBJLIST();


		m_mapObjLst[wstrObjTag].emplace_back(pGameObject);
	}

	return S_OK;
}

HRESULT CLayer::Delete_GameObject(wstring wstrObjTag, _int iIdx)
{
	/*__________________________________________________________________________________________________________
	- ������ OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapObjLst.find(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- Ž���� ������ ��� ����
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapObjLst.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Ž�� ������, �ش� Key���� OBJLIST���� �ش� index����.
	____________________________________________________________________________________________________________*/
	CGameObject* pGameObject = Get_GameObject(wstrObjTag, iIdx);
	if (nullptr != pGameObject)
	{
		pGameObject->Set_DeadGameObject();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT CLayer::Delete_ServerObject(wstring wstrObjTag, int num)
{
	/*__________________________________________________________________________________________________________
	- ������ OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapObjLst.find(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- Ž���� ������ ��� ����
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapObjLst.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Ž�� ������, �ش� Key���� OBJLIST���� �ش� index����.
	____________________________________________________________________________________________________________*/
	CGameObject* pGameObject = Get_ServerObject(wstrObjTag, num);
	if (nullptr != pGameObject)
	{
		pGameObject->Set_DeadGameObject();

		return S_OK;
	}

	return E_FAIL;
}

HRESULT CLayer::Clear_OBJLIST(wstring wstrObjTag)
{
	/*__________________________________________________________________________________________________________
	- ������ OBJLIST�� Key���� Ž���Ѵ�.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapObjLst.find(wstrObjTag);

	/*__________________________________________________________________________________________________________
	- Ž���� ������ ��� ����
	____________________________________________________________________________________________________________*/
	if (iter_find == m_mapObjLst.end())
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- Ž�� ������, �ش� Key���� OBJLIST ����.
	____________________________________________________________________________________________________________*/
	for (auto& pGameObj : iter_find->second)
		pGameObj->Set_DeadGameObject();

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	for (auto& MyPair : m_mapObjLst)
	{
		OBJITER iter_begin = MyPair.second.begin();
		OBJITER iter_end = MyPair.second.end();

		for (; iter_begin != iter_end;)
		{
			if (MyPair.second.empty())
				break;

			_int iEvent = (*iter_begin)->Update_GameObject(fTimeDelta);

			if (DEAD_OBJ == iEvent)
			{
				Engine::Safe_Release(*iter_begin);
				iter_begin = MyPair.second.erase(iter_begin);
			}
			else if (RETURN_OBJ == iEvent)
			{
				iter_begin = MyPair.second.erase(iter_begin);
			}
			else
				++iter_begin;
		}

	}

	return NO_EVENT;
}

_int CLayer::LateUpdate_Layer(const _float & fTimeDelta)
{
	for (auto& MyPair : m_mapObjLst)
	{
		for (auto& pGameObject : MyPair.second)
		{
			pGameObject->LateUpdate_GameObject(fTimeDelta);
			pGameObject->Clear_CollisionList();
		}
	}

	return NO_EVENT;
}

void CLayer::Send_PacketToServer()
{
	for (auto& MyPair : m_mapObjLst)
	{
		for (auto& pGameObject : MyPair.second)
			pGameObject->Send_PacketToServer();
	}
}

void CLayer::Render_Layer(const _float & fTimeDelta)
{
	for (auto& MyPair : m_mapObjLst)
	{
		for (auto& pGameObject : MyPair.second)
			pGameObject->Render_GameObject(fTimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer* pInstance = new CLayer;

	if (FAILED(pInstance->Ready_Layer()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLayer::Free()
{
	for (auto& MyPair : m_mapObjLst)
	{
		for (auto& pGameObject : MyPair.second)
			Engine::Safe_Release(pGameObject);

		MyPair.second.clear();
	}

	m_mapObjLst.clear();
}
