#pragma once
#include "Scene.h"
#include "MainFrm.h"
#include "MyForm.h"

namespace Engine
{
	class CFont;
	class CLight;
	class CColliderSphere;

}

class CToolTerrain;
class CToolStaticMesh;
class CToolCell;

class CToolSceneStage : public Engine::CScene
{
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };

private:
	explicit CToolSceneStage(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolSceneStage() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int	Update_Scene(const _float & fTimeDelta);
	virtual _int	LateUpdate_Scene(const _float & fTimeDelta);
	virtual HRESULT	Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID);

private:
	HRESULT			Ready_LayerCamera(wstring wstrLayerTag);
	HRESULT			Ready_LayerEnvironment(wstring wstrLayerTag);
	HRESULT			Ready_LayerGameObject(wstring wstrLayerTag);
	HRESULT			Ready_LayerUI(wstring wstrLayerTag);
	HRESULT			Ready_LayerFont(wstring wstrLayerTag);
	HRESULT			Ready_LightInfo();

	void			KeyInput();

	// TabMap
	void			KeyInput_TabMapStaticMesh(CTabMap& TabMap);
	void			KeyInput_TabMapLightingInfo(CTabMap& TabMap);
	void			KeyInput_TabMapNavigationMesh(CTabMap& TabMap);
	void			KeyInput_TabMapModeChange(CTabMap& TabMap);

	// TabUI

public:
	/*__________________________________________________________________________________________________________
	[ TabMap ]
	____________________________________________________________________________________________________________*/
	CToolTerrain*			m_pPickingTerrain	= nullptr;
	Engine::CGameObject*	m_pPickingObject	= nullptr;
	Engine::CLight*			m_pPickingLight		= nullptr;
	CToolCell*				m_pPickingCell		= nullptr;

	// Picking NaviMeshCell
	_int						m_iPickingCnt	= -1;
	_vec3						m_vPickingPoint[POINT_END]		{ _vec3(0.0f), _vec3(0.0f) ,_vec3(0.0f) };
	_vec3*						m_pPickingPoint[POINT_END]		{ nullptr, nullptr , nullptr };
	_vec3*						m_pNearPoint	= nullptr;
	_matrix						m_matColliderWorld[POINT_END];
	Engine::CColliderSphere*	m_pPickingCollider[POINT_END]	{ nullptr, nullptr, nullptr };

	_vec3 m_vPrePickingPos = _vec3(0.0f);
	_vec3 m_vCurPickingPos = _vec3(1.0f);

	/*__________________________________________________________________________________________________________
	[ TabUI ]
	____________________________________________________________________________________________________________*/



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

