#include "stdafx.h"
#include "InventorySwapSlot.h"
#include "DescriptorHeapMgr.h"
#include "InventoryEquipmentMgr.h"

CInventorySwapSlot::CInventorySwapSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIRoot(pGraphicDevice, pCommandList)
	, m_pInvenEquipMgr(CInventoryEquipmentMgr::Get_Instance())
{
}

void CInventorySwapSlot::Set_CurItemInfo(const char& chItemType, const char& chItemName, const _int& iCnt)
{
	m_tCurItemInfo.chItemType = chItemType;
	m_tCurItemInfo.chItemName = chItemName;
	m_uiCnt = iCnt;
}

HRESULT CInventorySwapSlot::Ready_GameObject(wstring wstrObjectTag,
											 wstring wstrDataFilePath,
											 const _vec3& vPos,
											 const _vec3& vScale, 
											 const _bool& bIsSpriteAnimation, 
											 const _float& fFrameSpeed,
											 const _vec3& vRectOffset,
											 const _vec3& vRectScale,
											 const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::Ready_GameObject(wstrObjectTag,
															  wstrDataFilePath,
															  vPos,
															  vScale,
															  bIsSpriteAnimation,
															  fFrameSpeed,
															  vRectOffset,
															  vRectScale,
															  iUIDepth), E_FAIL);

	m_bIsActive = false;

	m_tCurItemInfo.chItemType = NO_ITEM;
	m_tCurItemInfo.chItemName = NO_ITEM;
	m_chPreItemName = m_tCurItemInfo.chItemType;

	return S_OK;
}

HRESULT CInventorySwapSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIRoot::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CInventorySwapSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	SetUp_ItemIcon();

	if (!m_bIsActive)
		return NO_EVENT;


	// 마우스 커서 포인터 위치.
	POINT pt = CMouseCursorMgr::Get_Instance()->Get_CursorPoint();
	m_pTransCom->m_vPos = _vec3(pt.x - 15.0f, pt.y - 15.0f, 1.0f);

	CGameUIRoot::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CInventorySwapSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIRoot::LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CInventorySwapSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIRoot::Render_GameObject(fTimeDelta);
}

void CInventorySwapSlot::SetUp_ItemIcon()
{
	if (m_tCurItemInfo.chItemName == m_chPreItemName)
		return;

	switch (m_tCurItemInfo.chItemType)
	{
	case NO_ITEM:
	{
		m_bIsActive = false;
	}
		break;

	case ItemType_WeaponTwoHand:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponTwohand");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Twohand19_A_SM:
			m_uiTexIdx = 0;
			break;
		case TwoHand27_SM:
			m_uiTexIdx = 1;
			break;
		case TwoHand29_SM:
			m_uiTexIdx = 2;
			break;
		case TwoHand31_SM:
			m_uiTexIdx = 3;
			break;
		case TwoHand33_B_SM:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_WeaponBow:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponBow");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Bow18_A_SM:
			m_uiTexIdx = 0;
			break;
		case Bow27_SM:
			m_uiTexIdx = 1;
			break;
		case Bow23_SM:
			m_uiTexIdx = 2;
			break;
		case Bow31_SM:
			m_uiTexIdx = 3;
			break;
		case Event_Season_Bow_01_SM:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_WeaponRod:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponRod");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Event_Wit_Rod_01:
			m_uiTexIdx = 0;
			break;
		case Rod19_A:
			m_uiTexIdx = 1;
			break;
		case Rod28_B:
			m_uiTexIdx = 2;
			break;
		case Rod31:
			m_uiTexIdx = 3;
			break;
		case Rod33_B:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Helmet:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemHelmet");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Helmet_D:
			m_uiTexIdx = 0;
			break;
		case Helmet_C:
			m_uiTexIdx = 1;
			break;
		case Helmet_B:
			m_uiTexIdx = 2;
			break;
		case Helmet_A:
			m_uiTexIdx = 3;
			break;
		case Helmet_S:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Armor:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemArmor");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Armor_D:
			m_uiTexIdx = 0;
			break;
		case Armor_C:
			m_uiTexIdx = 1;
			break;
		case Armor_B:
			m_uiTexIdx = 2;
			break;
		case Armor_A:
			m_uiTexIdx = 3;
			break;
		case Armor_S:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Shoes:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemShoes");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Shoes_D:
			m_uiTexIdx = 0;
			break;
		case Shoes_C:
			m_uiTexIdx = 1;
			break;
		case Shoes_B:
			m_uiTexIdx = 2;
			break;
		case Shoes_A:
			m_uiTexIdx = 3;
			break;
		case Shoes_S:
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Potion:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemPotion");
		m_bIsActive = true;

		switch (m_tCurItemInfo.chItemName)
		{
		case Potion_HP:
			m_uiTexIdx = 0;
			break;
		case Potion_MP:
			m_uiTexIdx = 1;
			break;
		}
	}
		break;

	default:
	{
		m_bIsActive = false;
	}
		break;
	}

	m_chPreItemName = m_tCurItemInfo.chItemType;
}

Engine::CGameObject* CInventorySwapSlot::Create(ID3D12Device* pGraphicDevice, 
												ID3D12GraphicsCommandList* pCommandList,
												wstring wstrObjectTag, 
												wstring wstrDataFilePath,
												const _vec3& vPos, 
												const _vec3& vScale,
												const _bool& bIsSpriteAnimation,
												const _float& fFrameSpeed,
												const _vec3& vRectOffset,
												const _vec3& vRectScale, 
												const _long& iUIDepth)
{
	CInventorySwapSlot* pInstance = new CInventorySwapSlot(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrObjectTag,
										   wstrDataFilePath,
										   vPos,
										   vScale,
										   bIsSpriteAnimation,
										   fFrameSpeed,
										   vRectOffset,
										   vRectScale,
										   iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CInventorySwapSlot::Free()
{
	CGameUIRoot::Free();
}
