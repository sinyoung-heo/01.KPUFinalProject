#include "stdafx.h"
#include "CollisionTick.h"
#include "InstancePoolMgr.h"
#include "WarningFrame.h"
#include "DmgFont.h"
#include "DynamicCamera.h"
#include "PCGladiator.h"
#include "PCPriest.h"
#include <random>

CCollisionTick::CCollisionTick(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
	, m_pPacketMgr(CPacketMgr::Get_Instance())
{
}

HRESULT CCollisionTick::Ready_GameObject(wstring wstrCollisionTag, 
										 const _vec3& vScale, 
										 const _vec3& vPos,
										 const _uint& uiDamage,
										 const _float& fLifeTime)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, false, true), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = _vec3(0.0f);
	m_pTransCom->m_vPos	  = vPos;
	m_uiDamage            = uiDamage;
	m_fLifeTime           = fLifeTime;
	m_wstrCollisionTag    = wstrCollisionTag;

	// BoundingSphere
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld), m_pTransCom->m_vScale, _vec3(1.0f), _vec3(0.0f));
	m_lstCollider.push_back(m_pBoundingSphereCom);

	return S_OK;
}

HRESULT CCollisionTick::LateInit_GameObject()
{
	// DynamicCamera
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);

	// ThisPlayer
	m_pThisPlayer = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer");
	Engine::NULL_CHECK_RETURN(m_pThisPlayer, E_FAIL);

	return S_OK;
}

_int CCollisionTick::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= m_fLifeTime)
	{
		m_fTimeDelta = 0.0f;
		m_bIsReturn  = true;
	}

	if (m_bIsReturn)
	{
		Return_Instance(m_pInstancePoolMgr->Get_CollisionTickPool(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CCollisionTick::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsReturn)
		return NO_EVENT;

	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision();

	return NO_EVENT;
}

void CCollisionTick::Process_Collision()
{
	m_bIsCameraEffect = false;

	for (auto& pDst : m_lstCollisionDst)
	{
		// Player Attack <---> Monster
		if (L"CollisionTick_ThisPlayer" == m_wstrCollisionTag &&
			L"Monster_SingleCollider" == pDst->Get_CollisionTag())
		{
			Set_IsReturnObject(true);
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

			CEffectMgr::Get_Instance()->Effect_ArrowHitted(m_pTransCom->m_vPos);

			pDst->Set_bisHitted(true);

			// DmgFont
			Engine::CGameObject* pGameObj = nullptr;
			pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_DmgFontPool());
			if (nullptr != pGameObj)
			{
				random_device					rd;
				default_random_engine			dre{ rd() };
				uniform_int_distribution<_int>	uid{ -7, 7 };

				// m_pTransCom->m_vDir.x = (_float)(uid(dre)) * 0.1f;

				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos = pDst->Get_Transform()->m_vPos;
				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.x += (_float)(uid(dre)) * 0.1f;
				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.y += 3.0f;
				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.z += (_float)(uid(dre)) * 0.1f;
				static_cast<CDmgFont*>(pGameObj)->Set_DamageList(m_uiDamage);
				static_cast<CDmgFont*>(pGameObj)->Set_DamageType(DMG_TYPE::DMG_PLAYER);
				static_cast<CDmgFont*>(pGameObj)->Set_RandomDir();
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"DmgFont", pGameObj);
			}

			// Camera Effect
			m_bIsCameraEffect = true;

			// Player Attack to Monster
			m_pPacketMgr->send_attackToMonster(pDst->Get_ServerNumber(), m_uiDamage, m_chAffect);
		}
		else if (L"CollisionTick_ThisPlayer" == m_wstrCollisionTag &&
				 L"Monster_MultiCollider" == pDst->Get_CollisionTag())
		{

		}

		// Monster Attack <---> ThisPlayer
		else if (L"CollisionTick_Monster" == m_wstrCollisionTag &&
				 L"ThisPlayer" == pDst->Get_CollisionTag())
		{
			Set_IsReturnObject(true);
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

			// On WarningFrame
			Engine::CGameObject* pWarningFrame = m_pObjectMgr->Get_GameObject(L"Layer_UI", L"WarningFrame");
			static_cast<CWarningFrame*>(pWarningFrame)->Set_IsRender(true);

			// DmgFont
			Engine::CGameObject* pGameObj = nullptr;
			pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_DmgFontPool());
			if (nullptr != pGameObj)
			{
				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos = pDst->Get_Transform()->m_vPos;
				static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos.y = pDst->Get_Transform()->m_vPos.y + 0.5f;
				static_cast<CDmgFont*>(pGameObj)->Set_DamageList(m_uiDamage);
				static_cast<CDmgFont*>(pGameObj)->Set_DamageType(DMG_TYPE::DMG_MONSTER);
				static_cast<CDmgFont*>(pGameObj)->Set_RandomDir();
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"DmgFont", pGameObj);
			}

			// Monster Attack to ThisPlayer
			m_pPacketMgr->send_attackByMonster(m_iSNum, m_uiDamage);
		}
	}

	SetUp_GladiatorCameraEvent();
	SetUp_PriestCameraEvent();
}

void CCollisionTick::SetUp_GladiatorCameraEvent()
{
	if (nullptr == m_pDynamicCamera || nullptr == m_pThisPlayer)
		return;
	if (PC_GLADIATOR != m_pThisPlayer->Get_OType())
		return;

	_uint uiAniIdx = static_cast<CPCGladiator*>(m_pThisPlayer)->Get_AnimationIdx();

	switch (uiAniIdx)
	{
	case Gladiator::COMBO1:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower = 0.04f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;
	case Gladiator::COMBO2:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower     = 0.03f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 6.0f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;
	case Gladiator::COMBO3:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower     = 0.04f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;
	case Gladiator::COMBO4:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower     = 0.04f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;

	case Gladiator::STINGER_BLADE:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower     = 0.045f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 12.5f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;

	case Gladiator::CUTTING_SLASH:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.3f;
			tCameraShakingDesc.vMin               = _vec2(-25.0f, 0.0f);
			tCameraShakingDesc.vMax               = _vec2(25.0f, 0.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 20.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Gladiator::JAW_BREAKER:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.2f;
			tCameraShakingDesc.vMin               = _vec2(-30.0f, 0.0f);
			tCameraShakingDesc.vMax               = _vec2(30.0f, 0.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 20.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Gladiator::GAIA_CRUSH2:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_OUT;
			tCameraZoomDesc.fPower     = 0.025f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 15.f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.1f;
			tCameraShakingDesc.vMin               = _vec2(-12.0f, 0.0f);
			tCameraShakingDesc.vMax               = _vec2(12.0f, 0.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 10.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
	break;

	case Gladiator::DRAW_SWORD:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_OUT;
			tCameraZoomDesc.fPower     = 0.05f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 15.f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;

	default:
		break;
	}
}

void CCollisionTick::SetUp_PriestCameraEvent()
{
	if (nullptr == m_pDynamicCamera || nullptr == m_pThisPlayer)
		return;
	if (PC_PRIEST != m_pThisPlayer->Get_OType())
		return;

	_uint uiAniIdx = static_cast<CPCPriest*>(m_pThisPlayer)->Get_AnimationIdx();

	switch (uiAniIdx)
	{
	case Priest::HASTE:
	{
		if (m_bIsCameraEffect)
		{
			CAMERA_ZOOM_DESC tCameraZoomDesc;
			tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
			tCameraZoomDesc.fPower = 0.04f;
			tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
		}
	}
		break;
	}
}

Engine::CGameObject* CCollisionTick::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
											wstring wstrCollisionTag, 
											const _vec3& vScale, 
											const _vec3& vPos, 
											const _uint& uiDamage, 
											const _float& fLifeTime)
{
	CCollisionTick* pInstance = new CCollisionTick(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrCollisionTag, vScale, vPos, uiDamage, fLifeTime)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CCollisionTick** CCollisionTick::Create_InstancePool(ID3D12Device* pGraphicDevice,
													 ID3D12GraphicsCommandList* pCommandList, 
													 const _uint& uiInstanceCnt)
{
	CCollisionTick** ppInstance = new (CCollisionTick*[uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CCollisionTick(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"", _vec3(1.0f), _vec3(0.0f), 0, 0.0f);
	}

	return ppInstance;
}

void CCollisionTick::Free()
{
	Engine::CGameObject::Free();
}
