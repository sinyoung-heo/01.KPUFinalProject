#include "stdafx.h"
#include "InventoryItemSlot.h"
#include "GameUIChild.h"
#include "DirectInput.h"
#include "DescriptorHeapMgr.h"
#include "InventoryEquipmentMgr.h"
#include "Font.h"
#include "StoreMgr.h"
#include "QuickSlotMgr.h"

CInventoryItemSlot::CInventoryItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
	, m_pInvenEquipMgr(CInventoryEquipmentMgr::Get_Instance())
{
}

void CInventoryItemSlot::Set_CurItemInfo(const char& chItemType, const char& chItemName, const _int& iCnt)
{
	m_tCurItemInfo.chItemType = chItemType;
	m_tCurItemInfo.chItemName = chItemName;
	m_uiCnt += iCnt;
}

HRESULT CInventoryItemSlot::Ready_GameObject(wstring wstrRootObjectTag,
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
															   iUIDepth,
															   true, L"Font_BinggraeMelona16"), E_FAIL);

	m_bIsActive = false;
	
	// NoItemInfo
	m_tNoItemInfo.vScale          = vScale;
	m_tNoItemInfo.chItemType      = NO_ITEM;
	m_tNoItemInfo.chItemName      = NO_ITEM;
	m_tNoItemInfo.uiItemIdx       = m_uiTexIdx;
	m_tNoItemInfo.tItemIconFrame  = m_tFrame;

	// ItemIcon
	m_tCurItemInfo.vScale                   = _vec3(0.09f, 0.06f, 1.0f);
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

	// PotionCnt Font
	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CInventoryItemSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CInventoryItemSlot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	
	if (m_bIsDead)
		return DEAD_OBJ;
	if (!m_bIsActive)
		return NO_EVENT;

	if (!m_bIsOnEquipment)
		m_wstrEquipSlotTag = L"";

	SetUp_ItemIcon();
	CGameUIChild::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CInventoryItemSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;

	CGameUIChild::LateUpdate_GameObject(fTimeDelta);

	KeyInput_MouseButton(fTimeDelta);
	
	if (m_bIsOnMouse || m_bIsOnEquipment)
	{
		m_pSlotFrame->Set_IsActive(m_bIsActive);
		m_pSlotFrame->Update_GameObject(fTimeDelta);
		m_pSlotFrame->LateUpdate_GameObject(fTimeDelta);
	}

	if (m_tCurItemInfo.chItemType == ItemType_Potion)
		SetUp_FontPotionCnt(fTimeDelta);

	return NO_EVENT;
}

void CInventoryItemSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CInventoryItemSlot::KeyInput_MouseButton(const _float& fTimeDelta)
{
	// 아이템 스왑
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) && 
		m_bIsKeyPressingLB)
	{
		if (!g_bIsOpenShop)
		{
			// Item 정보 교환.
			if (m_pInvenEquipMgr->Get_IsInventoryItemSwapState())
			{
				vector<CInventoryItemSlot*> vecInvenSlot = m_pInvenEquipMgr->Get_InventorySlotList();
				_uint uiSelectIdx = m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Get_ItemSlotIdx();
				if (uiSelectIdx == m_uiIdx)
				{
					m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
					m_pInvenEquipMgr->Set_IsInventoryItemSwapState(false);
					m_bIsKeyPressingLB = false;
					return;
				}

				map<wstring, CEquipmentItemSlot*> mapEquipmentSlot = m_pInvenEquipMgr->Get_EquipmentSlotMap();

				CInventoryItemSlot* pSelectItemSlot  = vecInvenSlot[uiSelectIdx];
				ITEM_INFO			tSelectItemInfo  = pSelectItemSlot->Get_CurItemInfo();
				_uint				uiSelectItemCnt  = pSelectItemSlot->Get_CurItemCnt();
				_bool				bIsOnEquipment   = pSelectItemSlot->Get_IsOnEquipment();
				wstring				wstrEquipmentTag = pSelectItemSlot->Get_EquipmentTag();

				// SelectItemSlot 장비 장착 O && this ItemSlot 장비 장착 O
				if (pSelectItemSlot->Get_EquipmentTag() != L"" && m_wstrEquipSlotTag != L"")
				{
					mapEquipmentSlot[pSelectItemSlot->Get_EquipmentTag()]->Set_InventorySlotClass(this);
					mapEquipmentSlot[m_wstrEquipSlotTag]->Set_InventorySlotClass(pSelectItemSlot);
				}
				// SelectItemSlot 장비 장착 O && this ItemSlot 장비 장착 X
				else if (pSelectItemSlot->Get_EquipmentTag() != L"" && m_wstrEquipSlotTag == L"")
				{
					mapEquipmentSlot[pSelectItemSlot->Get_EquipmentTag()]->Set_InventorySlotClass(this);
				}
				// SelectItemSlot 장비 장착 X && this ItemSlot 장비 장착 O
				else if (pSelectItemSlot->Get_EquipmentTag() == L"" && m_wstrEquipSlotTag != L"")
				{
					mapEquipmentSlot[m_wstrEquipSlotTag]->Set_InventorySlotClass(pSelectItemSlot);
				}

				// SelectSlot에 현재 슬롯의 정보를 저장.
				pSelectItemSlot->Set_CurItemInfo(m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);
				pSelectItemSlot->Set_CurItemCnt(m_uiCnt);
				pSelectItemSlot->Set_IsOnEquipment(m_bIsOnEquipment);
				pSelectItemSlot->Set_EquipSlotTag(m_wstrEquipSlotTag);

				// 현재 슬롯의 정보에 SelectSlot의 정보를 저장.
				m_tCurItemInfo.chItemType = tSelectItemInfo.chItemType;
				m_tCurItemInfo.chItemName = tSelectItemInfo.chItemName;
				m_uiCnt                   = uiSelectItemCnt;
				m_bIsOnEquipment          = bIsOnEquipment;
				m_wstrEquipSlotTag        = wstrEquipmentTag;

				m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
				m_pInvenEquipMgr->Set_IsInventoryItemSwapState(false);
				m_bIsKeyPressingLB = false;

				// SelectSlot을 Potion ptr로 저장.
				if (ItemType_Potion == m_tCurItemInfo.chItemType)
				{
					if (Potion_HP == m_tCurItemInfo.chItemName)
						m_pInvenEquipMgr->Set_HpPotionSlot(this);
					else
						m_pInvenEquipMgr->Set_MpPotionSlot(this);
				}

				// this를 Potion ptr로 저장.
				if (ItemType_Potion == pSelectItemSlot->Get_CurItemInfo().chItemType)
				{
					if (Potion_HP == pSelectItemSlot->Get_CurItemInfo().chItemName)
						m_pInvenEquipMgr->Set_HpPotionSlot(pSelectItemSlot);
					else
						m_pInvenEquipMgr->Set_MpPotionSlot(pSelectItemSlot);
				}
				return;
			}

			if (!m_pInvenEquipMgr->Get_IsInventoryItemSwapState() &&
				!CQuickSlotMgr::Get_Instance()->Get_IsQuickSlotSwapState())
			{
				if (NO_ITEM != m_tCurItemInfo.chItemType)
					m_pInvenEquipMgr->Set_IsInventoryItemSwapState(true);
				else
					m_pInvenEquipMgr->Set_IsInventoryItemSwapState(false);

				m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_ItemSlotIdx(m_uiIdx);
				m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_CurItemInfo(m_tCurItemInfo.chItemType,
																				m_tCurItemInfo.chItemName,
																				m_uiCnt);

				m_bIsKeyPressingLB = false;
				return;
			}
		}

		// 상점 Open --> 판매 리스트 슬롯에 아이템 등록.
		else
		{
			// 만약 Swap 상태라면 해제.
			if (m_pInvenEquipMgr->Get_IsInventoryItemSwapState())
			{
				m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
				m_pInvenEquipMgr->Set_IsInventoryItemSwapState(false);
				m_bIsKeyPressingLB = false;
			}

			if (NO_ITEM == m_tCurItemInfo.chItemType &&
				NO_ITEM == m_tCurItemInfo.chItemName)
				return;

			if (m_bIsOnEquipment)
				return;

			if (ItemType_Potion == m_tCurItemInfo.chItemType)
			{
				--m_uiCnt;

				CStoreMgr::Get_Instance()->Push_StoreItemSellSlot(m_tCurItemInfo.chItemType,
																  m_tCurItemInfo.chItemName,
																  m_uiIdx,
																  m_uiPrice);

				if (m_uiCnt <= 0)
				{
					m_tCurItemInfo.chItemType = NO_ITEM;
					m_tCurItemInfo.chItemName = NO_ITEM;
					return;
				}
			}
			else
			{
				CStoreMgr::Get_Instance()->Push_StoreItemSellSlot(m_tCurItemInfo.chItemType,
																  m_tCurItemInfo.chItemName,
																  m_uiIdx,
																  m_uiPrice);

				m_tCurItemInfo.chItemType = NO_ITEM;
				m_tCurItemInfo.chItemName = NO_ITEM;
				m_uiCnt = 0;
			}
		}
	}

	// 아이템 스왑 해제 or 아이템 장착
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_RB) && 
		m_bIsKeyPressingRB)
	{
		if (!g_bIsOpenShop)
		{
			if (m_pInvenEquipMgr->Get_IsInventoryItemSwapState())
			{
				m_pInvenEquipMgr->Get_InventorySwapSlotClass()->Set_CurItemInfo(NO_ITEM, NO_ITEM, 0);
				m_pInvenEquipMgr->Set_IsInventoryItemSwapState(false);
				m_bIsKeyPressingRB = false;
				return;
			}

			if (ItemType_Potion == m_tCurItemInfo.chItemType ||
				NO_ITEM == m_tCurItemInfo.chItemType)
			{
				return;
			}

			// 현재 ItemSlot이 장비 착용 상태 [X] => ITEM 장착.
			wstring wsrEquipSlotTag = L"";
			map<wstring, CEquipmentItemSlot*> mapEquipmentSlot = m_pInvenEquipMgr->Get_EquipmentSlotMap();
			Engine::CGameObject* pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");

			if (!m_bIsOnEquipment)
			{
				if (ItemType_WeaponTwoHand == m_tCurItemInfo.chItemType ||
					ItemType_WeaponBow == m_tCurItemInfo.chItemType ||
					ItemType_WeaponRod == m_tCurItemInfo.chItemType)
				{
					if (ItemType_WeaponTwoHand == m_tCurItemInfo.chItemType && PC_GLADIATOR != m_pInvenEquipMgr->Get_ThisPlayerJob())
						return;
					else if (ItemType_WeaponBow == m_tCurItemInfo.chItemType && PC_ARCHER != m_pInvenEquipMgr->Get_ThisPlayerJob())
						return;
					else if (ItemType_WeaponRod == m_tCurItemInfo.chItemType && PC_PRIEST != m_pInvenEquipMgr->Get_ThisPlayerJob())
						return;

					wsrEquipSlotTag = L"EquipmentWeapon";

					// 현재 장비창에 장비 착용 [X] :: 아이템 장착.
					if (nullptr == mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot())
					{
						mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(this);
						m_bIsOnEquipment = true;
					}
					// 현재 장비창에 장비 착용 [O] :: 장착중인 아이템슬롯 초기화 -> 아이템 장착.
					else
					{
						mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot()->Set_IsOnEquipment(false);
						mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(this);
						m_bIsOnEquipment = true;
					}

					pThisPlayer->Set_WeaponType(m_tCurItemInfo.chItemName);
					CPacketMgr::Get_Instance()->send_equip_item(EQUIP_WEAPON, m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);

					return;
				}

				// 헬멧 장착
				else if (ItemType_Helmet == m_tCurItemInfo.chItemType)
				{
					wsrEquipSlotTag = L"EquipmentHelmet";
					CPacketMgr::Get_Instance()->send_equip_item(EQUIP_HELMET, m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);
				}
				// 갑옷 장착
				else if (ItemType_Armor == m_tCurItemInfo.chItemType)
				{
					wsrEquipSlotTag = L"EquipmentArmor";
					CPacketMgr::Get_Instance()->send_equip_item(EQUIP_ARMOR, m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);
				}
				// 신발 장착
				else if (ItemType_Shoes == m_tCurItemInfo.chItemType)
				{
					wsrEquipSlotTag = L"EquipmentShoes";
					CPacketMgr::Get_Instance()->send_equip_item(EQUIP_SHOES, m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);
				}

				// 현재 장비창에 장비 착용 [X] :: 아이템 장착.
				if (nullptr == mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot())
				{
					mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(this);
					m_bIsOnEquipment = true;
				}
				// 현재 장비창에 장비 착용 [O] :: 장착중인 아이템슬롯 초기화 -> 아이템 장착.
				else
				{
					mapEquipmentSlot[wsrEquipSlotTag]->Get_InventoryItemSlot()->Set_IsOnEquipment(false);
					mapEquipmentSlot[wsrEquipSlotTag]->Set_InventorySlotClass(this);
					m_bIsOnEquipment = true;
				}

				return;
			}
		}
	}

	m_bIsKeyPressingLB = false;
	m_bIsKeyPressingRB = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		m_bIsOnMouse = true;
		m_pSlotFrame->Set_TextureIdx(2);

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
		{
			m_bIsKeyPressingLB = true;
		}
		else if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_RB))
		{
			m_bIsKeyPressingRB = true;
		}
	}
	else
	{
		m_bIsOnMouse = false;

		if (m_bIsOnEquipment)
			m_pSlotFrame->Set_TextureIdx(1);
		else
			m_pSlotFrame->Set_TextureIdx(2);
	}
}

void CInventoryItemSlot::SetUp_ItemIcon()
{
	if (m_tCurItemInfo.chItemType == m_chPreItemType &&
		m_tCurItemInfo.chItemName == m_chPreItemName)
		return;

	switch (m_tCurItemInfo.chItemType)
	{
	case NO_ITEM:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);
		m_uiTexIdx            = m_tNoItemInfo.uiItemIdx;
		m_tFrame              = m_tNoItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tNoItemInfo.vScale;
		m_chJob               = -1;
		m_uiPrice             = 0;
	}
		break;

	case ItemType_WeaponTwoHand:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponTwohand");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_GLADIATOR;

		switch (m_tCurItemInfo.chItemName)
		{
		case Twohand19_A_SM:
			m_uiPrice = Item_Twohand19_A_SM_COST;
			m_uiTexIdx = 0;
			break;
		case TwoHand27_SM:
			m_uiPrice = Item_TwoHand27_SM_COST;
			m_uiTexIdx = 1;
			break;
		case TwoHand29_SM:
			m_uiPrice = Item_TwoHand29_SM_COST;
			m_uiTexIdx = 2;
			break;
		case TwoHand31_SM:
			m_uiPrice = Item_TwoHand31_SM_COST;
			m_uiTexIdx = 3;
			break;
		case TwoHand33_B_SM:
			m_uiPrice = Item_TwoHand33_B_SM_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_WeaponBow:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponBow");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_ARCHER;

		switch (m_tCurItemInfo.chItemName)
		{
		case Bow18_A_SM:
			m_uiPrice = Item_Bow18_A_SM_COST;
			m_uiTexIdx = 0;
			break;
		case Bow27_SM:
			m_uiPrice = Item_Bow27_SM_COST;
			m_uiTexIdx = 1;
			break;
		case Bow23_SM:
			m_uiPrice = Item_Bow23_SM_COST;
			m_uiTexIdx = 2;
			break;
		case Bow31_SM:
			m_uiPrice = Item_Bow31_SM_COST;
			m_uiTexIdx = 3;
			break;
		case Event_Season_Bow_01_SM:
			m_uiPrice = Item_Event_Season_Bow_01_SM_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_WeaponRod:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponRod");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_PRIEST;

		switch (m_tCurItemInfo.chItemName)
		{
		case Event_Wit_Rod_01:
			m_uiPrice = Item_Event_Wit_Rod_01_COST;
			m_uiTexIdx = 0;
			break;
		case Rod19_A:
			m_uiPrice = Item_Rod19_A_COST;
			m_uiTexIdx = 1;
			break;
		case Rod28_B:
			m_uiPrice = Item_Rod28_B_COST;
			m_uiTexIdx = 2;
			break;
		case Rod31:
			m_uiPrice = Item_Rod31_COST;
			m_uiTexIdx = 3;
			break;
		case Rod33_B:
			m_uiPrice = Item_Rod33_B_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Helmet:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemHelmet");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_ALL;

		switch (m_tCurItemInfo.chItemName)
		{
		case Helmet_D:
			m_uiPrice = Item_Helmet_D_COST;
			m_uiTexIdx = 0;
			break;
		case Helmet_C:
			m_uiPrice = Item_Helmet_C_COST;
			m_uiTexIdx = 1;
			break;
		case Helmet_B:
			m_uiPrice = Item_Helmet_B_COST;
			m_uiTexIdx = 2;
			break;
		case Helmet_A:
			m_uiPrice = Item_Helmet_A_COST;
			m_uiTexIdx = 3;
			break;
		case Helmet_S:
			m_uiPrice = Item_Helmet_S_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Armor:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemArmor");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_ALL;

		switch (m_tCurItemInfo.chItemName)
		{
		case Armor_D:
			m_uiPrice = Item_Armor_D_COST;
			m_uiTexIdx = 0;
			break;
		case Armor_C:
			m_uiPrice = Item_Armor_C_COST;
			m_uiTexIdx = 1;
			break;
		case Armor_B:
			m_uiPrice = Item_Armor_B_COST;
			m_uiTexIdx = 2;
			break;
		case Armor_A:
			m_uiPrice = Item_Armor_A_COST;
			m_uiTexIdx = 3;
			break;
		case Armor_S:
			m_uiPrice = Item_Armor_S_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Shoes:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemShoes");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = PC_ALL;

		switch (m_tCurItemInfo.chItemName)
		{
		case Shoes_D:
			m_uiPrice = Item_Shoes_D_COST;
			m_uiTexIdx = 0;
			break;
		case Shoes_C:
			m_uiPrice = Item_Shoes_C_COST;
			m_uiTexIdx = 1;
			break;
		case Shoes_B:
			m_uiPrice = Item_Shoes_B_COST;
			m_uiTexIdx = 2;
			break;
		case Shoes_A:
			m_uiPrice = Item_Shoes_A_COST;
			m_uiTexIdx = 3;
			break;
		case Shoes_S:
			m_uiPrice = Item_Shoes_S_COST;
			m_uiTexIdx = 4;
			break;
		}
	}
		break;

	case ItemType_Potion:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemPotion");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;
		m_chJob = -1;

		switch (m_tCurItemInfo.chItemName)
		{
		case Potion_HP:
			m_uiPrice = Item_Prtion_HP_COST;
			m_uiTexIdx = 0;
			break;
		case Potion_MP:
			m_uiPrice = Item_Prtion_MP_COST;
			m_uiTexIdx = 1;
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
		m_chJob               = -1;
		m_uiPrice             = 0;
	}
		break;
	}

	m_chPreItemType = m_tCurItemInfo.chItemType;
	m_chPreItemName = m_tCurItemInfo.chItemName;
}

void CInventoryItemSlot::SetUp_FontPotionCnt(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += -19.0f;
		vPos.y += -2.0f;

		_tchar	szText[MIN_STR] = L"";
		wstring wstrText        = L"%d";
		wsprintf(szText, wstrText.c_str(), m_uiCnt);

		m_pFont->Set_Text(szText);
		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CInventoryItemSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CInventoryItemSlot* pInstance = new CInventoryItemSlot(pGraphicDevice, pCommandList);

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

void CInventoryItemSlot::Free()
{
	CGameUIChild::Free();

	Engine::Safe_Release(m_pSlotFrame);
}
