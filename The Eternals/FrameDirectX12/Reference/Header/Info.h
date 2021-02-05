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
	HRESULT			Ready_Component();
	virtual void	Update_Component(const _float& fTimeDelta);

public:
	_float	m_fSpeed	= 0.0f;
	_float	m_fRadius	= 0.0f;
	_vec3	m_arrBezierPoint[4] = { _vec3(0.f,0.f,0.f), };

public:
	virtual CComponent* Clone();
	static CInfo*		Create();
private:
	virtual void		Free();
};

END