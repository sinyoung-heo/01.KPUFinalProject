#pragma once
#include "GameUIChild.h"

const char NO_ITEM = -1;

typedef struct tagItemInfo
{
	char	chItemType      = NO_ITEM;
	char	chItemName      = NO_ITEM;
	
	_vec3	vScale          = _vec3(0.0f);
	_uint	uiItemIdx       = 0;
	FRAME	tItemIconFrame;

} ITEM_INFO;

class CInventoryItemSlot : public CGameUIChild
{
private:
	explicit CInventoryItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CInventoryItemSlot() = default;

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
	void KeyInput_MouseButton(const _float& fTimeDelta);
	void SetUp_ItemIcon();
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsKeyPressingLB = false;
	_bool m_bIsKeyPressingRB = false;

	_bool			m_bIsOnEquipment = false;	// 아이템 장착	- 노란색 테두리.
	CGameUIChild*	m_pSlotFrame     = nullptr;	// 마우스 이벤트 - 붉은색 테두리.
	_uint			m_uiSlotFrameIdx = 2;
	_uint			m_bIsOnMouse     = false;

	ITEM_INFO	m_tNoItemInfo;
	ITEM_INFO	m_tCurItemInfo;
	char		m_chPreItemType = NO_ITEM;
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

