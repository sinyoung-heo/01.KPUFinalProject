#pragma once
#include "GameUIChild.h"

class CPartySuggestCanvas;
class CPartySystemMgr;

class CPartySuggestButton : public CGameUIChild
{
private:
	explicit CPartySuggestButton(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPartySuggestButton() = default;

public:
	void Set_CanvasClass(CPartySuggestCanvas* pCanvas) { m_pCanvas = pCanvas; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrRootObjectTag,
									 wstring wstrObjectTag,							   
									 wstring wstrDataFilePath,
									 const _vec3& vPos,
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth = 1000);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	void SetUp_FontPosition(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Manager ]
	____________________________________________________________________________________________________________*/
	CPartySystemMgr* m_pPartySystemMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsKeyPressing = false;
	CPartySuggestCanvas* m_pCanvas = nullptr;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrRootObjectTag,
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

