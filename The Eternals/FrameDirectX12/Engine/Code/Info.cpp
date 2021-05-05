#include "Info.h"
#include <random>

USING(Engine)

CInfo::CInfo()
{	
}

CInfo::CInfo(const CInfo& rhs)
	: m_fSpeed(rhs.m_fSpeed), m_iLev(0), m_iMinAttack(0), m_iMaxAttack(0),
	m_iHp(0), m_iMaxHp(0), m_iMp(0), m_iMaxMp(0),
	m_iExp(0), m_iMaxExp(0), m_fRadius(rhs.m_fRadius)
	, m_vArrivePos(rhs.m_vArrivePos)
{
}


_int CInfo::Get_RandomDamage()
{
	random_device					rd;
	default_random_engine			dre { rd() };
	uniform_int_distribution<_int>	uid { m_iMinAttack, m_iMaxAttack };

	return uid(dre);
}

HRESULT CInfo::Ready_Component()
{
	return S_OK;
}

void CInfo::Update_Component(const _float & fTimeDelta)
{
}

CComponent * CInfo::Clone()
{
	return new CInfo(*this);
}

CInfo * CInfo::Create()
{
	CInfo* pInstance = new CInfo;

	if (FAILED(pInstance->Ready_Component()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CInfo::Free()
{
	CComponent::Free();
}
