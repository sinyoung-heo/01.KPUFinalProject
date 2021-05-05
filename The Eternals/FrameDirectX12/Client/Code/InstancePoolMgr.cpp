#include "stdafx.h"
#include "InstancePoolMgr.h"
#include "ObjectMgr.h"
#include "CollisionTick.h"
#include "Crab.h"
#include "Monkey.h"
#include "CloderA.h"
#include "DrownedSailor.h"
#include "GiantBeetle.h"
#include "GiantMonkey.h"
#include "CraftyArachne.h"

IMPLEMENT_SINGLETON(CInstancePoolMgr)

CInstancePoolMgr::CInstancePoolMgr()
{
}

void CInstancePoolMgr::Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	_uint uiInstanceSize = 0;

	// CollisionTick
	uiInstanceSize                       = 32;
	m_pCollisionTickPool                 = new INSTANCE_POOL_DESC<CCollisionTick>;
	m_pCollisionTickPool->ppInstances    = CCollisionTick::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pCollisionTickPool->uiInstanceSize = uiInstanceSize;
	
	// Monster Crab
	uiInstanceSize                     = 4;
	m_pMonsterCrabPool                 = new INSTANCE_POOL_DESC<CCrab>;
	m_pMonsterCrabPool->ppInstances    = CCrab::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterCrabPool->uiInstanceSize = uiInstanceSize;

	// Monster CloderA
	uiInstanceSize                        = 4;
	m_pMonsterCloderAPool                 = new INSTANCE_POOL_DESC<CCloderA>;
	m_pMonsterCloderAPool->ppInstances    = CCloderA::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterCloderAPool->uiInstanceSize = uiInstanceSize;

	// Monster Monkey
	uiInstanceSize                       = 4;
	m_pMonsterMonkeyPool                 = new INSTANCE_POOL_DESC<CMonkey>;
	m_pMonsterMonkeyPool->ppInstances    = CMonkey::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterMonkeyPool->uiInstanceSize = uiInstanceSize;

	// Monster DrownedSailor
	uiInstanceSize                              = 4;
	m_pMonsterDrownedSailorPool                 = new INSTANCE_POOL_DESC<CDrownedSailor>;
	m_pMonsterDrownedSailorPool->ppInstances    = CDrownedSailor::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterDrownedSailorPool->uiInstanceSize = uiInstanceSize;

	// Monster GiantBeetle
	uiInstanceSize                            = 2;
	m_pMonsterGiantBeetlePool                 = new INSTANCE_POOL_DESC<CGiantBeetle>;
	m_pMonsterGiantBeetlePool->ppInstances    = CGiantBeetle::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterGiantBeetlePool->uiInstanceSize = uiInstanceSize;

	// Monster GiantMonkey
	uiInstanceSize                            = 2;
	m_pMonsterGiantMonkeyPool                 = new INSTANCE_POOL_DESC<CGiantMonkey>;
	m_pMonsterGiantMonkeyPool->ppInstances    = CGiantMonkey::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterGiantMonkeyPool->uiInstanceSize = uiInstanceSize;

	// Monster CraftyArchne
	uiInstanceSize                              = 2;
	m_pMonsterCraftyArachnePool                 = new INSTANCE_POOL_DESC<CCraftyArachne>;
	m_pMonsterCraftyArachnePool->ppInstances    = CCraftyArachne::Create_InstancePool(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pMonsterCraftyArachnePool->uiInstanceSize = uiInstanceSize;
}

void CInstancePoolMgr::Free()
{
	// CollisionTick
	for (_uint i = 0; i < m_pCollisionTickPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pCollisionTickPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pCollisionTickPool->ppInstances);
	Engine::Safe_Delete(m_pCollisionTickPool);

	// Monster Crab
	for (_uint i = 0; i < m_pMonsterCrabPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterCrabPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterCrabPool->ppInstances);
	Engine::Safe_Delete(m_pMonsterCrabPool);

	// Monster CloderA
	for (_uint i = 0; i < m_pMonsterCloderAPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterCloderAPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterCloderAPool->ppInstances);
	Engine::Safe_Delete(m_pMonsterCloderAPool);

	// Monster Monkey
	for (_uint i = 0; i < m_pMonsterMonkeyPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterMonkeyPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterMonkeyPool->ppInstances);
	Engine::Safe_Delete(m_pMonsterMonkeyPool);

	// Monster DrownedSailor
	for (_uint i = 0; i < m_pMonsterDrownedSailorPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterDrownedSailorPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterDrownedSailorPool->ppInstances);
	Engine::Safe_Delete(m_pMonsterDrownedSailorPool);

	// Monster GiantBeetle
	for (_uint i = 0; i < m_pMonsterGiantBeetlePool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterGiantBeetlePool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterGiantBeetlePool->ppInstances);
	Engine::Safe_Delete(m_pMonsterGiantBeetlePool);

	// Monster GiantMonkey
	for (_uint i = 0; i < m_pMonsterGiantMonkeyPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterGiantMonkeyPool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterGiantMonkeyPool->ppInstances);
	Engine::Safe_Delete(m_pMonsterGiantMonkeyPool);

	// Monster CraftyArchne
	for (_uint i = 0; i < m_pMonsterCraftyArachnePool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pMonsterCraftyArachnePool->ppInstances[i]);

	Engine::Safe_Delete_Array(m_pMonsterCraftyArachnePool->ppInstances);
	Engine::Safe_Delete(m_pMonsterCraftyArachnePool);
}
