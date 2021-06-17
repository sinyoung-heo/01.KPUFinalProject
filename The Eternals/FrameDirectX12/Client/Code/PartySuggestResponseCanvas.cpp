#include "stdafx.h"
#include "PartySuggestResponseCanvas.h"
#include "DirectInput.h"
#include "Font.h"

CPartySuggestResponseCanvas::CPartySuggestResponseCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CPartySuggestResponseCanvas::Ready_GameObject(wstring wstrObjectTag, 
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
	m_pFont->Set_Text(L"파티가입을 수락하시겠습니까?");

	return S_OK;
}

HRESULT CPartySuggestResponseCanvas::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPartySuggestResponseCanvas::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	KeyInput_MouseMove(fTimeDelta);

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPartySuggestResponseCanvas::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CPartySuggestResponseCanvas::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CPartySuggestResponseCanvas::KeyInput_MouseMove(const _float& fTimeDelta)
{
	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
	{
		/*__________________________________________________________________________________________________________
		[ 마우스 좌, 우 이동 ]
		____________________________________________________________________________________________________________*/
		_long dwMouseMoveX = 0;
		if (dwMouseMoveX = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_X))
		{
			m_pTransCom->m_vPos.x += static_cast<_float>(dwMouseMoveX);
		}

		/*__________________________________________________________________________________________________________
		[ 마우스 상, 하 이동 ]
		___________________________________________________________________________________________________________*/
		_long dwMouseMoveY = 0;
		if (dwMouseMoveY = Engine::GetDIMouseMove(Engine::MOUSEMOVESTATE::DIMS_Y))
		{
			m_pTransCom->m_vPos.y += static_cast<_float>(dwMouseMoveY);
		}
	}
}

void CPartySuggestResponseCanvas::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x -= 110.0f;
		vPos.y += 64.0f;

		// 파티초대
		if (PARTY_REQUEST_STATE::REQUEST_PARTY_INVITE == m_ePartyRequestState)
			m_pFont->Set_Text(L"파티초대를 수락하시겠습니까?");
		// 파티가입
		else if (PARTY_REQUEST_STATE::REQUEST_PARTY_JOIN == m_ePartyRequestState)
			m_pFont->Set_Text(L"파티가입을 수락하시겠습니까?");

		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CPartySuggestResponseCanvas::Create(ID3D12Device* pGraphicDevice, 
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
	CPartySuggestResponseCanvas* pInstance = new CPartySuggestResponseCanvas(pGraphicDevice, pCommandList);

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

void CPartySuggestResponseCanvas::Free()
{
	CGameUIRoot::Free();
}
