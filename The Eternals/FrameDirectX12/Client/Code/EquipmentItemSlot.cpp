#include "stdafx.h"
#include "EquipmentItemSlot.h"
#include "GameUIChild.h"
#include "DirectInput.h"
#include "DescriptorHeapMgr.h"
#include "InventoryItemSlot.h"


CEquipmentItemSlot::CEquipmentItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

HRESULT CEquipmentItemSlot::Ready_GameObject(wstring wstrRootObjectTag, 
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
	Engine::FAILED_CHECK_RETURN(CGameUIChild::Ready_GameObject(wstrRootObjectTag, 
															   wstrObjectTag,
															   wstrDataFilePath,
															   vPos,
															   vScale,
															   bIsSpriteAnimation,
															   fFrameSpeed,
															   vRectOffset,
															   vRectScale,
															   iUIDepth), E_FAIL);

	m_bIsActive = false;

	// NoItemInfo
	m_tNoItemInfo.vScale          = vScale;
	m_tNoItemInfo.chItemType      = NO_ITEM;
	m_tNoItemInfo.chItemName      = NO_ITEM;
	m_tNoItemInfo.uiItemIdx       = m_uiTexIdx;
	m_tNoItemInfo.tItemIconFrame  = m_tFrame;

	// ItemIcon
	m_tCurItemInfo.vScale                   = _vec3(0.08f, 0.05f, 1.0f);
	m_tCurItemInfo.chItemType               = NO_ITEM;
	m_tCurItemInfo.chItemName               = NO_ITEM;
	m_tCurItemInfo.tItemIconFrame.fFrameCnt = 1.0f;
	m_tCurItemInfo.tItemIconFrame.fCurFrame = 0.0f;
	m_tCurItemInfo.tItemIconFrame.fSceneCnt = 1.0f;
	m_tCurItemInfo.tItemIconFrame.fCurScene = 0.0f;

	m_chPreItemType = m_tCurItemInfo.chItemType;
	m_chPreItemName = m_tCurItemInfo.chItemName;

	// Slot Frame
	Engine::CGameObject* pSlotFrame = CGameUIChild::Create(m_pGraphicDevice, m_pCommandList,
														   wstrRootObjectTag, 
														   wstrObjectTag,
														   L"",
														   vPos,
														   _vec3(0.1f, 0.07f, 1.0f),
														   bIsSpriteAnimation,
														   fFrameSpeed,
														   _vec3(0.0f),
														   _vec3(0.0f),
														   iUIDepth - 1);
	m_pSlotFrame = static_cast<CGameUIChild*>(pSlotFrame);

	FRAME tFrame;
	tFrame.fFrameCnt = 1.0f;
	tFrame.fCurFrame = 0.0f;
	tFrame.fSceneCnt = 1.0f;
	tFrame.fCurScene = 0.0f;
	Engine::FAILED_CHECK_RETURN(m_pSlotFrame->SetUp_TexDescriptorHeap(L"S1UI_WareHouse", tFrame, m_uiSlotFrameIdx), E_FAIL);
	m_pSlotFrame->Set_IsActive(m_bIsActive);

	return S_OK;
}

HRESULT CEquipmentItemSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CEquipmentItemSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	
	if (!m_bIsActive)
		return NO_EVENT;

	SetUp_ItemIcon();
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CEquipmentItemSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);
	
	KeyInput_MouseButton(fTimeDelta);
	
	if (m_bIsOnMouse)
	{
		m_pSlotFrame->Set_IsActive(m_bIsActive);
		m_pSlotFrame->Update_GameObject(fTimeDelta);
		m_pSlotFrame->LateUpdate_GameObject(fTimeDelta);
	}

	return NO_EVENT;
}

void CEquipmentItemSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CEquipmentItemSlot::KeyInput_MouseButton(const _float& fTimeDelta)
{
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_RB) && 
		m_bIsKeyPressingRB)
	{
		// 아이템 해제
		if (nullptr != m_pInventoryItemSlot)
		{
			ITEM_INFO tItemInfo = m_pInventoryItemSlot->Get_CurItemInfo();
			char chItemSlotType = 0;
			if (ItemType_WeaponTwoHand == tItemInfo.chItemType ||
				ItemType_WeaponBow == tItemInfo.chItemType ||
				ItemType_WeaponRod == tItemInfo.chItemType)
				chItemSlotType = EQUIP_WEAPON;
			else if (ItemType_Helmet == tItemInfo.chItemType)
				chItemSlotType = EQUIP_HELMET;
			else if (ItemType_Armor == tItemInfo.chItemType)
				chItemSlotType = EQUIP_ARMOR;
			else if (ItemType_Shoes == tItemInfo.chItemType)
				chItemSlotType = EQUIP_SHOES;


			CPacketMgr::Get_Instance()->send_unequip_item(chItemSlotType, tItemInfo.chItemType, tItemInfo.chItemName);

			m_pInventoryItemSlot->Set_IsOnEquipment(false);
			m_pInventoryItemSlot->Set_EquipSlotTag(L"");
			m_pInventoryItemSlot = nullptr;
		}
	}

	m_bIsKeyPressingRB = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		m_bIsOnMouse = true;

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_RB))
			m_bIsKeyPressingRB = true;
		else
			m_bIsKeyPressingRB = false;
	}
	else
		m_bIsOnMouse = false;
}

void CEquipmentItemSlot::SetUp_ItemIcon()
{
	if (nullptr == m_pInventoryItemSlot)
	{
		m_tCurItemInfo.chItemType = NO_ITEM;
		m_tCurItemInfo.chItemName = NO_ITEM;
		m_chPreItemType = m_tCurItemInfo.chItemType;
		m_chPreItemName = m_tCurItemInfo.chItemName;

		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);
		m_uiTexIdx            = m_tNoItemInfo.uiItemIdx;
		m_tFrame              = m_tNoItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tNoItemInfo.vScale;

		m_bIsOnEquipment = false;
		m_uiInventoryIdx = NO_ITEM;
	}
	else 
	{
		m_bIsOnEquipment = true;
		m_uiInventoryIdx = m_pInventoryItemSlot->Get_ItemSlotIdx();
		
		if (m_pInventoryItemSlot->Get_CurItemInfo().chItemType == m_chPreItemType &&
			m_pInventoryItemSlot->Get_CurItemInfo().chItemName == m_chPreItemName)
			return;

		switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemType)
		{
		case ItemType_WeaponTwoHand:
		{
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentWeapon");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponTwohand");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentWeapon");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponBow");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentWeapon");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponRod");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentHelmet");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemHelmet");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentArmor");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemArmor");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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
			m_pInventoryItemSlot->Set_EquipSlotTag(L"EquipmentShoes");
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemShoes");
			m_tFrame              = m_tCurItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

			switch (m_pInventoryItemSlot->Get_CurItemInfo().chItemName)
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

		default:
		{
			m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);
			m_uiTexIdx            = m_tNoItemInfo.uiItemIdx;
			m_tFrame              = m_tNoItemInfo.tItemIconFrame;
			m_pTransCom->m_vScale = m_tNoItemInfo.vScale;
		}
			break;
		}

		m_chPreItemType = m_pInventoryItemSlot->Get_CurItemInfo().chItemType;
		m_chPreItemName = m_pInventoryItemSlot->Get_CurItemInfo().chItemName;
	}

}

Engine::CGameObject* CEquipmentItemSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
												wstring wstrRootObjectTag,
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
	CEquipmentItemSlot* pInstance = new CEquipmentItemSlot(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrRootObjectTag,
										   wstrObjectTag,
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

void CEquipmentItemSlot::Free()
{
	CGameUIChild::Free();

	Engine::Safe_Release(m_pSlotFrame);
	m_pInventoryItemSlot = nullptr;
}
