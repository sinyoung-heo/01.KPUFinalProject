#pragma once
#include "Include.h"
#include "Base.h"

class CFadeInOut;
class CCollisionTick;
class CCollisionArrow;

class CCrab;
class CCloderA;
class CMonkey;
class CDrownedSailor;
class CGiantBeetle;
class CGiantMonkey;
class CCraftyArachne;
class CLakan;
class CPrionBerserker;
class CPrionBerserkerBoss;
class CCinemaVergos;
class CVergos;

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

class CDmgFont;

//Effect
class CSwordEffect;
class CSwordEffect_s;
class CIceStorm;
class CIceStorm_m;
class CIceStorm_s;
class CFireDecal;
class CIceDecal;
class CParticleEffect;
class CFireRing;
class CFrameMesh;
class CDistTrail;
class CTextureEffect;
class CEffectShield;
class CEffectAxe;
class CMagicCircle;
class CGridShieldEffect;
class CMeshParticleEffect;
class CWarningGround;
class CRectDecal;
class CDirParticleEffect;
class CBreathEffect;
class CBossDecal;
class CTonadoEffect;
class CBossGroundEffect;
class CDragonEffect;
class CBreathParticle;

class CInstancePoolMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CInstancePoolMgr)

private:
	explicit CInstancePoolMgr();
	virtual ~CInstancePoolMgr() = default;

public:
	// FadeInOut
	INSTANCE_POOL_DESC<CFadeInOut>*			Get_FadeInOutPool() { return m_pFadeInOutPool; }
	// DmgFont
	INSTANCE_POOL_DESC<CDmgFont>*			Get_DmgFontPool() { return m_pDmgFontPool; }
	// CollisionTick
	INSTANCE_POOL_DESC<CCollisionTick>*		Get_CollisionTickPool()		{ return m_pCollisionTickPool; }
	INSTANCE_POOL_DESC<CCollisionArrow>*	Get_CollisionArrowPool(const ARROW_POOL_TYPE& eType);
	// Monster
	INSTANCE_POOL_DESC<CCrab>*				Get_MonsterCrabPool()						{ return m_pMonsterCrabPool; }
	INSTANCE_POOL_DESC<CCloderA>*			Get_MonsterCloderAPool()					{ return m_pMonsterCloderAPool; }
	INSTANCE_POOL_DESC<CMonkey>*			Get_MonsterMonkeyPool()						{ return m_pMonsterMonkeyPool; }
	INSTANCE_POOL_DESC<CDrownedSailor>*		Get_MonsterDrownedSailorPool()				{ return m_pMonsterDrownedSailorPool; }
	INSTANCE_POOL_DESC<CGiantBeetle>*		Get_MonsterGiantBeetlePool()				{ return m_pMonsterGiantBeetlePool; }
	INSTANCE_POOL_DESC<CGiantMonkey>*		Get_MonsterGiantMonkeyPool()				{ return m_pMonsterGiantMonkeyPool; }
	INSTANCE_POOL_DESC<CCraftyArachne>*		Get_MonsterCraftyArachnePool()				{ return m_pMonsterCraftyArachnePool; }
	INSTANCE_POOL_DESC<CLakan>*				Get_MonsterLakanPool()						{ return m_pMonsterLakanPool; }
	INSTANCE_POOL_DESC<CPrionBerserker>*	Get_MonsterPrionBerserkerPool()				{ return m_pMonsterPrionBerserkerPool; }
	INSTANCE_POOL_DESC<CPrionBerserkerBoss>*Get_MonsterPrionBerserkerBossPool()			{ return m_pMonsterPrionBerserkerBossPool; }
	INSTANCE_POOL_DESC<CCinemaVergos>*		Get_MonsterCinemaVergosPool()				{ return m_pMonsterCinemaVergosPool; }
	INSTANCE_POOL_DESC<CVergos>*			Get_MonsterVergosPool()						{ return m_pMonsterVergosPool; }

	// NPC
	INSTANCE_POOL_DESC<CNPC_Walker>*		Get_NPCChickenPool()						{ return m_pNPC_Walker_ChickenPool; }
	INSTANCE_POOL_DESC<CNPC_Walker>*		Get_NPCCatPool()							{ return m_pNPC_Walker_CatPool; }
	INSTANCE_POOL_DESC<CNPC_Walker>*		Get_NPCAmanBoyPool()						{ return m_pNPC_Walker_AmanBoyPool; }
	INSTANCE_POOL_DESC<CNPC_Assistant>*		Get_NPCAssistantPool()						{ return m_pNPC_AssistantPool; }
	INSTANCE_POOL_DESC<CNPC_Stander>*		Get_NPCStanderVillagersPool()				{ return m_pNPC_Stander_VillagersPool; }
	INSTANCE_POOL_DESC<CNPC_Stander>*		Get_NPCStanderBarakaPool()					{ return m_pNPC_Stander_Baraka_M_ExtractorPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*		Get_NPCMerchant_Popori_M_MerchantPool()		{ return m_pNPC_Merchant_Popori_M_MerchantPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*		Get_NPCMerchant_Baraka_M_MerchantPool()		{ return m_pNPC_Merchant_Baraka_M_MerchantPool; }
	INSTANCE_POOL_DESC<CNPC_Merchant>*		Get_NPCMerchant_Baraka_M_MystelliumPool()	{ return m_pNPC_Merchant_Baraka_M_MystelliumPool; }
	INSTANCE_POOL_DESC<CNPC_Quest>*			Get_NPCQuestPool()							{ return m_pNPC_QuestPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*		Get_NPCChildren_HumanBoyPool()				{ return m_pNPC_Children_HumanBoyPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*		Get_NPCChildren_HumanGirlPool()				{ return m_pNPC_Children_HumanGirlPool; }
	INSTANCE_POOL_DESC<CNPC_Children>*		Get_NPCChildren_HighelfGirlPool()			{ return m_pNPC_Children_HighelfGirlPool; }
	// PCOthers
	INSTANCE_POOL_DESC<CPCOthersGladiator>* Get_PCOthersGladiatorPool() { return m_pPCOthersGladiatorPool; }
	INSTANCE_POOL_DESC<CPCOthersArcher>*	Get_PCOthersArcherPool()	{ return m_pPCOthersArcherPool; }
	INSTANCE_POOL_DESC<CPCOthersPriest>*	Get_PCOthersPriestPool()	{ return m_pPCOthersPriestPool; }
	// PCWeapon
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	Get_PCWeaponTwoHand(const char& chWeaponType);
	INSTANCE_POOL_DESC<CPCWeaponBow>*		Get_PCWeaponBow(const char& chWeaponType);
	INSTANCE_POOL_DESC<CPCWeaponRod>*		Get_PCWeaponRod(const char& chWeaponType);
	// DmgFont

	//Effect
	INSTANCE_POOL_DESC<CSwordEffect>*		Get_Effect_SwordEffect()		{ return m_pEffect_SwordEffect_Pool; }
	INSTANCE_POOL_DESC<CSwordEffect_s>*		Get_Effect_Sword_s_Effect()		{ return m_pEffect_SwordEffect_s_Pool; }
	INSTANCE_POOL_DESC<CIceStorm>*			Get_Effect_IceStormEffect()		{ return m_pEffect_IceStorm_Pool; }
	INSTANCE_POOL_DESC<CIceStorm_m>*		Get_Effect_IceStorm_m_Effect()	{ return m_pEffect_IceStorm_m_Pool; }
	INSTANCE_POOL_DESC<CIceDecal>*			Get_Effect_IceDecal_Effect()	{ return m_pEffect_IceDecal_Pool; }
	INSTANCE_POOL_DESC<CFireDecal>*			Get_Effect_FireDecal_Effect()	{ return m_pEffect_FireDecal_Pool; }
	INSTANCE_POOL_DESC<CFireRing>*			Get_Effect_FireRing_Effect()	{ return m_pEffect_FireRing_Pool; }
	INSTANCE_POOL_DESC<CFrameMesh>*			Get_Effect_FrameMesh_Effect()	{ return m_pEffect_FrameMesh_Pool; }
	INSTANCE_POOL_DESC<CParticleEffect>*	Get_Effect_Particle_Effect()	{ return m_pEffect_Particle_Pool; }
	INSTANCE_POOL_DESC<CIceStorm_s>*		Get_Effect_IceStorm_s_Effect()	{ return m_pEffect_IceStorm_s_Pool; }
	INSTANCE_POOL_DESC<CDistTrail>*			Get_Effect_DistTrail_Effect()	{ return m_pEffect_DistTrail_Pool; }
	INSTANCE_POOL_DESC<CTextureEffect>*		Get_Effect_TextureEffect()		{ return m_pEffect_Texture_Pool; }
	INSTANCE_POOL_DESC<CEffectShield>*		Get_Effect_ShieldEffect()		{ return m_pEffect_Shield_Pool; }
	INSTANCE_POOL_DESC<CEffectAxe>*			Get_Effect_AxeEffect()			{ return m_pEffect_Axe_Pool; }
	INSTANCE_POOL_DESC<CGridShieldEffect>*	Get_Effect_GridShieldEffect()	{ return m_pEffect_GridShieldEffect_Pool; }
	INSTANCE_POOL_DESC<CMagicCircle>*		Get_Effect_MagicCircleEffect()	{ return m_pEffect_MagicCircle_Pool; }
	INSTANCE_POOL_DESC<CMeshParticleEffect>* Get_Effect_MeshParticleRock0_Effect() { return m_pEffect_MeshParticleRock0_Pool; }
	INSTANCE_POOL_DESC<CMeshParticleEffect>* Get_Effect_MeshParticleRock1_Effect() { return m_pEffect_MeshParticleRock1_Pool; }
	INSTANCE_POOL_DESC<CMeshParticleEffect>* Get_Effect_MeshParticleRock2_Effect() { return m_pEffect_MeshParticleRock2_Pool; }
	INSTANCE_POOL_DESC<CMeshParticleEffect>* Get_Effect_MeshParticleRock3_Effect() { return m_pEffect_MeshParticleRock3_Pool; }
	INSTANCE_POOL_DESC<CMeshParticleEffect>* Get_Effect_MeshParticleRock4_Effect() { return m_pEffect_MeshParticleRock4_Pool; }

	INSTANCE_POOL_DESC<CWarningGround>* Get_Effect_WarningGround_Effect() { return m_pEffect_WarningGround_Pool; }
	INSTANCE_POOL_DESC<CRectDecal>* Get_Effect_RectDecal_Effect() { return m_pEffect_RectDecal_Pool; }
	INSTANCE_POOL_DESC<CBossDecal>* Get_Effect_BossDecal_Effect() { return m_pEffect_BossDecal_Pool; }
	INSTANCE_POOL_DESC<CDirParticleEffect>* Get_Effect_DirParticle_Effect() { return m_pEffect_DirParticle_Pool; }
	INSTANCE_POOL_DESC<CBreathEffect>* Get_Effect_Breath_Effect() { return m_pEffect_BreathEffect_Pool; }
	INSTANCE_POOL_DESC<CTonadoEffect>* Get_Effect_Tonado_Effect() { return m_pEffect_Tonado_Pool; }
	INSTANCE_POOL_DESC<CBossGroundEffect>* Get_Effect_BossGround_Effect() { return m_pEffect_BossGroundEffect_Pool; }
	INSTANCE_POOL_DESC<CDragonEffect>* Get_Effect_Dragon_Effect() { return m_pEffect_DragonEffect_Pool; }

	INSTANCE_POOL_DESC<CBreathParticle>* Get_BreathParticlePool() { return m_pBreathParticle_Pool; }
	
	void Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void Ready_LoadingInstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

private:
	// FadeInOut
	INSTANCE_POOL_DESC<CFadeInOut>*			m_pFadeInOutPool						= nullptr;
	// DmgFont
	INSTANCE_POOL_DESC<CDmgFont>*			m_pDmgFontPool							= nullptr;

	// CollisionTick
	INSTANCE_POOL_DESC<CCollisionTick>*		m_pCollisionTickPool					= nullptr;
	INSTANCE_POOL_DESC<CCollisionArrow>*	m_pCollisionArrowIcePool				= nullptr;
	INSTANCE_POOL_DESC<CCollisionArrow>*	m_pCollisionArrowFirePool				= nullptr;
	INSTANCE_POOL_DESC<CCollisionArrow>*	m_pCollisionArrowLightningPool			= nullptr;

	// Monster
	INSTANCE_POOL_DESC<CCrab>*				m_pMonsterCrabPool						= nullptr;
	INSTANCE_POOL_DESC<CCloderA>*			m_pMonsterCloderAPool					= nullptr;
	INSTANCE_POOL_DESC<CMonkey>*			m_pMonsterMonkeyPool					= nullptr;
	INSTANCE_POOL_DESC<CDrownedSailor>*		m_pMonsterDrownedSailorPool				= nullptr;
	INSTANCE_POOL_DESC<CGiantBeetle>*		m_pMonsterGiantBeetlePool				= nullptr;
	INSTANCE_POOL_DESC<CGiantMonkey>*		m_pMonsterGiantMonkeyPool				= nullptr;
	INSTANCE_POOL_DESC<CCraftyArachne>*		m_pMonsterCraftyArachnePool				= nullptr;
	INSTANCE_POOL_DESC<CLakan>*				m_pMonsterLakanPool						= nullptr;
	INSTANCE_POOL_DESC<CPrionBerserker>*	m_pMonsterPrionBerserkerPool			= nullptr;
	INSTANCE_POOL_DESC<CPrionBerserkerBoss>* m_pMonsterPrionBerserkerBossPool		= nullptr;
	INSTANCE_POOL_DESC<CCinemaVergos>*		m_pMonsterCinemaVergosPool				= nullptr;
	INSTANCE_POOL_DESC<CVergos>*			m_pMonsterVergosPool					= nullptr;

	// NPC
	INSTANCE_POOL_DESC<CNPC_Walker>*		m_pNPC_Walker_ChickenPool               = nullptr;
	INSTANCE_POOL_DESC<CNPC_Walker>*		m_pNPC_Walker_CatPool                   = nullptr;
	INSTANCE_POOL_DESC<CNPC_Walker>*		m_pNPC_Walker_AmanBoyPool               = nullptr;
	INSTANCE_POOL_DESC<CNPC_Assistant>*		m_pNPC_AssistantPool                    = nullptr;
	INSTANCE_POOL_DESC<CNPC_Stander>*		m_pNPC_Stander_VillagersPool            = nullptr;
	INSTANCE_POOL_DESC<CNPC_Stander>*		m_pNPC_Stander_Baraka_M_ExtractorPool   = nullptr;
	INSTANCE_POOL_DESC<CNPC_Merchant>*		m_pNPC_Merchant_Popori_M_MerchantPool   = nullptr;
	INSTANCE_POOL_DESC<CNPC_Merchant>*		m_pNPC_Merchant_Baraka_M_MerchantPool   = nullptr;
	INSTANCE_POOL_DESC<CNPC_Merchant>*		m_pNPC_Merchant_Baraka_M_MystelliumPool = nullptr;
	INSTANCE_POOL_DESC<CNPC_Quest>*			m_pNPC_QuestPool                        = nullptr;
	INSTANCE_POOL_DESC<CNPC_Children>*		m_pNPC_Children_HumanBoyPool            = nullptr;
	INSTANCE_POOL_DESC<CNPC_Children>*		m_pNPC_Children_HumanGirlPool           = nullptr;
	INSTANCE_POOL_DESC<CNPC_Children>*		m_pNPC_Children_HighelfGirlPool         = nullptr;

	// PCOthers
	INSTANCE_POOL_DESC<CPCOthersGladiator>* m_pPCOthersGladiatorPool				= nullptr;
	INSTANCE_POOL_DESC<CPCOthersArcher>*	m_pPCOthersArcherPool					= nullptr;
	INSTANCE_POOL_DESC<CPCOthersPriest>*	m_pPCOthersPriestPool					= nullptr;

	// PCWeapon
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwohand19_A_SM_Pool			= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand27_SM_Pool			= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand29_SM_Pool			= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand31_SM_Pool			= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponTwoHand>*	m_pPCWeaponTwoHand33_B_SM_Pool			= nullptr;

	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow18_A_SM_Pool				= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow23_SM_Pool				= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow27_SM_Pool				= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponBow31_SM_Pool				= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponBow>*		m_pPCWeaponEvent_Season_Bow_01_SM_Pool	= nullptr;

	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponEvent_Wit_Rod_01_Pool		= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod18_A_Pool					= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod28_B_Pool					= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod31_Pool					= nullptr;
	INSTANCE_POOL_DESC<CPCWeaponRod>*		m_pPCWeaponRod33_B_Pool					= nullptr;

	// Effect Gladiator
	INSTANCE_POOL_DESC<CSwordEffect>*		m_pEffect_SwordEffect_Pool						= nullptr;
	INSTANCE_POOL_DESC<CSwordEffect_s>*		m_pEffect_SwordEffect_s_Pool = nullptr;
	INSTANCE_POOL_DESC<CIceStorm>*			m_pEffect_IceStorm_Pool = nullptr;
	INSTANCE_POOL_DESC<CIceStorm_m>*		m_pEffect_IceStorm_m_Pool = nullptr;
	INSTANCE_POOL_DESC<CFireRing>*			m_pEffect_FireRing_Pool = nullptr;
	INSTANCE_POOL_DESC<CFrameMesh>*			m_pEffect_FrameMesh_Pool = nullptr;
	INSTANCE_POOL_DESC<CFireDecal>*			m_pEffect_FireDecal_Pool = nullptr;
	INSTANCE_POOL_DESC<CIceDecal>*			m_pEffect_IceDecal_Pool = nullptr;
	INSTANCE_POOL_DESC<CTonadoEffect>* m_pEffect_Tonado_Pool = nullptr;

	

	// Effect Archer
	INSTANCE_POOL_DESC<CIceStorm_s>*		m_pEffect_IceStorm_s_Pool = nullptr;
	INSTANCE_POOL_DESC<CDistTrail>*			m_pEffect_DistTrail_Pool = nullptr;

	//Effect Priest
	INSTANCE_POOL_DESC<CEffectAxe>*			m_pEffect_Axe_Pool = nullptr;
	INSTANCE_POOL_DESC<CEffectShield>*		m_pEffect_Shield_Pool = nullptr;
	INSTANCE_POOL_DESC<CMagicCircle>*		m_pEffect_MagicCircle_Pool = nullptr;
	INSTANCE_POOL_DESC<CGridShieldEffect>*	m_pEffect_GridShieldEffect_Pool = nullptr;


	// Effect Particle
	INSTANCE_POOL_DESC<CMeshParticleEffect>* m_pEffect_MeshParticleRock0_Pool = nullptr;
	INSTANCE_POOL_DESC<CMeshParticleEffect>* m_pEffect_MeshParticleRock1_Pool = nullptr;
	INSTANCE_POOL_DESC<CMeshParticleEffect>* m_pEffect_MeshParticleRock2_Pool = nullptr;
	INSTANCE_POOL_DESC<CMeshParticleEffect>* m_pEffect_MeshParticleRock3_Pool = nullptr;
	INSTANCE_POOL_DESC<CMeshParticleEffect>* m_pEffect_MeshParticleRock4_Pool = nullptr;
	INSTANCE_POOL_DESC<CParticleEffect>* m_pEffect_Particle_Pool = nullptr;
	INSTANCE_POOL_DESC<CTextureEffect>* m_pEffect_Texture_Pool = nullptr;

	//Effect Boss
	INSTANCE_POOL_DESC<CWarningGround>* m_pEffect_WarningGround_Pool = nullptr;
	INSTANCE_POOL_DESC<CRectDecal>* m_pEffect_RectDecal_Pool = nullptr;
	INSTANCE_POOL_DESC<CBossDecal>* m_pEffect_BossDecal_Pool = nullptr;
	INSTANCE_POOL_DESC<CDirParticleEffect>* m_pEffect_DirParticle_Pool = nullptr;
	INSTANCE_POOL_DESC<CBreathEffect>* m_pEffect_BreathEffect_Pool = nullptr;
	INSTANCE_POOL_DESC<CBossGroundEffect>* m_pEffect_BossGroundEffect_Pool = nullptr;
	INSTANCE_POOL_DESC<CDragonEffect>* m_pEffect_DragonEffect_Pool = nullptr;
	
	INSTANCE_POOL_DESC<CBreathParticle>* m_pBreathParticle_Pool = nullptr;

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
	if (nullptr == pInstacePoolDesc)
		return;

	for (_uint i = 0; i < pInstacePoolDesc->uiInstanceSize; ++i)
		Engine::Safe_Release(pInstacePoolDesc->ppInstances[i]);

	Engine::Safe_Delete_Array(pInstacePoolDesc->ppInstances);
	Engine::Safe_Delete(pInstacePoolDesc);
}