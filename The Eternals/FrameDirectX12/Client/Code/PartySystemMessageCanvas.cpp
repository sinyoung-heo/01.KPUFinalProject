#include "stdafx.h"
#include "PartySystemMessageCanvas.h"
#include "DirectInput.h"
#include "Font.h"

CPartySystemMessageCanvas::CPartySystemMessageCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

void CPartySystemMessageCanvas::Set_PartyMessageState(const PARTY_SYSTEM_MESSAGE& eState)
{
	if (nullptr == m_pFont)
		return;

	if (PARTY_SYSTEM_MESSAGE::ENTER_PARTY_MEMBER == eState)
	{
		m_pFont->Set_Text(L"새로운 파티원이 입장하였습니다.");
	}
	else if (PARTY_SYSTEM_MESSAGE::LEAVE_PARTY_MEMBER == eState)
	{
		m_pFont->Set_Text(L"파티원이 파티를 탈퇴하였습니다.");
	}
}

void CPartySystemMessageCanvas::Set_PartyMemberInfo(wstring wstrName,
													const char& chOtype, 
													const _int& iServerNum)
{
	m_wstrName   = wstrName;
	m_iServerNum = iServerNum;

	if (PC_GLADIATOR == chOtype)
		m_wstrJob = L"GLADIATOR";
	else if (PC_ARCHER == chOtype)
		m_wstrJob = L"ARCHER";
	else
		m_wstrJob = L"PRIEST";
}

HRESULT CPartySystemMessageCanvas::Ready_GameObject(wstring wstrObjectTag,
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
															  iUIDepth,
															  true, L"Font_BinggraeMelona24"), E_FAIL);

	m_bIsActive = true;
	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"새로운 파티원이 입장하였습니다.");

	return S_OK;
}

HRESULT CPartySystemMessageCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartySystemMessageCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime >= m_fUpdateLifeTime)
	{
		m_fLifeTime = 0.0f;
		m_bIsActive = false;

		return NO_EVENT;
	}
	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartySystemMessageCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	KeyInput_Mouse(fTimeDelta);
	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CPartySystemMessageCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CPartySystemMessageCanvas::KeyInput_Mouse(const _float& fTimeDelta)
{
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) &&
		m_bIsKeyPressing)
	{
		m_fLifeTime = 0.0f;
		m_bIsActive = false;
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
			m_bIsKeyPressing = true;
		else
			m_bIsKeyPressing = false;
	}
}

void CPartySystemMessageCanvas::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -160.0f;
		vPos.y += -40.0f;

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CPartySystemMessageCanvas::Create(ID3D12Device* pGraphicDevice, 
													   ID3D12GraphicsCommandList* pCommandList,
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
	CPartySystemMessageCanvas* pInstance = new CPartySystemMessageCanvas(pGraphicDevice, pCommandList);

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

void CPartySystemMessageCanvas::Free()
{
	CGameUIRoot::Free();
}
