#pragma once
#include "GameUIChild.h"

class CChattingCursor : public CGameUIChild
{
private:
	explicit CChattingCursor(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CChattingCursor() = default;

public:
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
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool	m_bIsRenderOn	    = false;

	_float	m_fTime             = 0.0f;
	_float	m_fUpdateTime       = 0.25f;
	_float	m_fRenderTime       = 0.0f;
	_float	m_fUpdateRenderTime = 0.25f;

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

