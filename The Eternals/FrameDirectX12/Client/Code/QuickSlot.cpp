#include "stdafx.h"
#include "QuickSlot.h"
#include "Font.h"
#include "DirectInput.h"
#include "InventoryEquipmentMgr.h"
#include "QuickSlotMgr.h"

CQuickSlot::CQuickSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
	, m_pQuickSlotMgr(CQuickSlotMgr::Get_Instance())
{
}

HRESULT CQuickSlot::Ready_GameObject(wstring wstrRootObjectTag,
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
	Engine::FAILED_CHECK_RETURN(CGameUIChild::Ready_GameObject(wstrRootObjectTag, 
															   wstrObjectTag,
															   wstrDataFilePath,
															   vPos,
															   vScale,
															   bIsSpriteAnimation,
															   fFrameSpeed,
															   vRectOffset,
															   vRectScale,
															   iUIDepth,
															   true, L"Font_BinggraeMelona11"), E_FAIL);

	m_bIsActive = true;

	m_chCurSlotName = EMPTY_SLOT;
	m_chPreSlotName = EMPTY_SLOT;

	// Slot Frame
	FRAME tFrame;
	tFrame.fFrameCnt = 1.0f;
	tFrame.fCurFrame = 0.0f;
	tFrame.fSceneCnt = 1.0f;
	tFrame.fCurScene = 0.0f;

	Engine::CGameObject* pSlotFrame = CGameUIChild::Create(m_pGraphicDevice, m_pCommandList,
														   wstrRootObjectTag, 
														   wstrObjectTag,
														   L"",
														   vPos,
														   _vec3(0.086f, 0.88f, 1.0f),
														   bIsSpriteAnimation,
														   fFrameSpeed,
														   _vec3(0.0f),
														   _vec3(0.0f),
														   iUIDepth - 1);
	m_pSlotFrame = static_cast<CGameUIChild*>(pSlotFrame);
	Engine::FAILED_CHECK_RETURN(m_pSlotFrame->SetUp_TexDescriptorHeap(L"S1UI_WareHouse", tFrame, 2), E_FAIL);
	m_pSlotFrame->Set_IsActive(m_bIsActive);

	// Slot Icon
	Engine::CGameObject* pSlotIcon = CGameUIChild::Create(m_pGraphicDevice, m_pCommandList,
														   wstrRootObjectTag, 
														   wstrObjectTag,
														   L"",
														   vPos,
														   _vec3(0.069f, 0.7f, 1.0f),
														   bIsSpriteAnimation,
														   fFrameSpeed,
														   _vec3(0.0f),
														   _vec3(0.0f),
														   iUIDepth - 1);
	m_pSlotIcon = static_cast<CGameUIChild*>(pSlotIcon);
	Engine::FAILED_CHECK_RETURN(m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, m_uiSlotIncoIdx), E_FAIL);
	m_pSlotIcon->Set_IsActive(m_bIsActive);

	// PotionCnt Font
	m_pFontPotionCnt = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_BinggraeMelona16"));
	Engine::NULL_CHECK_RETURN(m_pFontPotionCnt, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFontPotionCnt->Ready_GameObject(L"", _vec2(0.0f, 0.f), D2D1::ColorF::White), E_FAIL);
	m_pFontPotionCnt->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFontPotionCnt->Set_Text(L"");


	m_pFont->Set_Color(D2D1::ColorF::White);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CQuickSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CQuickSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;

	SetUp_SlotIcon();
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CQuickSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	KeyInput_MouseButton(fTimeDelta);
	
	if (m_bIsOnMouse)
	{
		m_pSlotFrame->Update_GameObject(fTimeDelta);
		m_pSlotFrame->LateUpdate_GameObject(fTimeDelta);
	}

	if (m_chCurSlotName != EMPTY_SLOT)
	{
		m_pSlotIcon->Update_GameObject(fTimeDelta);
		m_pSlotIcon->LateUpdate_GameObject(fTimeDelta);
	}

	if (m_chCurSlotName == Potion_HP || m_chCurSlotName == Potion_MP)
		SetUp_FontPotionCnt(fTimeDelta);

	SetUp_FontDIKKey(fTimeDelta);

	return NO_EVENT;
}

void CQuickSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CQuickSlot::KeyInput_MouseButton(const _float& fTimeDelta)
{
	// 슬롯 정보 스왑
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressingLB)
	{
		if (!g_bIsOpenShop)
		{
			// QuickSlot 정보 교환.
			if (m_pQuickSlotMgr->Get_IsQuickSlotSwapState())
			{
				vector<CQuickSlot*> vecQuickSlot = m_pQuickSlotMgr->Get_QuickSlotList();
				_uint uiSelectIdx = m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Get_QuickSlotIdx();
				if (uiSelectIdx == m_uiIdx)
				{
					m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Set_CurQuickSlotName(EMPTY_SLOT);
					m_pQuickSlotMgr->Set_IsQuickSlotSwapState(false);
					m_bIsKeyPressingLB = false;
					return;
				}

				CQuickSlot*	pSelectQuickSlot = vecQuickSlot[uiSelectIdx];
				char chSelectSlotName = pSelectQuickSlot->Get_QuickSlotName();

				// SelectSlot에 현재 슬롯의 정보를 저장.
				pSelectQuickSlot->Set_CurQuickSlotName(m_chCurSlotName);

				// 현재 슬롯의 정보에 SelectSlot의 정보를 저장.
				m_chCurSlotName = chSelectSlotName;


				m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Set_CurQuickSlotName(EMPTY_SLOT);
				m_pQuickSlotMgr->Set_IsQuickSlotSwapState(false);
				m_bIsKeyPressingLB = false;

				return;
			}

			if (!m_pQuickSlotMgr->Get_IsQuickSlotSwapState() &&
				!CInventoryEquipmentMgr::Get_Instance()->Get_IsInventoryItemSwapState())
			{
				if (EMPTY_SLOT != m_chCurSlotName)
					m_pQuickSlotMgr->Set_IsQuickSlotSwapState(true);
				else
					m_pQuickSlotMgr->Set_IsQuickSlotSwapState(false);

				m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Set_CurQuickSlotName(m_chCurSlotName);
				m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Set_QuickSlotIdx(m_uiIdx);

				m_bIsKeyPressingLB = false;
				return;
			}
		}
	}

	// 슬롯 정보 스왑 해제
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_RB) && 
		m_bIsKeyPressingRB)
	{
		if (!g_bIsOpenShop)
		{
			if (m_pQuickSlotMgr->Get_IsQuickSlotSwapState())
			{
				m_pQuickSlotMgr->Set_IsQuickSlotSwapState(false);
				m_pQuickSlotMgr->Get_QuickSlotSwapSlot()->Set_CurQuickSlotName(EMPTY_SLOT);
				m_bIsKeyPressingRB = false;
				return;
			}
		}
	}

	m_bIsKeyPressingLB = false;
	m_bIsKeyPressingRB = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		m_bIsOnMouse = true;

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_bIsKeyPressingLB = true;
		}
		else if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_RB))
		{
			m_bIsKeyPressingRB = true;
		}
	}
	else
	{
		m_bIsOnMouse = false;
	}
}

void CQuickSlot::SetUp_SlotIcon()
{
	if (m_chCurSlotName == m_chPreSlotName)
		return;

	FRAME tFrame;
	tFrame.fFrameCnt = 1.0f;
	tFrame.fCurFrame = 0.0f;
	tFrame.fSceneCnt = 1.0f;
	tFrame.fCurScene = 0.0f;

	Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

	switch (m_chCurSlotName)
	{
	case EMPTY_SLOT:
	{
	}
		break;

	case QUCKSLOT_POTION_HP:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"ItemPotion", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"HP_POTION", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_POTION_MP:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"ItemPotion", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"MP_POTION", m_uiDIK_Key);
	}
		break;

	// PC Gladiator Skill
	case QUCKSLOT_SKILL_STINGER_BLADE:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"STINGER_BLADE", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_CUTTING_SLASH:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"CUTTING_SLASH", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_JAW_BREAKER:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"JAW_BREAKER", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_GAIA_CRUSH:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"GAIA_CRUSH", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_DRAW_SWORD:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 4);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"DRAW_SWORD", m_uiDIK_Key);
	}
		break;

	// PC Archer Skill
	case QUCKSLOT_SKILL_RAPID_SHOT:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"RAPID_SHOT", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_ARROW_SHOWER:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"ARROW_SHOWER", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_ESCAPING_BOMB:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"ESCAPING_BOMB", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_ARROW_FALL:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"ARROW_FALL", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_CHARGE_ARROW:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 4);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"CHARGE_ARROW", m_uiDIK_Key);
	}
		break;

	// PC Priest Skill
	case QUCKSLOT_SKILL_AURA_ON:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"AURA_ON", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_PURIFY:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"PURIFY", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_HEAL:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"HEAL", m_uiDIK_Key);
	}
		break;

	case QUCKSLOT_SKILL_MP_CHARGE:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThilsPlayerSkillKeyInput(L"MP_CHARGE", m_uiDIK_Key);
	}
		break;

	default:
		break;
	}

	m_chPreSlotName = m_chCurSlotName;
}

void CQuickSlot::SetUp_FontPotionCnt(const _float& fTimeDelta)
{
	CInventoryItemSlot* pInventorySlot = nullptr;

	if (m_chCurSlotName == Potion_HP)
		pInventorySlot = CInventoryEquipmentMgr::Get_Instance()->Get_HpPotionSlot();
	else
		pInventorySlot = CInventoryEquipmentMgr::Get_Instance()->Get_MpPotionSlot();

	if (nullptr == pInventorySlot)
		m_uiCnt = 0;
	else
		m_uiCnt = pInventorySlot->Get_CurItemCnt();

	if (nullptr != m_pFontPotionCnt)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -19.0f;
		vPos.y += -2.0f;

		_tchar	szText[MIN_STR] = L"";
		wstring wstrText        = L"%d";
		wsprintf(szText, wstrText.c_str(), m_uiCnt);

		m_pFontPotionCnt->Set_Text(szText);
		m_pFontPotionCnt->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFontPotionCnt->Update_GameObject(fTimeDelta);
	}
}

void CQuickSlot::SetUp_FontDIKKey(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -18.0f;
		vPos.y += -22.0f;

		_uint uiDIKKey = m_uiDIK_Key - 1;
		if (uiDIKKey == 10)
			uiDIKKey = 0;

		_tchar	szText[MIN_STR] = L"";
		wstring wstrText = L"%d";
		wsprintf(szText, wstrText.c_str(), uiDIKKey);

		m_pFont->Set_Text(szText);
		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CQuickSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
												wstring wstrRootObjectTag,
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
	CQuickSlot* pInstance = new CQuickSlot(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrRootObjectTag,
										   wstrObjectTag,
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

void CQuickSlot::Free()
{
	CGameUIChild::Free();

	Engine::Safe_Release(m_pSlotFrame);
	Engine::Safe_Release(m_pSlotIcon);
	Engine::Safe_Release(m_pFontPotionCnt);
}
