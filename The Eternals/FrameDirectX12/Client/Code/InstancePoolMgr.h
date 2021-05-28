#pragma once
#include "Include.h"
#include "Base.h"

class CCollisionTick;
class CCollisionArrow;

class CCrab;
class CCloderA;
class CMonkey;
class CDrownedSailor;
class CGiantBeetle;
class CGiantMonkey;
class CCraftyArachne;

class CNPC_Walker;
class CNPC_Assistant;
class CNPC_Stander;
class CNPC_Merchant;
class CNPC_Quest;
class CNPC_Children;

class CPCOthersGladiator;
class CPCOthersArcher;
class CPCOthersPriest;

class CPCWeaponTwoHand;
class CPCWeaponBow;
class CPCWeaponRod;

class CInstancePoolMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CInstancePoolMgr)

private:
	explicit CInstancePoolMgr();
	virtual ~CInstancePoolMgr() = default;

public:
	// CollisionTick
	INSTANCE_POOL_DESC<CCollisionTick>*		Get_CollisionTickPool()		{ return m_pCollisionTickPool; }
	INSTANCE_POOL_DESC<CCollisionArrow>*	Get_CollisionArrowPool()	{ return m_pCollisionArrowPool; }
	// Monster
	INSTANCE_POOL_DESC<CCrab>*			Get_MonsterCrabPool()			{ return m_pMonsterCrabPool; }
	INSTANCE_POOL_DESC<CCloderA>*		Get_MonsterCloderAPool()		{ return m_pMonsterCloderAPool; }
	INSTANCE_POOL_DESC<CMonkey>*		Get_MonsterMonkeyPool()			{ return m_pMonsterMonkeyPool; }
	INSTANCE_POOL_DESC<CDrownedSailor>* Get_MonsterDrownedSailorPool()	{ return m_pMonsterDrownedSailorPool; }
	INSTANCE_POOL_DESC<CGiantBeetle>*	Get_MonsterGiantBeetlePool()	{ return m_pMonsterGiantBeetlePool; }
	INSTANCE_POOL_DESC<CGiantMonkey>*	Get_MonsterGiantMonkeyPool()	{ return m_pMonsterGiantMonkeyPool; }
	INSTANCE_POOL_DESC<CCraftyArachne>* Get_MonsterCraftyArachnePool()	{ return m_pMonsterCraftyArachnePool; }
	// NPC
	INSTANCE_POOL_DESC<CNPC_Walker>*	Get_NPCChickenPool()						{ return m_pNPC_Walker_ChickenPool; }
	INSTANCE_POOL_DESC<CNPC_Walker>*	Get_NPCCatPool()							{ return m_pNPC_Walker_CatPool; }
	INSTANCE_POOL_DESC<CNPC_Walker>*	Get_NPCAmanBoyPool()						{ return m_pNPC_Walker_AmanBoyPool; }
	INSTANCE_POOL_DESC<CNPC_Assistant>* Get_NPCAssistantPool()						{ return m_pNPC_AssistantPool; }
	INSTANCE_POOL_DESC<CNPC_Stander>*	Get_NPCStanderVillagersPool()				{ return m_pNPC_Stander_VillagersPool; }
	INSTANCE_POOL_DESC<CNPC_Stander>*	Get_NPCStanderBarakaPool()					{ return m_pNPC_Stander_Baraka_M_ExtractorPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*	Get_NPCMerchant_Popori_M_MerchantPool()		{ return m_pNPC_Merchant_Popori_M_MerchantPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*	Get_NPCMerchant_Baraka_M_MerchantPool()		{ return m_pNPC_Merchant_Baraka_M_MerchantPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*	Get_NPCMerchant_Baraka_M_MystelliumPool()	{ return m_pNPC_Merchant_Baraka_M_MystelliumPool; }
	INSTANCE_POOL_DESC<CNPC_Quest>*		Get_NPCQuestPool()							{ return m_pNPC_QuestPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*	Get_NPCChildren_HumanBoyPool()				{ return m_pNPC_Children_HumanBoyPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*	Get_NPCChildren_HumanGirlPool()				{ return m_pNPC_Children_HumanGirlPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*	Get_NPCChildren_HighelfGirlPool()			{ return m_pNPC_Children_HighelfGirlPool; }
	// PCOthers
	INSTANCE_POOL_DESC<CPCOthersGladiator>* Get_PCOthersGladiatorPool() { return m_pPCOthersGladiatorPool; }
	INSTANCE_POOL_DESC<CPCOthersArcher>*	Get_PCOthersArcherPool()	{ return m_pPCOthersArcherPool; }
	INSTANCE_POOL_DESC<CPCOthersPriest>*	Get_PCOthersPriestPool()	{ return m_pPCOthersPriestPool; }
	// PCWeapon
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	Get_PCWeaponTwoHand(const char& chWeaponType);
	INSTANCE_POOL_DESC<CPCWeaponBow>*		Get_PCWeaponBow(const char& chWeaponType);
	INSTANCE_POOL_DESC<CPCWeaponRod>*		Get_PCWeaponRod(const char& chWeaponType);


	void Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

private:
	// CollisionTick
	INSTANCE_POOL_DESC<CCollisionTick>*		m_pCollisionTickPool;
	INSTANCE_POOL_DESC<CCollisionArrow>*	m_pCollisionArrowPool;

	// Monster
	INSTANCE_POOL_DESC<CCrab>*			m_pMonsterCrabPool;
	INSTANCE_POOL_DESC<CCloderA>*		m_pMonsterCloderAPool;
	INSTANCE_POOL_DESC<CMonkey>*		m_pMonsterMonkeyPool;
	INSTANCE_POOL_DESC<CDrownedSailor>*	m_pMonsterDrownedSailorPool;
	INSTANCE_POOL_DESC<CGiantBeetle>*	m_pMonsterGiantBeetlePool;
	INSTANCE_POOL_DESC<CGiantMonkey>*	m_pMonsterGiantMonkeyPool;
	INSTANCE_POOL_DESC<CCraftyArachne>* m_pMonsterCraftyArachnePool;

	// NPC
	INSTANCE_POOL_DESC<CNPC_Walker>*	m_pNPC_Walker_ChickenPool;
	INSTANCE_POOL_DESC<CNPC_Walker>*	m_pNPC_Walker_CatPool;
	INSTANCE_POOL_DESC<CNPC_Walker>*	m_pNPC_Walker_AmanBoyPool;
	INSTANCE_POOL_DESC<CNPC_Assistant>* m_pNPC_AssistantPool;
	INSTANCE_POOL_DESC<CNPC_Stander>*	m_pNPC_Stander_VillagersPool;
	INSTANCE_POOL_DESC<CNPC_Stander>*	m_pNPC_Stander_Baraka_M_ExtractorPool;
	INSTANCE_POOL_DESC<CNPC_Merchant>*	m_pNPC_Merchant_Popori_M_MerchantPool;
	INSTANCE_POOL_DESC<CNPC_Merchant>*	m_pNPC_Merchant_Baraka_M_MerchantPool;
	INSTANCE_POOL_DESC<CNPC_Merchant>*	m_pNPC_Merchant_Baraka_M_MystelliumPool;
	INSTANCE_POOL_DESC<CNPC_Quest>*		m_pNPC_QuestPool;
	INSTANCE_POOL_DESC<CNPC_Children>*	m_pNPC_Children_HumanBoyPool;
	INSTANCE_POOL_DESC<CNPC_Children>*	m_pNPC_Children_HumanGirlPool;
	INSTANCE_POOL_DESC<CNPC_Children>*	m_pNPC_Children_HighelfGirlPool;

	// PCOthers
	INSTANCE_POOL_DESC<CPCOthersGladiator>* m_pPCOthersGladiatorPool;
	INSTANCE_POOL_DESC<CPCOthersArcher>*	m_pPCOthersArcherPool;
	INSTANCE_POOL_DESC<CPCOthersPriest>*	m_pPCOthersPriestPool;

	// PCWeapon
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwohand19_A_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand27_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand29_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand31_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand33_B_SM_Pool;

	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow18_A_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow23_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow27_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow31_SM_Pool;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponEvent_Season_Bow_01_SM_Pool;

	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponEvent_Wit_Rod_01_Pool;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod18_A_Pool;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod28_B_Pool;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod31_Pool;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod33_B_Pool;

private:
	virtual void Free();
};


/*__________________________________________________________________________________________________________
[ InstacePool Template Method  ]
____________________________________________________________________________________________________________*/
template<class T1>
void Ready_InstacePool(ID3D12Device* pGraphicDevice, 
					   ID3D12GraphicsCommandList* pCommandList, 
					   INSTANCE_POOL_DESC<T1>** pInstacePoolDesc,
					   const _uint& uiSize)
{
	(*pInstacePoolDesc)                 = new INSTANCE_POOL_DESC<T1>;
	(*pInstacePoolDesc)->ppInstances    = T1::Create_InstancePool(pGraphicDevice, pCommandList, uiSize);
	(*pInstacePoolDesc)->uiInstanceSize = uiSize;
}

template<class T1>
void Ready_InstacePool(ID3D12Device* pGraphicDevice, 
					   ID3D12GraphicsCommandList* pCommandList, 
					   INSTANCE_POOL_DESC<T1>** pInstacePoolDesc,
					   wstring wstrMeshTag,
					   const _uint& uiSize)
{
	(*pInstacePoolDesc)                 = new INSTANCE_POOL_DESC<T1>;
	(*pInstacePoolDesc)->ppInstances    = T1::Create_InstancePool(pGraphicDevice, pCommandList, wstrMeshTag, uiSize);
	(*pInstacePoolDesc)->uiInstanceSize = uiSize;
}


template<class T1>
Engine::CGameObject* Pop_Instance(INSTANCE_POOL_DESC<T1>* pInstacePoolDesc)
{
	_uint uiIdx   = pInstacePoolDesc->uiCurrentIdx;
	_uint uiSize  = pInstacePoolDesc->uiInstanceSize;

	if (uiIdx == uiSize)
	{
		for (_uint i = 0; i < uiSize; ++i)
		{
			if (!pInstacePoolDesc->ppInstances[i]->Get_IsUsingInstance())
			{
				pInstacePoolDesc->uiCurrentIdx = i + 1;
				pInstacePoolDesc->ppInstances[i]->AddRef();

				return pInstacePoolDesc->ppInstances[i];
			}
		}
	}
	else
	{
		// CurrentIdx 사용중인지 검사.
		if (!pInstacePoolDesc->ppInstances[uiIdx]->Get_IsUsingInstance())
		{
			++pInstacePoolDesc->uiCurrentIdx;
			pInstacePoolDesc->ppInstances[uiIdx]->AddRef();

			return pInstacePoolDesc->ppInstances[uiIdx];
		}
		// 시용중이라면 사용중이지 않은 Indstnace 탐색.
		else
		{
			for (_uint i = uiIdx + 1; i < uiSize; ++i)
			{
				if (!pInstacePoolDesc->ppInstances[i]->Get_IsUsingInstance())
				{
					pInstacePoolDesc->uiCurrentIdx = i + 1;
					pInstacePoolDesc->ppInstances[i]->AddRef();

					return pInstacePoolDesc->ppInstances[i];
				}
			}

			for (_uint i = 0; i < uiIdx; ++i)
			{
				if (!pInstacePoolDesc->ppInstances[i]->Get_IsUsingInstance())
				{
					pInstacePoolDesc->uiCurrentIdx = i + 1;
					pInstacePoolDesc->ppInstances[i]->AddRef();

					return pInstacePoolDesc->ppInstances[i];
				}
			}
		}
	}

	return nullptr;
}

template<class T1>
void Return_Instance(INSTANCE_POOL_DESC<T1>* pInstacePoolDesc, const _uint& uiInstanceIdx)
{
	pInstacePoolDesc->ppInstances[uiInstanceIdx]->Set_IsUsingInstance(false);
	pInstacePoolDesc->ppInstances[uiInstanceIdx]->Set_IsReturnObject(false);
	Engine::Safe_Release(pInstacePoolDesc->ppInstances[uiInstanceIdx]);
}

template<class T1>
void Safe_Release_InstacePool(INSTANCE_POOL_DESC<T1>* pInstacePoolDesc)
{
	for (_uint i = 0; i < pInstacePoolDesc->uiInstanceSize; ++i)
		Engine::Safe_Release(pInstacePoolDesc->ppInstances[i]);

	Engine::Safe_Delete_Array(pInstacePoolDesc->ppInstances);
	Engine::Safe_Delete(pInstacePoolDesc);
}