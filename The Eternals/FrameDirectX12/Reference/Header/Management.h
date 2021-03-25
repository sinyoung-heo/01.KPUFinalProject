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
	CScene* Get_CurrentScene() { return m_pCurrentScene; }
public:
	HRESULT	SetUp_CurrentScene(CScene* pNewScene);
	void	Process_PacketFromServer();
	_int	Update_Management(const _float& fTimeDelta);
	_int	LateUpdate_Management(const _float& fTimeDelta);
	void	Send_PacketToServer();
	HRESULT	Render_Management(const _float& fTimeDelta, const RENDERID& eID = RENDERID::MULTI_THREAD);
protected:
	CScene* m_pCurrentScene = nullptr;

private:
	virtual void Free();
};

END