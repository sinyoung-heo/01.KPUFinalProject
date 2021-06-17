#pragma once
#include "Include.h"
#include "Base.h"
#include "ObjectMgr.h"

namespace Engine
{
	class CGameObject;
}

const int MOUSE_HIDE = INT_MIN;

class CMouseCursorMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CMouseCursorMgr)

private:
	explicit CMouseCursorMgr();
	virtual ~CMouseCursorMgr() = default;

public:
	Engine::CGameObject* Get_MouseCursorClass() { return m_pMouseCursor; }
	static POINT	Get_CursorPoint();
	const _bool&	Get_IsActiveMouse()							{ return m_bIsActiveMouse; }
	void			Set_IsActiveMouse(const _bool& bIsActive)	{ m_bIsActiveMouse = bIsActive; }
	void			Reset_MouseCursor()							{ m_pMouseCursor = nullptr; }

	HRESULT			Ready_MouseCursorMgr();
	void			Update_MouseCursorMgr(const _float& fTimeDelta);

	void	Is_ActiveMouse() { m_bIsActiveMouse = !m_bIsActiveMouse; }
	_bool	Check_CursorInRect(RECT& rcSrc);
	_bool	Check_PickingBoundingBox(Engine::CGameObject** ppPickingObject, Engine::OBJLIST* pOBJLIST);
	_bool	Check_PickingBoundingBox(Engine::CGameObject* ppPickingObject);
private:
	_bool	Check_IntersectRect(RECT& rcSrc, RECT& rcDst);
private:
	Engine::CGameObject*	m_pMouseCursor   = nullptr;
	_bool					m_bIsActiveMouse = true;
	_bool					m_bIsResetMouse  = false;

private:
	virtual void Free();
};

