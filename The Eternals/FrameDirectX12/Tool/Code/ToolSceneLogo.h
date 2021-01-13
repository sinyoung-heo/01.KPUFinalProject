#pragma once
#include "Scene.h"
#include "ToolLoading.h"

namespace Engine
{
	class CFont;
}

class CToolSceneLogo : public Engine::CScene
{
private:
	explicit CToolSceneLogo(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolSceneLogo() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float& fTimeDelta);
	virtual HRESULT	Render_Scene(const _float& fTimeDelta, const Engine::RENDERID& eID = Engine::RENDERID::MULTI_THREAD);

private:
	HRESULT			Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT			Ready_LayerCamera(wstring wstrLayerTag);
	HRESULT			Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT			Ready_LayerUI(wstring wstrLayerTag);
	HRESULT			Ready_LayerFont(wstring wstrLayerTag);
	HRESULT			SetUp_MaxLoadingCount();

private:
	CToolLoading*	m_pLoading	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Loading Text ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont_LoadingStr		= nullptr;
	_tchar			m_szLoadingStr[MIN_STR]	= L"";
	_int			m_iMaxFileCount			= 0;

	Engine::CFont*	m_pFont_Stage			= nullptr;
	_tchar			m_szFPS[MAX_STR]		= L"";
	_int			m_iFPS					= 0;

public:
	static CToolSceneLogo* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

private:
	virtual void Free();
};

