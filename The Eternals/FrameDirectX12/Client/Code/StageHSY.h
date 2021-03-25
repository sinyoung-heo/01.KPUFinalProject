#pragma once
#include "Include.h"
#include "Scene.h"

class CStageHSY : public Engine::CScene
{
private:
	explicit CStageHSY(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CStageHSY() = default;

public:
	virtual HRESULT Ready_Scene();
	void			Process_PacketFromServer();
	virtual _int	Update_Scene(const _float& fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float& fTimeDelta);
	virtual void	Send_PacketToServer();
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
	_bool m_bIsReadyMouseCursorMgr = false;

public:
	static CStageHSY* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

