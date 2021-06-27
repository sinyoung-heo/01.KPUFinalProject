#pragma once
#include "GameUIChild.h"

class CStoreItemSlot : public CGameUIChild
{
private:
	explicit CStoreItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CStoreItemSlot() = default;

public:
	const ITEM_INFO& Get_CurItemInfo() { return m_tCurItemInfo; }
	void Set_CurItemInfo(const char& chItemType, const char& chItemName);

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
	void KeyInput_MouseButton(const _float& fTimeDelta);
	void SetUp_ItemIcon();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CGameUIChild*	m_pSlotFrame       = nullptr;	// 마우스 이벤트 - 붉은색 테두리.
	_uint			m_uiSlotFrameIdx   = 2;
	_uint			m_bIsOnMouse       = false;
	_bool			m_bIsKeyPressingLB = false;

	ITEM_INFO	m_tNoItemInfo;
	ITEM_INFO	m_tCurItemInfo;
	char		m_chPreItemType = NO_ITEM;
	char		m_chPreItemName = NO_ITEM;
	_uint		m_uiPrice = 0;
	wstring		m_wstrFontText = L"";
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

