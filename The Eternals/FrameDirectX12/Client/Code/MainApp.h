#pragma once
#include "Include.h"
#include "Base.h"
#include "GraphicDevice.h"

namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
	class CRenderer;
	class CFont;
}


class CMainApp : public Engine::CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

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

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	// FPS
	Engine::CFont*	m_pFont_FPS						= nullptr;
	_tchar			m_szFPS[MAX_STR]				= L"";
	_float			m_fTime							= 0.0f;
	_uint			m_uiFPS							= 0;

	// PipelineState
	Engine::CFont*	m_pFont_PipelineState			= nullptr;
	wstring			m_wstrPipeline					= L"";
	_tchar			m_szPipeline[MAX_STR]			= L"";
	_uint			m_uiCnt_ShaderFile				= 0;
	_uint			m_uiCnt_PipelineState			= 0;
	_uint			m_uiCnt_SetPipelineState		= 0;

	// RenderList
	Engine::CFont*	m_pFont_RenderList				= nullptr;
	wstring			m_wstrRenderList				= L"";
	_tchar			m_szRenderList[MAX_STR]			= L"";
	_uint			m_iPrioritySize					= 0;
	_uint			m_iNoneAlphaSize				= 0;
	_uint			m_iAlphaSize					= 0;
	_uint			m_iUISize						= 0;
	_uint			m_iColliderSize					= 0;
	_uint			m_iFontSize						= 0;

public:
	static CMainApp*	Create();
private:
	virtual void		Free();
};
