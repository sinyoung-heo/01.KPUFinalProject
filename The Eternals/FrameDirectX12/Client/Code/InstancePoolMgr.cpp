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
#include "PCOthersGladiator.h"
#include "PCWeaponTwoHand.h"
#include "PCWeaponBow.h"

IMPLEMENT_SINGLETON(CInstancePoolMgr)

CInstancePoolMgr::CInstancePoolMgr()
{
}

INSTANCE_POOL_DESC<CPCWeaponTwoHand>* CInstancePoolMgr::Get_PCWeaponTwoHand(const char& chWeaponType)
{
	if (chWeaponType == Twohand19_A_SM)
		return m_pPCWeaponTwohand19_A_SM_Pool;

	else if (chWeaponType == TwoHand27_SM)
		return m_pPCWeaponTwoHand27_SM_Pool;

	else if (chWeaponType == TwoHand29_SM)
		return m_pPCWeaponTwoHand29_SM_Pool;

	else if (chWeaponType == TwoHand31_SM)
		return m_pPCWeaponTwoHand31_SM_Pool;

	else if (chWeaponType == TwoHand33_B_SM)
		return m_pPCWeaponTwoHand33_B_SM_Pool;

	else
		return nullptr;
}

INSTANCE_POOL_DESC<CPCWeaponBow>* CInstancePoolMgr::Get_PCWeaponBow(const char& chWeaponType)
{
	if (chWeaponType == Bow18_A_SM)
		return m_pPCWeaponBow18_A_SM_Pool;

	else if (chWeaponType == Bow23_SM)
		return m_pPCWeaponBow23_SM_Pool;

	else if (chWeaponType == Bow27_SM)
		return m_pPCWeaponBow27_SM_Pool;

	else if (chWeaponType == Bow31_SM)
		return m_pPCWeaponBow31_SM_Pool;

	else if (chWeaponType == Event_Season_Bow_01_SM)
		return m_pPCWeaponEvent_Season_Bow_01_SM_Pool;

	else
		return nullptr;
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

	// PCWeapon
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponTwohand19_A_SM_Pool, L"Twohand19_A_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponTwoHand27_SM_Pool, L"TwoHand27_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponTwoHand29_SM_Pool, L"TwoHand29_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponTwoHand31_SM_Pool, L"TwoHand31_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponTwoHand33_B_SM_Pool, L"TwoHand33_B_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponBow18_A_SM_Pool, L"Bow18_A_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponBow23_SM_Pool, L"Bow23_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponBow27_SM_Pool, L"Bow27_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponBow31_SM_Pool, L"Bow31_SM", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponEvent_Season_Bow_01_SM_Pool, L"Event_Season_Bow_01_SM", 16);

	// PCOthers
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCOthersGladiatorPool, 10);
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

	Safe_Release_InstacePool(m_pPCOthersGladiatorPool);

	Safe_Release_InstacePool(m_pPCWeaponTwohand19_A_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponTwoHand27_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponTwoHand29_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponTwoHand31_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponTwoHand33_B_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponBow18_A_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponBow23_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponBow27_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponBow31_SM_Pool);
	Safe_Release_InstacePool(m_pPCWeaponEvent_Season_Bow_01_SM_Pool);
}
