#pragma once
#include "Include.h"
#include "Scene.h"

class CScene_StageVelika final : public Engine::CScene
{
private:
	explicit CScene_StageVelika(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CScene_StageVelika() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float& fTimeDelta);
	virtual HRESULT	Render_Scene(const _float& fTimeDelta, const Engine::RENDERID& eID = Engine::RENDERID::MULTI_THREAD);

private:
	HRESULT			Ready_LayerCamera(wstring wstrLayerTag);
	HRESULT			Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT			Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT			Ready_LayerUI(wstring wstrLayerTag);
	HRESULT			Ready_LayerFont(wstring wstrLayerTag);
	HRESULT			Ready_LightInfo();
	HRESULT			Ready_NaviMesh();

public:
	static CScene_StageVelika*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

