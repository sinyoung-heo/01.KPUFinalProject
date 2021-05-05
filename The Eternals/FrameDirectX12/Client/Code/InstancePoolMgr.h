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

class CInstancePoolMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CInstancePoolMgr)

private:
	explicit CInstancePoolMgr();
	virtual ~CInstancePoolMgr() = default;

public:
	INSTANCE_POOL_DESC<CCollisionTick>* Get_CollisionTickPool()			{ return m_pCollisionTickPool; }
	INSTANCE_POOL_DESC<CCrab>*			Get_MonsterCrabPool()			{ return m_pMonsterCrabPool; }
	INSTANCE_POOL_DESC<CCloderA>*		Get_MonsterCloderAPool()		{ return m_pMonsterCloderAPool; }
	INSTANCE_POOL_DESC<CMonkey>*		Get_MonsterMonkeyPool()			{ return m_pMonsterMonkeyPool; }
	INSTANCE_POOL_DESC<CDrownedSailor>* Get_MonsterDrownedSailorPool()	{ return m_pMonsterDrownedSailorPool; }
	INSTANCE_POOL_DESC<CGiantBeetle>*	Get_MonsterGiantBeetlePool()	{ return m_pMonsterGiantBeetlePool; }
	INSTANCE_POOL_DESC<CGiantMonkey>*	Get_MonsterGiantMonkeyPool()	{ return m_pMonsterGiantMonkeyPool; }
	INSTANCE_POOL_DESC<CCraftyArachne>* Get_MonsterCraftyArachnePool()	{ return m_pMonsterCraftyArachnePool; }

	void Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

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