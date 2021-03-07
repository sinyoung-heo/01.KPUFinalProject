#pragma once
#include "Include.h"
#include "Base.h"

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
	static POINT	Get_CursorPoint();
	const _bool&	Get_IsActiveMouse() { return m_bIsActiveMouse; }
	void			Reset_MouseCursor() { m_pMouseCursor = nullptr; }
	void			Set_IsActiveMouse(const _bool& bIsActive) { m_bIsActiveMouse = bIsActive; }
public:
	HRESULT			Ready_MouseCursorMgr();
	void			Update_MouseCursorMgr(const _float& fTimeDelta);

private:
	Engine::CGameObject*	m_pMouseCursor   = nullptr;
	_bool					m_bIsActiveMouse = true;

private:
	virtual void Free();
};

