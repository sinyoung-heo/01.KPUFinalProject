#pragma once
#include "GameUIRoot.h"

class CMainMenuEquipment : public CGameUIRoot
{
private:
	explicit CMainMenuEquipment(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMainMenuEquipment() = default;

public:
	vector<Engine::CGameObject*>& Get_ChildUIList() { return m_vecUIChild; };
	void Set_IsActiveCanvas(const _bool& bIsActive) { m_bIsActiveCanvas = bIsActive; };

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrObjectTag,
									 wstring wstrDataFilePath,
									 const _vec3& vPos, 
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	HRESULT			SetUp_MainMenuState();

private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	map<wstring, UI_ROOT_STATE> m_mapMainMenuState;
	_bool						m_bIsActiveCanvas = false;
	_bool						m_bIsKeyPressing  = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrObjectTag,							   
									   wstring wstrDataFilePath,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _bool& bIsSpriteAnimation,
									   const _float& fFrameSpeed,
									   const _vec3& vRectOffset,
									   const _vec3& vRectScale,
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

