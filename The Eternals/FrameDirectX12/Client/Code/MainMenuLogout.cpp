#include "stdafx.h"
#include "MainMenuLogout.h"
#include "DirectInput.h"
#include "MainMenuLogoutCanvas.h"

CMainMenuLogout::CMainMenuLogout(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
{
}

HRESULT CMainMenuLogout::Ready_GameObject(wstring wstrObjectTag, 
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

	m_mapMainMenuState[L"Normal"] = UI_ROOT_STATE();
	m_mapMainMenuState[L"Normal"].tFrame         = m_tFrame;
	m_mapMainMenuState[L"Normal"].vPos           = m_pTransCom->m_vPos;
	m_mapMainMenuState[L"Normal"].vScale         = m_pTransCom->m_vScale;
	m_mapMainMenuState[L"Normal"].vRectPosOffset = vRectOffset;
	m_mapMainMenuState[L"Normal"].vRectScale     = vRectScale;

	Engine::FAILED_CHECK_RETURN(SetUp_MainMenuState(), E_FAIL);

	return S_OK;
}

HRESULT CMainMenuLogout::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CMainMenuLogout::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (g_bIsCinemaStart)
		return NO_EVENT;

	if (m_bIsDead)
		return DEAD_OBJ;
	if (g_bIsOpenShop)
		m_bIsActiveCanvas = false;

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CMainMenuLogout::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (g_bIsCinemaStart)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressing && !g_bIsOpenShop)
	{
		m_bIsActiveCanvas = !m_bIsActiveCanvas;
	}

	m_bIsKeyPressing = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_tFrame                = m_mapMainMenuState[L"Blue"].tFrame;
			m_pTransCom->m_vPos     = m_mapMainMenuState[L"Blue"].vPos;
			m_pTransCom->m_vScale   = m_mapMainMenuState[L"Blue"].vScale;
			m_vRectOffset           = m_mapMainMenuState[L"Blue"].vRectPosOffset;
			m_pTransColor->m_vScale = m_mapMainMenuState[L"Blue"].vRectScale;
			m_bIsKeyPressing = true;
		}
		else
		{
			m_tFrame                = m_mapMainMenuState[L"Red"].tFrame;
			m_pTransCom->m_vPos     = m_mapMainMenuState[L"Red"].vPos;
			m_pTransCom->m_vScale   = m_mapMainMenuState[L"Red"].vScale;
			m_vRectOffset           = m_mapMainMenuState[L"Red"].vRectPosOffset;
			m_pTransColor->m_vScale = m_mapMainMenuState[L"Red"].vRectScale;
			m_bIsKeyPressing = false;
		}
	}
	else
	{
		m_tFrame                = m_mapMainMenuState[L"Normal"].tFrame;
		m_pTransCom->m_vPos     = m_mapMainMenuState[L"Normal"].vPos;
		m_pTransCom->m_vScale   = m_mapMainMenuState[L"Normal"].vScale;
		m_vRectOffset           = m_mapMainMenuState[L"Normal"].vRectPosOffset;
		m_pTransColor->m_vScale = m_mapMainMenuState[L"Normal"].vRectScale;
	}

	// Active Canvas UI
	if (nullptr != m_pLogoutCanvas)
	{
		m_pLogoutCanvas->Set_IsActive(m_bIsActiveCanvas);
		m_pLogoutCanvas->SetUp_ActiveChildUI(m_bIsActiveCanvas);
	}

	return NO_EVENT;
}

void CMainMenuLogout::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

HRESULT CMainMenuLogout::SetUp_MainMenuState()
{
	// IMG RED 
	m_mapMainMenuState[L"Red"] = UI_ROOT_STATE();

	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuLogout_Red.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			m_mapMainMenuState[L"Red"].vPos           = vPos;
			m_mapMainMenuState[L"Red"].vScale         = vScale;
			m_mapMainMenuState[L"Red"].vRectPosOffset = vRectPosOffset;
			m_mapMainMenuState[L"Red"].vRectScale     = vRectScale;

			wifstream fin2{ wstrDataFilePath };
			if (fin2.fail())
				return E_FAIL;

			wstring	wstrTextureTag = L"";
			_int	iTextureIndex  = 0;
			_float	fFrameCnt      = 0.0f;
			_float	fCurFrame      = 0.0f;
			_float	fSceneCnt      = 0.0f;
			_float	fCurScene      = 0.0f;
			_float	fCanvasWidth   = 0.0f;
			_float	fCanvasHeight  = 0.0f;
			_float	fGridWidth     = 0.0f;
			_float	fGridHeight    = 0.0f;

			while (true)
			{
				fin2 >> wstrTextureTag 	// TextureTag
					>> iTextureIndex	// TextureIndex
					>> fFrameCnt		// FrameCnt
					>> fCurFrame		// CurFrame
					>> fSceneCnt		// SceneCnt
					>> fCurScene		// CurScene
					>> fCanvasWidth		// CanvasWidth
					>> fCanvasHeight	// CanvasHeight
					>> fGridWidth		// GridWidth
					>> fGridHeight;		// GridHeight

				if (fin2.eof())
					break;

				m_mapMainMenuState[L"Red"].tFrame.fFrameCnt = fFrameCnt;
				m_mapMainMenuState[L"Red"].tFrame.fCurFrame = fCurFrame;
				m_mapMainMenuState[L"Red"].tFrame.fSceneCnt = fSceneCnt;
				m_mapMainMenuState[L"Red"].tFrame.fCurScene = fCurScene;
			}
		}
	}

	// IMG BLUE
	m_mapMainMenuState[L"Blue"] = UI_ROOT_STATE();
	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuLogout_Blue.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			m_mapMainMenuState[L"Blue"].vPos           = vPos;
			m_mapMainMenuState[L"Blue"].vScale         = vScale;
			m_mapMainMenuState[L"Blue"].vRectPosOffset = vRectPosOffset;
			m_mapMainMenuState[L"Blue"].vRectScale     = vRectScale;

			wifstream fin2{ wstrDataFilePath };
			if (fin2.fail())
				return E_FAIL;

			wstring	wstrTextureTag = L"";
			_int	iTextureIndex  = 0;
			_float	fFrameCnt      = 0.0f;
			_float	fCurFrame      = 0.0f;
			_float	fSceneCnt      = 0.0f;
			_float	fCurScene      = 0.0f;
			_float	fCanvasWidth   = 0.0f;
			_float	fCanvasHeight  = 0.0f;
			_float	fGridWidth     = 0.0f;
			_float	fGridHeight    = 0.0f;

			while (true)
			{
				fin2 >> wstrTextureTag 	// TextureTag
					>> iTextureIndex	// TextureIndex
					>> fFrameCnt		// FrameCnt
					>> fCurFrame		// CurFrame
					>> fSceneCnt		// SceneCnt
					>> fCurScene		// CurScene
					>> fCanvasWidth		// CanvasWidth
					>> fCanvasHeight	// CanvasHeight
					>> fGridWidth		// GridWidth
					>> fGridHeight;		// GridHeight

				if (fin2.eof())
					break;

				m_mapMainMenuState[L"Blue"].tFrame.fFrameCnt = fFrameCnt;
				m_mapMainMenuState[L"Blue"].tFrame.fCurFrame = fCurFrame;
				m_mapMainMenuState[L"Blue"].tFrame.fSceneCnt = fSceneCnt;
				m_mapMainMenuState[L"Blue"].tFrame.fCurScene = fCurScene;
			}
		}
	}

	return S_OK;
}

Engine::CGameObject* CMainMenuLogout::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
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
	CMainMenuLogout* pInstance = new CMainMenuLogout(pGraphicDevice, pCommandList);

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

void CMainMenuLogout::Free()
{
	CGameUIRoot::Free();

	m_mapMainMenuState.clear();

}