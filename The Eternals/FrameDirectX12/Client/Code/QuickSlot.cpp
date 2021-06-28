#include "stdafx.h"
#include "QuickSlot.h"
#include "Font.h"
#include "DirectInput.h"


CQuickSlot::CQuickSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
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
															   true, L"Font_BinggraeMelona5"), E_FAIL);

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
	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
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

	if (m_chCurSlotName == Prtion_HP || m_chCurSlotName == Prtion_MP)
		SetUp_FontPotionCnt(fTimeDelta);

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
		
	}

	// 슬롯 정보 스왑 해제
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_RB) && 
		m_bIsKeyPressingRB)
	{
		
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

	switch (m_chCurSlotName)
	{

	default:
		break;
	}

	m_chPreSlotName = m_chCurSlotName;
}

void CQuickSlot::SetUp_FontPotionCnt(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -19.0f;
		vPos.y += -2.0f;

		_tchar	szText[MIN_STR] = L"";
		wstring wstrText        = L"%d";
		wsprintf(szText, wstrText.c_str(), m_uiCnt);

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
}
