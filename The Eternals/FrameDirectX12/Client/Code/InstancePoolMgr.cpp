#include "stdafx.h"
#include "InstancePoolMgr.h"
#include "ObjectMgr.h"
#include "CollisionTick.h"
#include "CollisionArrow.h"
#include "Crab.h"
#include "Monkey.h"
#include "CloderA.h"
#include "DrownedSailor.h"
#include "GiantBeetle.h"
#include "GiantMonkey.h"
#include "CraftyArachne.h"
#include "Lakan.h"
#include "PrionBerserker.h"
#include "PrionBerserkerBoss.h"
#include "CinemaVergos.h"
#include "Vergos.h"
#include "NPC_Walker.h"
#include "NPC_Assistant.h"
#include "NPC_Stander.h"
#include "NPC_Merchant.h"
#include "NPC_Quest.h"
#include "NPC_Children.h"
#include "PCOthersGladiator.h"
#include "PCOthersArcher.h"
#include "PCOthersPriest.h"
#include "PCWeaponTwoHand.h"
#include "PCWeaponBow.h"
#include "PCWeaponRod.h"
#include "FadeInOut.h"
#include "DmgFont.h"

//Effect
#include "SwordEffect.h"
#include "SwordEffect_s.h"
#include "IceStorm.h"
#include "IceStorm_s.h"
#include "IceStorm_m.h"
#include "IceDecal.h"
#include "FireDecal.h"
#include "FireRing.h"
#include "FrameMesh.h"
#include "ParticleEffect.h"
#include "TextureEffect.h"
#include "DistTrail.h"
#include "EffectShield.h"
#include "EffectAxe.h"
#include "MagicCircle.h"
#include "GridShieldEffect.h"
#include "MeshParticleEffect.h"

#include "WarningGround.h"
#include "RectDecal.h"
#include "DirParticleEffect.h"
#include "BreathEffect.h"
#include "BossDecal.h"
#include "TonadoEffect.h"
IMPLEMENT_SINGLETON(CInstancePoolMgr)

CInstancePoolMgr::CInstancePoolMgr()
{
}

INSTANCE_POOL_DESC<CCollisionArrow>* CInstancePoolMgr::Get_CollisionArrowPool(const ARROW_POOL_TYPE& eType)
{
	if (ARROW_POOL_TYPE::ARROW_POOL_ICE == eType)
		return m_pCollisionArrowIcePool;
	else if (ARROW_POOL_TYPE::ARROW_POOL_FIRE == eType)
		return m_pCollisionArrowFirePool;
	else if (ARROW_POOL_TYPE::ARROW_POOL_LIGHTNING)
		return m_pCollisionArrowLightningPool;

	return nullptr;
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

INSTANCE_POOL_DESC<CPCWeaponRod>* CInstancePoolMgr::Get_PCWeaponRod(const char& chWeaponType)
{
	if (chWeaponType == Event_Wit_Rod_01)
		return m_pPCWeaponEvent_Wit_Rod_01_Pool;

	else if (chWeaponType == Rod19_A)
		return m_pPCWeaponRod18_A_Pool;

	else if (chWeaponType == Rod28_B)
		return m_pPCWeaponRod28_B_Pool;

	else if (chWeaponType == Rod31)
		return m_pPCWeaponRod31_Pool;

	else if (chWeaponType == Rod33_B)
		return m_pPCWeaponRod33_B_Pool;

	else
		return nullptr;
}

void CInstancePoolMgr::Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	// DmgFont
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pDmgFontPool, 128);

	// CollisionTick
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pCollisionTickPool, 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pCollisionArrowIcePool, L"ArrowIce", 128);
	// Ready_InstacePool(pGraphicDevice, pCommandList, &m_pCollisionArrowFirePool, L"ArrowFire", 128);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pCollisionArrowLightningPool, L"ArrowLightning", 128);
	
	// Monster
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCrabPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCloderAPool, 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterMonkeyPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterDrownedSailorPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterGiantBeetlePool, 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterGiantMonkeyPool, 2);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCraftyArachnePool, 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterLakanPool, 52);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterPrionBerserkerPool, 50);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterPrionBerserkerBossPool, 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterCinemaVergosPool, 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pMonsterVergosPool, 1);

	// NPC
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_ChickenPool, L"Chicken", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_CatPool, L"Cat", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Walker_AmanBoyPool, L"Aman_boy", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_AssistantPool, L"Popori_boy", 2);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Stander_VillagersPool, L"NPC_Villagers", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Stander_Baraka_M_ExtractorPool, L"Baraka_M_Extractor", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Popori_M_MerchantPool, L"Popori_M_Merchant", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Baraka_M_MerchantPool, L"Baraka_M_Merchant", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Merchant_Baraka_M_MystelliumPool, L"Baraka_M_Mystellium", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_QuestPool, L"Castanic_M_Lsmith", 1);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HumanBoyPool, L"Human_boy", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HumanGirlPool, L"Human_girl", 4);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pNPC_Children_HighelfGirlPool, L"Highelf_girl", 4);

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

	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponEvent_Wit_Rod_01_Pool, L"Event_Wit_Rod_01", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponRod18_A_Pool, L"Rod19_A", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponRod28_B_Pool, L"Rod28_B", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponRod31_Pool, L"Rod31", 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCWeaponRod33_B_Pool, L"Rod33_B", 16);

	// PCOthers
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCOthersGladiatorPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCOthersArcherPool, 10);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pPCOthersPriestPool, 10);

	// Effect
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_SwordEffect_Pool, 16);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_SwordEffect_s_Pool, 100);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_IceStorm_Pool, 512);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_IceStorm_m_Pool, 256);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_IceDecal_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_FireDecal_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_Particle_Pool, 512);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_FrameMesh_Pool, 100);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_FireRing_Pool, 20);

	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_IceStorm_s_Pool, 512);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_DistTrail_Pool, 128);

	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_Texture_Pool, 512);

	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_Shield_Pool, 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_Axe_Pool, 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MagicCircle_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_GridShieldEffect_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MeshParticleRock0_Pool,L"publicStone0", 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MeshParticleRock1_Pool, L"publicStone1", 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MeshParticleRock2_Pool, L"publicStone2", 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MeshParticleRock3_Pool, L"publicStone3", 64);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_MeshParticleRock4_Pool, L"publicStone4", 64);


	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_WarningGround_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_RectDecal_Pool, 20);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_BossDecal_Pool, 3);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_DirParticle_Pool, 100);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_BreathEffect_Pool, 3);
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pEffect_Tonado_Pool, 3);

	
}

void CInstancePoolMgr::Ready_LoadingInstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	Ready_InstacePool(pGraphicDevice, pCommandList, &m_pFadeInOutPool, 4);
}

void CInstancePoolMgr::Free()
{
	Safe_Release_InstacePool(m_pFadeInOutPool);

	Safe_Release_InstacePool(m_pDmgFontPool);

	Safe_Release_InstacePool(m_pCollisionTickPool);
	Safe_Release_InstacePool(m_pCollisionArrowIcePool);
	Safe_Release_InstacePool(m_pCollisionArrowFirePool);
	Safe_Release_InstacePool(m_pCollisionArrowLightningPool);

	Safe_Release_InstacePool(m_pMonsterCrabPool);
	Safe_Release_InstacePool(m_pMonsterCloderAPool);
	Safe_Release_InstacePool(m_pMonsterMonkeyPool);
	Safe_Release_InstacePool(m_pMonsterDrownedSailorPool);
	Safe_Release_InstacePool(m_pMonsterGiantBeetlePool);
	Safe_Release_InstacePool(m_pMonsterGiantMonkeyPool);
	Safe_Release_InstacePool(m_pMonsterCraftyArachnePool);
	Safe_Release_InstacePool(m_pMonsterLakanPool);
	Safe_Release_InstacePool(m_pMonsterPrionBerserkerPool);
	Safe_Release_InstacePool(m_pMonsterPrionBerserkerBossPool);

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
	Safe_Release_InstacePool(m_pPCOthersArcherPool);
	Safe_Release_InstacePool(m_pPCOthersPriestPool);

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

	Safe_Release_InstacePool(m_pPCWeaponEvent_Wit_Rod_01_Pool);
	Safe_Release_InstacePool(m_pPCWeaponRod18_A_Pool);
	Safe_Release_InstacePool(m_pPCWeaponRod28_B_Pool);
	Safe_Release_InstacePool(m_pPCWeaponRod31_Pool);
	Safe_Release_InstacePool(m_pPCWeaponRod33_B_Pool);

	//Effect
	Safe_Release_InstacePool(m_pEffect_SwordEffect_Pool);
	Safe_Release_InstacePool(m_pEffect_SwordEffect_s_Pool);
	Safe_Release_InstacePool(m_pEffect_IceStorm_Pool);

	Safe_Release_InstacePool(m_pEffect_IceStorm_m_Pool);
	Safe_Release_InstacePool(m_pEffect_IceDecal_Pool);
	Safe_Release_InstacePool(m_pEffect_FireDecal_Pool);
	Safe_Release_InstacePool(m_pEffect_Particle_Pool);
	Safe_Release_InstacePool(m_pEffect_FireRing_Pool);
	Safe_Release_InstacePool(m_pEffect_FrameMesh_Pool);

	Safe_Release_InstacePool(m_pEffect_IceStorm_s_Pool);
	Safe_Release_InstacePool(m_pEffect_DistTrail_Pool);
	Safe_Release_InstacePool(m_pEffect_Texture_Pool);
	Safe_Release_InstacePool(m_pEffect_Shield_Pool);
	Safe_Release_InstacePool(m_pEffect_Axe_Pool);
	Safe_Release_InstacePool(m_pEffect_MagicCircle_Pool);
	Safe_Release_InstacePool(m_pEffect_GridShieldEffect_Pool);
	Safe_Release_InstacePool(m_pEffect_MeshParticleRock0_Pool);
	Safe_Release_InstacePool(m_pEffect_MeshParticleRock1_Pool);
	Safe_Release_InstacePool(m_pEffect_MeshParticleRock2_Pool);
	Safe_Release_InstacePool(m_pEffect_MeshParticleRock3_Pool);
	Safe_Release_InstacePool(m_pEffect_MeshParticleRock4_Pool);


	Safe_Release_InstacePool(m_pEffect_WarningGround_Pool);
	Safe_Release_InstacePool(m_pEffect_RectDecal_Pool);
	Safe_Release_InstacePool(m_pEffect_BossDecal_Pool);
	Safe_Release_InstacePool(m_pEffect_DirParticle_Pool);
	Safe_Release_InstacePool(m_pEffect_BreathEffect_Pool);
	Safe_Release_InstacePool(m_pEffect_Tonado_Pool);

	
}
