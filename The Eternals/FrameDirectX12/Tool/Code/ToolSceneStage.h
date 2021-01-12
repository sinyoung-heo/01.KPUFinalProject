#pragma once
#include "Scene.h"

namespace Engine
{
	class CFont;
}

class CToolTerrain;

class CToolSceneStage : public Engine::CScene
{
private:
	explicit CToolSceneStage(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolSceneStage() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float & fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float & fTimeDelta);
	virtual HRESULT	Render_Scene(const _float & fTimeDelta);

private:
	HRESULT			Ready_LayerCamera(wstring wstrLayerTag);
	HRESULT			Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT			Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT			Ready_LayerUI(wstring wstrLayerTag);
	HRESULT			Ready_LayerFont(wstring wstrLayerTag);
	HRESULT			Ready_LightInfo();

	void			Key_Input();

public:
	CToolTerrain* m_pPickingTerrain = nullptr;

private:
	/*__________________________________________________________________________________________________________
	[ Loading Text ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont_Stage = nullptr;
	_tchar			m_szFPS[MAX_STR] = L"";
	_int			m_iFPS = 0;

public:
	static CToolSceneStage* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);

private:
	virtual void Free();
};

