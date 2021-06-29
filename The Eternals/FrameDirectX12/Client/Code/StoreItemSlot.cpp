#include "stdafx.h"
#include "StoreItemSlot.h"
#include "DescriptorHeapMgr.h"
#include "DirectInput.h"
#include "Font.h"
#include "StoreMgr.h"

CStoreItemSlot::CStoreItemSlot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CGameUIChild(pGraphicDevice, pCommandList)
{
}

void CStoreItemSlot::Set_CurItemInfo(const char& chItemType, const char& chItemName)
{
	m_tCurItemInfo.chItemType = chItemType;
	m_tCurItemInfo.chItemName = chItemName;
}

HRESULT CStoreItemSlot::Ready_GameObject(wstring wstrRootObjectTag,
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
	m_pSlotFrame->Set_TextureIdx(1);

	m_pFont->Set_Color(D2D1::ColorF::Cornsilk);
	m_pFont->Set_Text(L"Grade D TwoHand  %d G");

	return S_OK;
}

HRESULT CStoreItemSlot::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CGameUIChild::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CStoreItemSlot::Update_GameObject(const _float& fTimeDelta)
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

_int CStoreItemSlot::LateUpdate_GameObject(const _float& fTimeDelta)
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

	SetUp_FontPosition(fTimeDelta);

	return NO_EVENT;
}

void CStoreItemSlot::SetUp_FontPosition(const _float& fTimeDelta)
{
	if (nullptr != m_pFont)
	{
		//if (Engine::KEY_PRESSING(DIK_Z))
		//{
		//	++fOffsetX;
		//	cout << fOffsetX << ", " << fOffsetY << endl;
		//}
		//if (Engine::KEY_PRESSING(DIK_X))
		//{
		//	--fOffsetX;
		//	cout << fOffsetX << ", " << fOffsetY << endl;
		//}
		//if (Engine::KEY_PRESSING(DIK_C))
		//{
		//	++fOffsetY;
		//	cout << fOffsetX << ", " << fOffsetY << endl;
		//}
		//if (Engine::KEY_PRESSING(DIK_V))
		//{
		//	--fOffsetY;
		//	cout << fOffsetX << ", " << fOffsetY << endl;
		//}

		_vec3 vPos = _vec3(m_pTransColor->m_matWorld._41, m_pTransColor->m_matWorld._42, m_pTransColor->m_matWorld._43).Convert_DescartesTo2DWindow(WINCX, WINCY);
		vPos.x += 36.0f;
		vPos.y += -10.0f;

		_tchar	szText[MIN_STR] = L"";
		wsprintf(szText, m_wstrFontText.c_str(), m_uiPrice);

		m_pFont->Set_Text(szText);
		m_pFont->Set_Pos(_vec2(vPos.x, vPos.y));
		m_pFont->Update_GameObject(fTimeDelta);
	}
}

void CStoreItemSlot::Render_GameObject(const _float& fTimeDelta)
{
	CGameUIChild::Render_GameObject(fTimeDelta);
}

void CStoreItemSlot::KeyInput_MouseButton(const _float& fTimeDelta)
{
	if (CMouseCursorMgr::Get_Instance()->Check_CursorInRect(m_tRect) &&
		Engine::MOUSE_KEYUP(Engine::MOUSEBUTTON::DIM_LB) &&
		m_bIsKeyPressingLB)
	{
		CStoreMgr::Get_Instance()->Push_StoreItemBuySlot(m_tCurItemInfo.chItemType, m_tCurItemInfo.chItemName, 1, m_uiPrice);
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

void CStoreItemSlot::SetUp_ItemIcon()
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
		m_uiPrice             = 0;
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
			m_uiPrice = Item_Twohand19_A_SM_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D TwoHand   %d G";
			break;
		case TwoHand27_SM:
			m_uiPrice = Item_TwoHand27_SM_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C TwoHand   %d G";
			break;
		case TwoHand29_SM:
			m_uiPrice = Item_TwoHand29_SM_COST;
			m_uiTexIdx = 2;
			m_wstrFontText = L"Grade B TwoHand   %d G";
			break;
		case TwoHand31_SM:
			m_uiPrice = Item_TwoHand31_SM_COST;
			m_uiTexIdx = 3;
			m_wstrFontText = L"Grade A TwoHand   %d G";
			break;
		case TwoHand33_B_SM:
			m_uiPrice = Item_TwoHand33_B_SM_COST;
			m_uiTexIdx = 4;
			m_wstrFontText = L"Grade S TwoHand   %d G";
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
			m_uiPrice = Item_Bow18_A_SM_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D Bow  %d G";
			break;
		case Bow27_SM:
			m_uiPrice = Item_Bow27_SM_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C Bow  %d G";
			break;
		case Bow23_SM:
			m_uiPrice = Item_Bow23_SM_COST;
			m_uiTexIdx = 2;
			m_wstrFontText =L"Grade B Bow  %d G";
			break;
		case Bow31_SM:
			m_uiPrice = Item_Bow31_SM_COST;
			m_uiTexIdx = 3;
			m_wstrFontText =L"Grade A Bow  %d G";
			break;
		case Event_Season_Bow_01_SM:
			m_uiPrice = Item_Event_Season_Bow_01_SM_COST;
			m_uiTexIdx = 4;
			m_wstrFontText =L"Grade S Bow  %d G";
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
			m_uiPrice = Item_Event_Wit_Rod_01_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D Rod  %d G";
			break;
		case Rod19_A:
			m_uiPrice = Item_Rod19_A_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C Rod  %d G";
			break;
		case Rod28_B:
			m_uiPrice = Item_Rod28_B_COST;
			m_uiTexIdx = 2;
			m_wstrFontText = L"Grade B Rod  %d G";
			break;
		case Rod31:
			m_uiPrice = Item_Rod31_COST;
			m_uiTexIdx = 3;
			m_wstrFontText = L"Grade A Rod  %d G";
			break;
		case Rod33_B:
			m_uiPrice = Item_Rod33_B_COST;
			m_uiTexIdx = 4;
			m_wstrFontText = L"Grade S Rod  %d G";
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
			m_uiPrice = Item_Helmet_D_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D Helmet  %d G";
			break;
		case Helmet_C:
			m_uiPrice = Item_Helmet_C_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C Helmet  %d G";
			break;
		case Helmet_B:
			m_uiPrice = Item_Helmet_B_COST;
			m_uiTexIdx = 2;
			m_wstrFontText = L"Grade B Helmet  %d G";
			break;
		case Helmet_A:
			m_uiPrice = Item_Helmet_A_COST;
			m_uiTexIdx = 3;
			m_wstrFontText = L"Grade A Helmet  %d G";
			break;
		case Helmet_S:
			m_uiPrice = Item_Helmet_S_COST;
			m_uiTexIdx = 4;
			m_wstrFontText = L"Grade S Helmet  %d G";
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
			m_uiPrice = Item_Armor_D_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D Armor  %d G";
			break;
		case Armor_C:
			m_uiPrice = Item_Armor_C_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C Armor  %d G";
			break;
		case Armor_B:
			m_uiPrice = Item_Armor_B_COST;
			m_uiTexIdx = 2;
			m_wstrFontText = L"Grade B Armor  %d G";
			break;
		case Armor_A:
			m_uiPrice = Item_Armor_A_COST;
			m_uiTexIdx = 3;
			m_wstrFontText = L"Grade A Armor  %d G";
			break;
		case Armor_S:
			m_uiPrice = Item_Armor_S_COST;
			m_uiTexIdx = 4;
			m_wstrFontText = L"Grade S Armor  %d G";
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
			m_uiPrice = Item_Shoes_D_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"Grade D Shoes  %d G";
			break;
		case Shoes_C:
			m_uiPrice = Item_Shoes_C_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"Grade C Shoes  %d G";
			break;
		case Shoes_B:
			m_uiPrice = Item_Shoes_B_COST;
			m_uiTexIdx = 2;
			m_wstrFontText = L"Grade B Shoes  %d G";
			break;
		case Shoes_A:
			m_uiPrice = Item_Shoes_A_COST;
			m_uiTexIdx = 3;
			m_wstrFontText = L"Grade A Shoes  %d G";
			break;
		case Shoes_S:
			m_uiPrice = Item_Shoes_S_COST;
			m_uiTexIdx = 4;
			m_wstrFontText = L"Grade S Shoes  %d G";
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
			m_uiPrice = Item_Prtion_HP_COST;
			m_uiTexIdx = 0;
			m_wstrFontText = L"HP Potion  %d G";
			break;
		case Potion_MP:
			m_uiPrice = Item_Prtion_MP_COST;
			m_uiTexIdx = 1;
			m_wstrFontText = L"MP Potion  %d G";
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

Engine::CGameObject* CStoreItemSlot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
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
	CStoreItemSlot* pInstance = new CStoreItemSlot(pGraphicDevice, pCommandList);

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

void CStoreItemSlot::Free()
{
	CGameUIChild::Free();
	Engine::Safe_Release(m_pSlotFrame);
}
