#pragma once
#include "Base.h"
#include "GraphicDevice.h"

namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
	class CRenderer;
}

class CToolMainApp : public Engine::CBase
{
private:
	explicit CToolMainApp();
	virtual ~CToolMainApp() = default;

public:
	HRESULT Ready_MainApp();
	_int	Update_MainApp(const _float& fTimeDelta);
	_int	LateUpdate_MainApp(const _float& fTimeDelta);
	void	Render_MainApp(const _float& fTimeDelta);
private:
	HRESULT	SetUp_DefaultSetting(Engine::WINMODE eMode, const _uint& dwWidth, const _uint& dwHeight);
	HRESULT	SetUp_Font();
	HRESULT	SetUp_ComponentPrototype();
	HRESULT	SetUp_LoadingResource();
	HRESULT	SetUp_StartScene(Engine::SCENEID eScebeID);

	void	Key_Input();

private:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice / Mgr ]
	____________________________________________________________________________________________________________*/
	Engine::CGraphicDevice*		m_pDeviceClass		= nullptr;
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;

	Engine::CComponentMgr*		m_pComponentMgr		= nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		= nullptr;
	Engine::CManagement*		m_pManagement		= nullptr;
	Engine::CRenderer*			m_pRenderer			= nullptr;

public:
	static CToolMainApp* Create();
private:
	virtual void Free();
};

