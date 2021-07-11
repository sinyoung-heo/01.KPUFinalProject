#include "stdafx.h"
#include "QuestMessageButton.h"
#include "DirectInput.h"
#include "Font.h"
#include "QuestMgr.h"
#include "FadeInOut.h"
#include "PCGladiator.h"
#include "PCArcher.h"
#include "PCPriest.h"

CQuestMessageButton::CQuestMessageButton(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CQuestMessageButton::Ready_GameObject(wstring wstrRootObjectTag, 
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
															   true, L"Font_BinggraeMelona24"), E_FAIL);

	m_bIsActive = false;

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	if (L"SystemButtonYes" == wstrObjectTag)
		m_pFont->Set_Text(L"YES");
	else
		m_pFont->Set_Text(L"NO");

	return S_OK;
}

HRESULT CQuestMessageButton::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CQuestMessageButton::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CQuestMessageButton::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing)
	{
		if (L"SystemButtonYes" == m_wstrObjectTag)
		{
			Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
			char chOType = pThisPlayer->Get_OType();

			g_bIsStageChange = true;

			switch (chOType)
			{
			case PC_GLADIATOR:
			{
				static_cast<CPCGladiator*>(pThisPlayer)->Set_IsKeyDown(false);

				if (Gladiator::STANCE_ATTACK == static_cast<CPCGladiator*>(pThisPlayer)->Get_PlayerStance())
					static_cast<CPCGladiator*>(pThisPlayer)->Set_AnimationIdx(Gladiator::ATTACK_WAIT);
				else
					static_cast<CPCGladiator*>(pThisPlayer)->Set_AnimationIdx(Gladiator::NONE_ATTACK_IDLE);
			}
			break;
			case PC_ARCHER:
			{
				static_cast<CPCArcher*>(pThisPlayer)->Set_IsKeyDown(false);

				if (Gladiator::STANCE_ATTACK == static_cast<CPCArcher*>(pThisPlayer)->Get_PlayerStance())
					static_cast<CPCArcher*>(pThisPlayer)->Set_AnimationIdx(Archer::ATTACK_WAIT);
				else
					static_cast<CPCArcher*>(pThisPlayer)->Set_AnimationIdx(Archer::NONE_ATTACK_IDLE);
			}
			break;
			case PC_PRIEST:
			{
				static_cast<CPCPriest*>(pThisPlayer)->Set_IsKeyDown(false);

				if (Gladiator::STANCE_ATTACK == static_cast<CPCPriest*>(pThisPlayer)->Get_PlayerStance())
					static_cast<CPCPriest*>(pThisPlayer)->Set_AnimationIdx(Priest::ATTACK_WAIT);
				else
					static_cast<CPCPriest*>(pThisPlayer)->Set_AnimationIdx(Priest::NONE_ATTACK_IDLE);
			}
			break;
			}

			if (QUEST_TYPE::QUEST_SUB == CQuestMgr::Get_Instance()->Get_ClearQuestType())
			{
				Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
				if (nullptr != pGameObject)
				{
					static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
					static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_WINTER);
					m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
				}
			}
			else if (QUEST_TYPE::QUEST_MAIN == CQuestMgr::Get_Instance()->Get_ClearQuestType())
			{
				Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
				if (nullptr != pGameObject)
				{
					static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
					static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
					m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
				}
			}
		}

		CMouseCursorMgr::Get_Instance()->Set_IsActiveMouse(false);
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsActive(false);
		CQuestMgr::Get_Instance()->Get_QuestMessageCanvas()->Set_IsChildActive(false);
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_uiTexIdx       = 1;
			m_bIsKeyPressing = true;
		}
		else
		{
			m_uiTexIdx       = 0;
			m_bIsKeyPressing = false;
		}
	}
	else
	{
		m_uiTexIdx = 0;
	}

	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CQuestMessageButton::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CQuestMessageButton::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont && !g_bIsStageChange)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		if (L"SystemButtonYes" == m_wstrObjectTag)
			vPos.x -= 22.0f;
		else
			vPos.x -= 15.0f;

		vPos.y -= 15.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CQuestMessageButton::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CQuestMessageButton* pInstance = new CQuestMessageButton(pGraphicDevice, pCommandList);

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

void CQuestMessageButton::Free()
{
	CGameUIChild::Free();
}

