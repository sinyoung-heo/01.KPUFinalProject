#include "stdafx.h"
#include "QuickSlotSwapSlot.h"
#include "DescriptorHeapMgr.h"

CQuickSlotSwapSlot::CQuickSlotSwapSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CQuickSlotSwapSlot::Ready_GameObject(wstring wstrObjectTag,
											 wstring wstrDataFilePath,
											 const _vec3& vPos,
											 const _vec3& vScale, 
											 const _bool& bIsSpriteAnimation, 
											 const _float& fFrameSpeed,
											 const _vec3& vRectOffset,
											 const _vec3& vRectScale,
											 const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::Ready_GameObject(wstrObjectTag,
															  wstrDataFilePath,
															  vPos,
															  vScale,
															  bIsSpriteAnimation,
															  fFrameSpeed,
															  vRectOffset,
															  vRectScale,
															  iUIDepth), E_FAIL);

	m_bIsActive = false;

	m_chCurSlotName = EMPTY_SLOT;
	m_chPreSlotName = EMPTY_SLOT;

	return S_OK;
}

HRESULT CQuickSlotSwapSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CQuickSlotSwapSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	SetUp_SlotIcon();

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	// 마우스 커서 포인터 위치.
	POINT pt = CMouseCursorMgr::Get_Instance()->Get_CursorPoint();
	m_pTransCom->m_vPos = _vec3(pt.x - 15.0f, pt.y - 15.0f, 1.0f);

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CQuickSlotSwapSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CQuickSlotSwapSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}


void CQuickSlotSwapSlot::SetUp_SlotIcon()
{
	if (m_chCurSlotName == m_chPreSlotName)
		return;

	switch (m_chCurSlotName)
	{
	case EMPTY_SLOT:
	{
		m_bIsActive = false;
		m_chPreSlotName = m_chCurSlotName;
		return;
	}
		break;

	case QUCKSLOT_POTION_HP:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemPotion");
		m_uiTexIdx = 0;
	}
		break;

	case QUCKSLOT_POTION_MP:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemPotion");
		m_uiTexIdx = 1;
	}
		break;

	// PC Gladiator Skill
	case QUCKSLOT_SKILL_STINGER_BLADE:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Gladiator");
		m_uiTexIdx = 0;
	}
		break;

	case QUCKSLOT_SKILL_CUTTING_SLASH:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Gladiator");
		m_uiTexIdx = 1;
	}
		break;

	case QUCKSLOT_SKILL_JAW_BREAKER:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Gladiator");
		m_uiTexIdx = 2;
	}
		break;

	case QUCKSLOT_SKILL_GAIA_CRUSH:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Gladiator");
		m_uiTexIdx = 3;
	}
		break;

	case QUCKSLOT_SKILL_DRAW_SWORD:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Gladiator");
		m_uiTexIdx = 4;
	}
		break;

	// PC Archer Skill
	case QUCKSLOT_SKILL_RAPID_SHOT:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Archer");
		m_uiTexIdx = 0;
	}
		break;

	case QUCKSLOT_SKILL_ARROW_SHOWER:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Archer");
		m_uiTexIdx = 1;
	}
		break;

	case QUCKSLOT_SKILL_ESCAPING_BOMB:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Archer");
		m_uiTexIdx = 2;
	}
		break;

	case QUCKSLOT_SKILL_ARROW_FALL:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Archer");
		m_uiTexIdx = 3;
	}
		break;

	case QUCKSLOT_SKILL_CHARGE_ARROW:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Archer");
		m_uiTexIdx = 4;
	}
		break;

	// PC Priest Skill
	case QUCKSLOT_SKILL_AURA_ON:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Priest");
		m_uiTexIdx = 0;
	}
		break;

	case QUCKSLOT_SKILL_PURIFY:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Priest");
		m_uiTexIdx = 1;
	}
		break;

	case QUCKSLOT_SKILL_HEAL:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Priest");
		m_uiTexIdx = 2;
	}
		break;

	case QUCKSLOT_SKILL_MP_CHARGE:
	{
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"SkillIcon_Priest");
		m_uiTexIdx = 3;
	}
		break;

	default:
		break;
	}

	m_bIsActive = true;
	m_chPreSlotName = m_chCurSlotName;
}


Engine::CGameObject* CQuickSlotSwapSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
												wstring wstrObjectTag,							   
												wstring wstrDataFilePath,
												const _vec3& vPos,
												const _vec3& vScale,
												const _bool& bIsSpriteAnimation,
												const _float& fFrameSpeed,
												const _vec3& vRectOffset,
												const _vec3& vRectScale,
												const _long& iUIDepth)
{
	CQuickSlotSwapSlot* pInstance = new CQuickSlotSwapSlot(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrObjectTag,
										   wstrDataFilePath,
										   vPos,
										   vScale,
										   bIsSpriteAnimation,
										   fFrameSpeed,
										   vRectOffset,
										   vRectScale,
										   iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CQuickSlotSwapSlot::Free()
{
	CGameUIRoot::Free();
}
