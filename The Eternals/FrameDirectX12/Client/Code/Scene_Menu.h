#pragma once
#include "Include.h"
#include "Scene.h"

class CScene_Menu final : public Engine::CScene
{
private:
	explicit CScene_Menu(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CScene_Menu() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float& fTimeDelta);
	virtual HRESULT	Render_Scene(const _float& fTimeDelta, const Engine::RENDERID& eID = Engine::RENDERID::MULTI_THREAD);

private:
	HRESULT			Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT			Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT			Ready_LayerUI(wstring wstrLayerTag);

public:
	static CScene_Menu*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

