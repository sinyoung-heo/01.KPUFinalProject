#include "Frame.h"

USING(Engine)

CFrame::CFrame()
{
}


_bool CFrame::IsPermit_Call(const _float & fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ ���ѵ� ������ ��ġ ��ŭ GameLoop�� ������Ų�� ]
	____________________________________________________________________________________________________________*/
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

HRESULT CFrame::Ready_Frame(const _float & fCallLimit)
{
	/*__________________________________________________________________________________________________________
	[ ������ Frame ��ġ�� �Է¹޾� �ʱ�ȭ�Ѵ� ]
	____________________________________________________________________________________________________________*/
	m_fCallLimit = 1.f / fCallLimit;

	return S_OK;
}

CFrame * CFrame::Create(const _float & fCallLimit)
{
	CFrame* pInstance = new CFrame;

	if (FAILED(pInstance->Ready_Frame(fCallLimit)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CFrame::Free()
{
}
