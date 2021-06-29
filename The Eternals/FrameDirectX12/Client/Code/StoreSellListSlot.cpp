#include "stdafx.h"
#include "StoreSellListSlot.h"
#include "Font.h"
#include "DirectInput.h"
#include "DescriptorHeapMgr.h"
#include "StoreMgr.h"
#include "InventoryEquipmentMgr.h"

CStoreSellListSlot::CStoreSellListSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

void CStoreSellListSlot::Set_CurItemInfo(const char& chItemType, const char& chItemName, const _int& iCnt)
{
	m_tCurItemInfo.chItemType = chItemType;
	m_tCurItemInfo.chItemName = chItemName;
	m_uiCnt += iCnt;
}

HRESULT CStoreSellListSlot::Ready_GameObject(wstring wstrRootObjectTag,
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
	m_pSlotFrame->Set_TextureIdx(2);

	// PotionCnt Font
	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"");

	return S_OK;
}

HRESULT CStoreSellListSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CStoreSellListSlot::Update_GameObject(const _float& fTimeDelta)
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

_int CStoreSellListSlot::LateUpdate_GameObject(const _float& fTimeDelta)
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

	if (m_tCurItemInfo.chItemType == ItemType_Potion)
		SetUp_FontPotionCnt(fTimeDelta);

	return NO_EVENT;
}

void CStoreSellListSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CStoreSellListSlot::KeyInput_MouseButton(const _float& fTimeDelta)
{
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) &&
		m_bIsKeyPressingLB)
	{
		if (NO_ITEM != m_tCurItemInfo.chItemType &&
			NO_ITEM != m_tCurItemInfo.chItemName)
		{
			CInventoryEquipmentMgr::Get_Instance()->Push_ItemInventory(m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName);

			if (ItemType_Potion == m_tCurItemInfo.chItemType)
			{
				--m_uiCnt;
				if (m_uiCnt <= 0)
				{
					m_tCurItemInfo.chItemType = NO_ITEM;
					m_tCurItemInfo.chItemName = NO_ITEM;
					CStoreMgr::Get_Instance()->Min_StoreSellSlotSize();
				}
			}
			else
			{
				m_tCurItemInfo.chItemType = NO_ITEM;
				m_tCurItemInfo.chItemName = NO_ITEM;
				m_uiCnt = 0;
				CStoreMgr::Get_Instance()->Min_StoreSellSlotSize();
			}

			CStoreMgr::Get_Instance()->Add_SellItemSumGold(-m_iPrice);
		}

		return;
	}

	m_bIsKeyPressingLB = false;

	// Check Mouse Collision.
	if (CMouseCursorMgr::Get_Instance()->Get_IsActiveMouse() &&
		CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect))
	{
		m_bIsOnMouse = true;

		if (Engine::MOUSE_PRESSING(Engine::MOUSEBUTTON::DIM_LB))
			m_bIsKeyPressingLB = true;
		else
			m_bIsKeyPressingLB = false;

	}
	else
	{
		m_bIsOnMouse = false;
	}
}

void CStoreSellListSlot::SetUp_ItemIcon()
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
		m_iPrice             = 0;
	}
		break;

	case ItemType_WeaponTwoHand:
	{
		m_pTexDescriptorHeap  = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"ItemWeaponTwohand");
		m_tFrame              = m_tCurItemInfo.tItemIconFrame;
		m_pTransCom->m_vScale = m_tCurItemInfo.vScale;

		switch (m_tCurItemInfo.chItemName)
		{
		case Twohand19_A_SM:
			m_iPrice = Item_Twohand19_A_SM_COST;
			m_uiTexIdx = 0;
			break;
		case TwoHand27_SM:
			m_iPrice = Item_TwoHand27_SM_COST;
			m_uiTexIdx = 1;
			break;
		case TwoHand29_SM:
			m_iPrice = Item_TwoHand29_SM_COST;
			m_uiTexIdx = 2;
			break;
		case TwoHand31_SM:
			m_iPrice = Item_TwoHand31_SM_COST;
			m_uiTexIdx = 3;
			break;
		case TwoHand33_B_SM:
			m_iPrice = Item_TwoHand33_B_SM_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Bow18_A_SM:
			m_iPrice = Item_Bow18_A_SM_COST;
			m_uiTexIdx = 0;
			break;
		case Bow27_SM:
			m_iPrice = Item_Bow27_SM_COST;
			m_uiTexIdx = 1;
			break;
		case Bow23_SM:
			m_iPrice = Item_Bow23_SM_COST;
			m_uiTexIdx = 2;
			break;
		case Bow31_SM:
			m_iPrice = Item_Bow31_SM_COST;
			m_uiTexIdx = 3;
			break;
		case Event_Season_Bow_01_SM:
			m_iPrice = Item_Event_Season_Bow_01_SM_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Event_Wit_Rod_01:
			m_iPrice = Item_Event_Wit_Rod_01_COST;
			m_uiTexIdx = 0;
			break;
		case Rod19_A:
			m_iPrice = Item_Rod19_A_COST;
			m_uiTexIdx = 1;
			break;
		case Rod28_B:
			m_iPrice = Item_Rod28_B_COST;
			m_uiTexIdx = 2;
			break;
		case Rod31:
			m_iPrice = Item_Rod31_COST;
			m_uiTexIdx = 3;
			break;
		case Rod33_B:
			m_iPrice = Item_Rod33_B_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Helmet_D:
			m_iPrice = Item_Helmet_D_COST;
			m_uiTexIdx = 0;
			break;
		case Helmet_C:
			m_iPrice = Item_Helmet_C_COST;
			m_uiTexIdx = 1;
			break;
		case Helmet_B:
			m_iPrice = Item_Helmet_B_COST;
			m_uiTexIdx = 2;
			break;
		case Helmet_A:
			m_iPrice = Item_Helmet_A_COST;
			m_uiTexIdx = 3;
			break;
		case Helmet_S:
			m_iPrice = Item_Helmet_S_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Armor_D:
			m_iPrice = Item_Armor_D_COST;
			m_uiTexIdx = 0;
			break;
		case Armor_C:
			m_iPrice = Item_Armor_C_COST;
			m_uiTexIdx = 1;
			break;
		case Armor_B:
			m_iPrice = Item_Armor_B_COST;
			m_uiTexIdx = 2;
			break;
		case Armor_A:
			m_iPrice = Item_Armor_A_COST;
			m_uiTexIdx = 3;
			break;
		case Armor_S:
			m_iPrice = Item_Armor_S_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Shoes_D:
			m_iPrice = Item_Shoes_D_COST;
			m_uiTexIdx = 0;
			break;
		case Shoes_C:
			m_iPrice = Item_Shoes_C_COST;
			m_uiTexIdx = 1;
			break;
		case Shoes_B:
			m_iPrice = Item_Shoes_B_COST;
			m_uiTexIdx = 2;
			break;
		case Shoes_A:
			m_iPrice = Item_Shoes_A_COST;
			m_uiTexIdx = 3;
			break;
		case Shoes_S:
			m_iPrice = Item_Shoes_S_COST;
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

		switch (m_tCurItemInfo.chItemName)
		{
		case Potion_HP:
			m_iPrice = Item_Prtion_HP_COST;
			m_uiTexIdx = 0;
			break;
		case Potion_MP:
			m_iPrice = Item_Prtion_MP_COST;
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
	}
		break;
	}

	m_chPreItemType = m_tCurItemInfo.chItemType;
	m_chPreItemName = m_tCurItemInfo.chItemName;
}

void CStoreSellListSlot::SetUp_FontPotionCnt(const _float& fTimeDelta)
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

Engine::CGameObject* CStoreSellListSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CStoreSellListSlot* pInstance = new CStoreSellListSlot(pGraphicDevice, pCommandList);

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

void CStoreSellListSlot::Free()
{
	CGameUIChild::Free();
}
