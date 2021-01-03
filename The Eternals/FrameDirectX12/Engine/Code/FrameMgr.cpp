#include "FrameMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
{
}



_bool CFrameMgr::IsPermit_Call(wstring wstrFrameTag, const _float & fTimeDelta)
{
	CFrame*	pInstance = Find_Frame(wstrFrameTag);
	NULL_CHECK_RETURN(pInstance, false);

	return pInstance->IsPermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(wstring wstrFrameTag, const _float & fCallLimit)
{
	/*__________________________________________________________________________________________________________
	- ���� �����ϱ� ����, FrameTag���� Ž���Ѵ�.
	- �̹� �����ϴ� ���̸� ���� �Ҵ��� �ʿ� �����Ƿ� ����.
	____________________________________________________________________________________________________________*/
	CFrame* pInstance = Find_Frame(wstrFrameTag);

	if (nullptr != pInstance)
		return E_FAIL;

	/*__________________________________________________________________________________________________________
	- ������ ���� �Ҵ��� ��, map�����̳ʿ� �߰����ش�.
	____________________________________________________________________________________________________________*/
	pInstance = CFrame::Create(fCallLimit);
	NULL_CHECK_RETURN(pInstance, E_FAIL);

	m_mapFrame.emplace(wstrFrameTag, pInstance);

	return S_OK;
}

CFrame * CFrameMgr::Find_Frame(wstring wstrFrameTag)
{
	/*__________________________________________________________________________________________________________
	- ã���� �ϴ� Frame Tag�� Ž���Ѵ�.
	- ���ٸ� nullptr ��ȯ.
	____________________________________________________________________________________________________________*/
	auto iter_find = m_mapFrame.find(wstrFrameTag);

	if (iter_find == m_mapFrame.end())
		return nullptr;


	return iter_find->second;
}

void CFrameMgr::Free()
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());

	m_mapFrame.clear();
}
