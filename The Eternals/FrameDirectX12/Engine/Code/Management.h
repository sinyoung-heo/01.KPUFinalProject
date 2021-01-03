#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class CScene;

class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual ~CManagement() = default;

public:
	HRESULT	SetUp_CurrentScene(CScene* pNewScene);
	_int	Update_Management(const _float& fTimeDelta);
	_int	LateUpdate_Management(const _float& fTimeDelta);
	HRESULT	Render_Management(const _float& fTimeDelta);

protected:
	CScene* m_pCurrentScene = nullptr;

private:
	virtual void Free();
};

END