#pragma once
#include "Include.h"
#include "Base.h"

namespace Engine
{
	class CObjectMgr;
}
class CCollisionTick;

template<class T1>
struct INSTANCE_POOL_DESC
{
	T1**	ppInstances       = nullptr;
	_uint	uiInstanceSize    = 0;
	_uint	uiCurrentIdx      = 0;

};

class CInstancePoolMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CInstancePoolMgr)

private:
	explicit CInstancePoolMgr();
	virtual ~CInstancePoolMgr() = default;

public:
	void Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

	// CollisionTick
	CCollisionTick*	Pop_CollisionTickInstance();
	void			Return_CollisionTickInstance(const _uint& uiInstanceIdx);
private:
	Engine::CObjectMgr*					m_pObjectMgr;
	INSTANCE_POOL_DESC<CCollisionTick>*	m_pCollisionTickPool;

private:
	virtual void Free();
};

