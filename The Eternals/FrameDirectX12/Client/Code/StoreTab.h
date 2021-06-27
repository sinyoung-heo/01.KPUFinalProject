#pragma once
#include "GameUIChild.h"

class CStoreTab : public CGameUIChild
{
private:
	explicit CStoreTab(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CStoreTab() = default;

public:
	const char& Get_ItemType() { return m_chItemType; }
	void Set_IsSelected(const _bool& bIsSelected)		{ m_bIsSelected = bIsSelected; }
	void Set_FontTextPosOffset(const _vec2& vOffset)	{ m_vFontOffset = vOffset; }
	void Set_ItemType(const char& chItemType)			{ m_chItemType = chItemType; }
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
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsKeyPressing = false;
	_bool m_bIsSelected    = false;
	_vec2 m_vFontOffset    = _vec2(0.0f);

	char m_chItemType = NO_ITEM;

	_float m_fFontOffsetX = 0.0f;
	_float m_fFontOffsetY = 0.0f;
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

