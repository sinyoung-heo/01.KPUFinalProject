#pragma once
#include "Include.h"
#include "Base.h"

class CCollisionTick;

class CCrab;
class CCloderA;
class CMonkey;
class CDrownedSailor;
class CGiantBeetle;
class CGiantMonkey;
class CCraftyArachne;

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
	Engine::CGameObject*	Pop_CollisionTickInstance();
	void			Return_CollisionTickInstance(const _uint& uiInstanceIdx);

private:
	// CollisionTick
	INSTANCE_POOL_DESC<CCollisionTick>*	m_pCollisionTickPool;

	// Monster
	INSTANCE_POOL_DESC<CCrab>*			m_pMonsterCrabPool;
	INSTANCE_POOL_DESC<CCloderA>*		m_pMonsterCloderAPool;
	INSTANCE_POOL_DESC<CMonkey>*		m_pMonsterMonkeyPool;
	INSTANCE_POOL_DESC<CDrownedSailor>*	m_pMonsterDrownedSailorPool;
	INSTANCE_POOL_DESC<CGiantBeetle>*	m_pMonsterGiantBeetlePool;
	INSTANCE_POOL_DESC<CGiantMonkey>*	m_pMonsterGiantMonkeyPool;
	INSTANCE_POOL_DESC<CCraftyArachne>* m_pMonsterCraftyArachnePool;

private:
	virtual void Free();
};

