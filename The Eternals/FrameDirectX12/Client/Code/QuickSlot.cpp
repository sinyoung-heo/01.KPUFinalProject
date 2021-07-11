#include "stdafx.h"
#include "QuickSlot.h"
#include "Font.h"
#include "DirectInput.h"
#include "InventoryEquipmentMgr.h"
#include "QuickSlotMgr.h"
#include "CoolTime.h"

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

	// CoolTime
	Engine::CGameObject* pCoolTime = CCoolTime::Create(m_pGraphicDevice, m_pCommandList,
													   wstrRootObjectTag, 
													   wstrObjectTag,
													   L"",
													   vPos,
													   _vec3(0.086f, 0.88f, 1.0f),
													   bIsSpriteAnimation,
													   fFrameSpeed,
													   _vec3(0.0f),
													   _vec3(0.0f),
													   iUIDepth - 2);
	m_pUICoolDownTime = static_cast<CCoolTime*>(pCoolTime);

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

	//// CoolDownShader
	//m_pShaderCoolDownCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pShaderCoolDownCom, E_FAIL);
	//m_pShaderCoolDownCom->AddRef();
	//m_pShaderCoolDownCom->Set_PipelineStatePass(12);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ShaderCoolDown", m_pShaderCoolDownCom);

	// PotionCnt Font
	m_pFontPotionCnt = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_BinggraeMelona16"));
	Engine::NULL_CHECK_RETURN(m_pFontPotionCnt, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFontPotionCnt->Ready_GameObject(L"", _vec2(0.0f, 0.f), D2D1::ColorF::White), E_FAIL);
	m_pFontPotionCnt->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFontPotionCnt->Set_Text(L"");

	// CoolDownTime Font
	m_pFontSkillCoolDownTime = static_cast<Engine::CFont*>(m_pObjectMgr->Clone_GameObjectPrototype(L"Font_BinggraeMelona20"));
	Engine::NULL_CHECK_RETURN(m_pFontSkillCoolDownTime, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFontSkillCoolDownTime->Ready_GameObject(L"", _vec2(0.0f, 0.f), D2D1::ColorF::White), E_FAIL);
	m_pFontSkillCoolDownTime->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFontSkillCoolDownTime->Set_Text(L"");

	m_pFont->Set_Color(D2D1::ColorF::White);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CQuickSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);
	// m_pShaderCoolDownCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CQuickSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;

	SetUp_SlotIcon();
	SetUp_SkillCoolDownTime();

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

	if (0.0f != m_fCurCoolDownTime && L"" != m_wstrCoolDownTag)
	{
		m_pUICoolDownTime->Update_GameObject(fTimeDelta);
		m_pUICoolDownTime->LateUpdate_GameObject(fTimeDelta);
	}

	SetUp_FontDIKKey(fTimeDelta);
	SetUp_FontCoolDownTime(fTimeDelta);

	return NO_EVENT;
}

void CQuickSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
	// Render_CoolDownTime(fTimeDelta);
}

void CQuickSlot::Render_CoolDownTime(const _float& fTimeDelta)
{
	//if (0.0f != m_fCurCoolDownTime && L"" != m_wstrCoolDownTag)
	//{
	//	/*__________________________________________________________________________________________________________
	//	[ Set ConstantBuffer Data ]
	//	____________________________________________________________________________________________________________*/
	//	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	//	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	//	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	//	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	//	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	//	tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
	//	tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	//	tCB_ShaderTexture.fAlpha    = 0.75f;

	//	m_pShaderCoolDownCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);

	//	m_pShaderCoolDownCom->Begin_Shader(Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"BackBuffer"),
	//									   0,
	//									   1,
	//									   Engine::MATRIXID::ORTHO);
	//	m_pBufferCom->Begin_Buffer();
	//	m_pBufferCom->Render_Buffer();
	//}
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
		m_wstrCoolDownTag = L"";
		m_fMaxCoolDownTime = 0.0f;
		m_fCurCoolDownTime = 0.0f;
	}
		break;

	case QUCKSLOT_POTION_HP:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"ItemPotion", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"HP_POTION", m_uiDIK_Key);

		m_wstrCoolDownTag = L"HP_POTION";
	}
		break;

	case QUCKSLOT_POTION_MP:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"ItemPotion", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"MP_POTION", m_uiDIK_Key);

		m_wstrCoolDownTag = L"MP_POTION";
	}
		break;

	// PC Gladiator Skill
	case QUCKSLOT_SKILL_STINGER_BLADE:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"STINGER_BLADE", m_uiDIK_Key);

		m_wstrCoolDownTag = L"STINGER_BLADE";
	}
		break;

	case QUCKSLOT_SKILL_CUTTING_SLASH:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"CUTTING_SLASH", m_uiDIK_Key);

		m_wstrCoolDownTag = L"CUTTING_SLASH";
	}
		break;

	case QUCKSLOT_SKILL_JAW_BREAKER:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"JAW_BREAKER", m_uiDIK_Key);

		m_wstrCoolDownTag = L"JAW_BREAKER";
	}
		break;

	case QUCKSLOT_SKILL_GAIA_CRUSH:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"GAIA_CRUSH", m_uiDIK_Key);

		m_wstrCoolDownTag = L"GAIA_CRUSH";
	}
		break;

	case QUCKSLOT_SKILL_DRAW_SWORD:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Gladiator", tFrame, 4);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"DRAW_SWORD", m_uiDIK_Key);

		m_wstrCoolDownTag = L"DRAW_SWORD";
	}
		break;

	// PC Archer Skill
	case QUCKSLOT_SKILL_RAPID_SHOT:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"RAPID_SHOT", m_uiDIK_Key);

		m_wstrCoolDownTag = L"RAPID_SHOT";
	}
		break;

	case QUCKSLOT_SKILL_ARROW_SHOWER:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"ARROW_SHOWER", m_uiDIK_Key);

		m_wstrCoolDownTag = L"ARROW_SHOWER";
	}
		break;

	case QUCKSLOT_SKILL_ESCAPING_BOMB:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"ESCAPING_BOMB", m_uiDIK_Key);

		m_wstrCoolDownTag = L"ESCAPING_BOMB";
	}
		break;

	case QUCKSLOT_SKILL_ARROW_FALL:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"ARROW_FALL", m_uiDIK_Key);

		m_wstrCoolDownTag = L"ARROW_FALL";
	}
		break;

	case QUCKSLOT_SKILL_CHARGE_ARROW:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Archer", tFrame, 4);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"CHARGE_ARROW", m_uiDIK_Key);

		m_wstrCoolDownTag = L"CHARGE_ARROW";
	}
		break;

	// PC Priest Skill
	case QUCKSLOT_SKILL_AURA_ON:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 0);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"AURA_ON", m_uiDIK_Key);

		m_wstrCoolDownTag = L"AURA_ON";
	}
		break;

	case QUCKSLOT_SKILL_PURIFY:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 1);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"PURIFY", m_uiDIK_Key);

		m_wstrCoolDownTag = L"PURIFY";
	}
		break;

	case QUCKSLOT_SKILL_HEAL:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 2);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"HEAL", m_uiDIK_Key);

		m_wstrCoolDownTag = L"HEAL";
	}
		break;

	case QUCKSLOT_SKILL_MP_CHARGE:
	{
		m_pSlotIcon->SetUp_TexDescriptorHeap(L"SkillIcon_Priest", tFrame, 3);
		if (nullptr != pThisPlayer)
			pThisPlayer->Set_ThisPlayerSkillKeyInput(L"MP_CHARGE", m_uiDIK_Key);

		m_wstrCoolDownTag = L"MP_CHARGE";
	}
		break;

	default:

		break;
	}

	m_chPreSlotName = m_chCurSlotName;
}

void CQuickSlot::SetUp_SkillCoolDownTime()
{
	if (L"" == m_wstrCoolDownTag)
		return;

	Engine::CGameObject*			pThisPlayer        = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	Engine::SKILL_COOLDOWN_DESC*	pSkillCoolDownDesc = pThisPlayer->Get_ThisPlayerSkillCoolDown(m_wstrCoolDownTag);

	if (nullptr == pSkillCoolDownDesc)
		return;

	m_fMaxCoolDownTime = pSkillCoolDownDesc->fMaxCoolDownTime;
	m_fCurCoolDownTime = pSkillCoolDownDesc->fCurCoolDownTime;

	m_pUICoolDownTime->Set_Ratio(m_fCurCoolDownTime / m_fMaxCoolDownTime);
	
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

	if (nullptr != m_pFontPotionCnt && !g_bIsStageChange)
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
	if (nullptr != m_pFont && !g_bIsStageChange)
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

void CQuickSlot::SetUp_FontCoolDownTime(const _float& fTimeDelta)
{
	if (nullptr != m_pFontSkillCoolDownTime && 0.0f != m_fCurCoolDownTime && L"" != m_wstrCoolDownTag)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -12.0f;
		vPos.y += -10.0f;

		_tchar	szText[MIN_STR] = L"";
		wstring wstrText        = L"%d";
		swprintf_s(szText, L"%0.1f", m_fCurCoolDownTime);

		m_pFontPotionCnt->Set_Text(szText);
		m_pFontPotionCnt->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFontPotionCnt->Update_GameObject(fTimeDelta);
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

	// Engine::Safe_Release(m_pShaderCoolDownCom);
	Engine::Safe_Release(m_pSlotFrame);
	Engine::Safe_Release(m_pUICoolDownTime);
	Engine::Safe_Release(m_pSlotIcon);
	Engine::Safe_Release(m_pFontPotionCnt);
	Engine::Safe_Release(m_pFontSkillCoolDownTime);
}
