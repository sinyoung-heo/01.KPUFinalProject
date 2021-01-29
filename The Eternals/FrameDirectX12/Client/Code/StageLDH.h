#pragma once
#include "Include.h"
#include "Scene.h"

class CStageLDH : public Engine::CScene
{
private:
	explicit CStageLDH(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CStageLDH() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float& fTimeDelta);
	virtual HRESULT	Render_Scene(const _float& fTimeDelta, const Engine::RENDERID& eID = Engine::RENDERID::MULTI_THREAD);

private:
	HRESULT	Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT	Ready_LayerCamera(wstring wstrLayerTag);
	HRESULT	Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT	Ready_LayerUI(wstring wstrLayerTag);
	HRESULT	Ready_LayerFont(wstring wstrLayerTag);
	HRESULT	Ready_LightInfo();
	HRESULT	Ready_NaviMesh();

private:
	HRESULT Ready_Server();
	HRESULT Connect_Server();

public:
	static CStageLDH*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void		Free();
};

