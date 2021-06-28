#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CInfo final : public CComponent
{
private:
	explicit CInfo();
	explicit CInfo(const CInfo& rhs);
	virtual ~CInfo() = default;

public:
	_int Get_RandomDamage();

	HRESULT			Ready_Component();
	virtual void	Update_Component(const _float& fTimeDelta);

public:
	_int	m_iMoney		= 0;
	_int	m_iLev			= 1;
	_int	m_iMinAttack	= 0;
	_int	m_iMaxAttack	= 1;
	_int	m_iHp			= 0;
	_int	m_iMaxHp		= 0;
	_int	m_iMp			= 0;
	_int	m_iMaxMp		= 0;
	_int	m_iExp			= 0;
	_int	m_iMaxExp		= 0;
	_float	m_fSpeed		= 0.0f;
	_float	m_fRadius		= 0.0f;
	_vec3	m_vArrivePos	= _vec3(0.f);

public:
	virtual CComponent* Clone();
	static CInfo*		Create();
private:
	virtual void		Free();
};

END