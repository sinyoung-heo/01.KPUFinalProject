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
#include "NPC_Walker.h"
#include "NPC_Assistant.h"
#include "NPC_Stander.h"
#include "NPC_Merchant.h"
#include "NPC_Quest.h"
#include "NPC_Children.h"

IMPLEMENT_SINGLETON(CInstancePoolMgr)

CInstancePoolMgr::CInstancePoolMgr()
{
}

void CInstancePoolMgr::Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	// CollisionTick
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pCollisionTickPool, 32);
	
	// Monster
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCrabPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCloderAPool, 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterMonkeyPool, 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterDrownedSailorPool, 8);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterGiantBeetlePool, 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterGiantMonkeyPool, 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCraftyArachnePool, 2);

	// NPC
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_ChickenPool, L"Chicken", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_CatPool, L"Cat", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_AmanBoyPool, L"Aman_boy", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_AssistantPool, L"Popori_boy", 2);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Stander_VillagersPool, L"NPC_Villagers", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Stander_Baraka_M_ExtractorPool, L"Baraka_M_Extractor", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Popori_M_MerchantPool, L"Popori_M_Merchant", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Baraka_M_MerchantPool, L"Baraka_M_Merchant", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Baraka_M_MystelliumPool, L"Baraka_M_Mystellium", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_QuestPool, L"Castanic_M_Lsmith", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HumanBoyPool, L"Human_boy", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HumanGirlPool, L"Human_girl", 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HighelfGirlPool, L"Highelf_girl", 10);
}

void CInstancePoolMgr::Free()
{
	Safe_Release_InstacePool(m_pCollisionTickPool);

	Safe_Release_InstacePool(m_pMonsterCrabPool);
	Safe_Release_InstacePool(m_pMonsterCloderAPool);
	Safe_Release_InstacePool(m_pMonsterMonkeyPool);
	Safe_Release_InstacePool(m_pMonsterDrownedSailorPool);
	Safe_Release_InstacePool(m_pMonsterGiantBeetlePool);
	Safe_Release_InstacePool(m_pMonsterGiantMonkeyPool);
	Safe_Release_InstacePool(m_pMonsterCraftyArachnePool);

	Safe_Release_InstacePool(m_pNPC_Walker_ChickenPool);
	Safe_Release_InstacePool(m_pNPC_Walker_CatPool);
	Safe_Release_InstacePool(m_pNPC_Walker_AmanBoyPool);
	Safe_Release_InstacePool(m_pNPC_AssistantPool);
	Safe_Release_InstacePool(m_pNPC_Stander_VillagersPool);
	Safe_Release_InstacePool(m_pNPC_Stander_Baraka_M_ExtractorPool);
	Safe_Release_InstacePool(m_pNPC_Merchant_Popori_M_MerchantPool);
	Safe_Release_InstacePool(m_pNPC_Merchant_Baraka_M_MerchantPool);
	Safe_Release_InstacePool(m_pNPC_Merchant_Baraka_M_MystelliumPool);
	Safe_Release_InstacePool(m_pNPC_QuestPool);
	Safe_Release_InstacePool(m_pNPC_Children_HumanBoyPool);
	Safe_Release_InstacePool(m_pNPC_Children_HumanGirlPool);
	Safe_Release_InstacePool(m_pNPC_Children_HighelfGirlPool);
}
