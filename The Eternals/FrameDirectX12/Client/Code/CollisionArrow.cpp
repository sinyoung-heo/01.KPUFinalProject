#include "stdafx.h"
#include "CollisionArrow.h"
#include "GraphicDevice.h"
#include "InstancePoolMgr.h"
#include "EffectTrail.h"
#include "ObjectMgr.h"
#include "Glowring.h"
#include "SnowParticle.h"
#include "ObjectMgr.h"
#include <random>
#include "IceStorm_s.h"
#include "TextureEffect.h"
#include "DistTrail.h"
#include "DmgFont.h"
#include "DynamicCamera.h"

CCollisionArrow::CCollisionArrow(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CCollisionTick(pGraphicDevice, pCommandList)
	, m_pShaderMeshInstancing(Engine::CShaderMeshInstancing::Get_Instance())
{
}

HRESULT CCollisionArrow::Ready_GameObject(wstring wstrMeshTag, 
										  const _vec3& vScale, 
										  const _vec3& vAngle,
										  const _vec3& vPos, 
										  const ARROW_TYPE& eType, 
										  const _uint& uiDamage, 
										  const _float& fLifeTime)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos	  = vPos;
	m_uiDamage            = uiDamage;
	m_fLifeTime           = fLifeTime;
	m_wstrCollisionTag    = L"CollisionTick_ThisPlayer";
	m_wstrMeshTag         = wstrMeshTag;
	m_eType               = eType;

	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld), 
											  m_pTransCom->m_vScale,
											  _vec3(33.0f),
											  _vec3(0.0f, 0.0f, -3.3f));

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
											m_pTransCom->m_vScale ,
											m_pMeshCom->Get_CenterPos(),
											m_pMeshCom->Get_MinVector(),
											m_pMeshCom->Get_MaxVector(),
											5.0f,
											_vec3(0.0f, 0.f, 0.0f));
	m_lstCollider.push_back(m_pBoundingSphereCom);

	m_iMeshPipelineStatePass = 4;

	if (m_wstrMeshTag == L"ArrowIce")
		m_ePoolType = ARROW_POOL_TYPE::ARROW_POOL_ICE;
	else if (m_wstrMeshTag == L"ArrowFire")
		m_ePoolType = ARROW_POOL_TYPE::ARROW_POOL_FIRE;
	else if (m_wstrMeshTag == L"ArrowLightning")
		m_ePoolType = ARROW_POOL_TYPE::ARROW_POOL_LIGHTNING;

	return S_OK;
}

HRESULT CCollisionArrow::LateInit_GameObject()
{
	// DynamicCamera
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);

	return S_OK;
}

_int CCollisionArrow::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		if (ARROW_TYPE::ARROW_CHARGE == m_eType && m_wstrCollisionTag == L"ChargeArrow")
		{
			CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
			if (nullptr != pCollisionTick)
			{
				pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
				pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
				pCollisionTick->Set_Damage(m_uiDamage);
				pCollisionTick->Set_LifeTime(0.3f);
				pCollisionTick->Get_Transform()->m_vScale = _vec3(25.0f);
				pCollisionTick->Get_Transform()->m_vPos   = m_pTransCom->m_vPos;
				pCollisionTick->Get_Transform()->m_vPos.y = 1.5f;
				pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);

				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);

				m_bisFireworkEffect = false;
			}

			if (nullptr != m_pDynamicCamera)
			{
				m_pDynamicCamera->Set_CameraAtParentMatrix(nullptr);
				m_pDynamicCamera->Set_At(m_pTransCom->m_vPos);
			}
		}

		if (ARROW_TYPE::ARROW_FALL == m_eType || ARROW_TYPE::ARROW_CHARGE == m_eType)
		{
			m_bIsReadyArrowFall = false;
			m_bIsCreateCollisionTick = false;
			m_bIsStartDissolve       = false;
			m_fDissolve              = -0.05f;
		}

		m_wstrCollisionTag = L"NoneCollisionArrow";
		m_pTransCom->m_vScale = _vec3(_vec3(0.05f));
		m_pTransCom->m_vAngle = _vec3(0.0f);
		m_pTransCom->m_vPos   = _vec3(AWAY_FROM_STAGE);
		Return_Instance(m_pInstancePoolMgr->Get_CollisionArrowPool(m_ePoolType), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (ARROW_TYPE::ARROW_DEFAULT == m_eType)
	{
		m_fTimeDelta += fTimeDelta;

		if (m_fTimeDelta >= m_fLifeTime)
		{
			m_fTimeDelta = 0.0f;
			m_bIsReturn = true;
		}

		_float fDist = m_vOriginPos.Get_Distance(m_pTransCom->m_vPos);

		if (fDist >= ARROW_MAX_DISTANCE)
			m_bIsReturn = true;

		m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_fSpeed * fTimeDelta;
	}
	else if (ARROW_TYPE::ARROW_FALL == m_eType)
	{
		if (!m_bIsReadyArrowFall)
		{
			m_bIsReadyArrowFall = true;

			random_device					rd;
			default_random_engine			dre{ rd() };
			uniform_int_distribution<_int>	uid_angle{ -10, 10 };

			m_pTransCom->m_vAngle.x += (_float)uid_angle(dre);
			m_pTransCom->m_vAngle.z += (_float)uid_angle(dre);
		}

		if (m_pTransCom->m_vPos.y > 1.0f)
		{
			m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
			m_pTransCom->m_vDir.Normalize();
			m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_fSpeed * fTimeDelta;
		}

		if (m_pTransCom->m_vPos.y < 1.0f)
		{
			m_pTransCom->m_vPos.y = 1.0f;

			m_bIsStartDissolve = true;

			if (!m_bIsCreateCollisionTick)
			{
				//Eff
				CEffectMgr::Get_Instance()->Effect_IceStorm_s(m_pTransCom->m_vPos, 0.2f);
				CGameObject* pGameObj = nullptr;
		
				_vec3 newPos = m_pTransCom->m_vPos;
				newPos.y = 0.5f;

				CEffectMgr::Get_Instance()->Effect_TextureEffect(
					L"Bomb00",						// TextureTag
					_vec3(2.f, 2.0f, 1.0f),		// Scale
					_vec3(0.0f, 0.0f, 0.0f),		// Angle
					newPos,	// Pos
					FRAME(10, 9, 30.0f), false, false,1.f,0, _vec4(0.f, 0.3f, 0.6f, 1.f));

				m_bIsCreateCollisionTick = true;

				if (m_wstrCollisionTag != L"NoneCollisionArrow")
				{
					CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
					if (nullptr != pCollisionTick)
					{
						pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
						pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
						pCollisionTick->Set_Damage(m_uiDamage);
						pCollisionTick->Set_LifeTime(0.25f);
						pCollisionTick->Get_Transform()->m_vScale = _vec3(3.0f);
						pCollisionTick->Get_Transform()->m_vPos = m_pTransCom->m_vPos;
						pCollisionTick->Get_Transform()->m_vPos.y = 1.5f;
						pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);

						m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);
					}
				}
			}
		}

		SetUp_Dissolve(fTimeDelta);
	}
	else if (ARROW_TYPE::ARROW_CHARGE == m_eType)
	{
		m_fTimeDelta += fTimeDelta;
		m_fEffectDelta += fTimeDelta;
		if (m_fTimeDelta >= m_fLifeTime)
		{
			m_fTimeDelta = 0.0f;
			m_bIsReturn = true;
		}

		_float fDist = m_vOriginPos.Get_Distance(m_pTransCom->m_vPos);

		if (fDist >= ARROW_MAX_DISTANCE * 1.5f)
		{
			m_bIsReturn = true;
		}
		m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_fSpeed * fTimeDelta;

		if (m_fEffectDelta > 0.005f)
		{
			if (m_wstrCollisionTag == L"ChargeArrow")
			{
				CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
				if (nullptr != pCollisionTick)
				{
					pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
					pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
					pCollisionTick->Set_Damage(m_uiDamage);
					pCollisionTick->Set_LifeTime(0.3f);
					pCollisionTick->Get_Transform()->m_vScale = _vec3(12.0f);
					pCollisionTick->Get_Transform()->m_vPos = m_pTransCom->m_vPos;
					pCollisionTick->Get_Transform()->m_vPos.y = 1.5f;
					pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);

					m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);
				}
			}

			m_fEffectDelta = 0.f;
			if (fDist < 5.f)
			{
				CEffectMgr::Get_Instance()->Effect_DistTrail(m_pTransCom->m_vPos, _vec3(0.0f, m_pTransCom->m_vAngle.y, 0.f)
					, true, fDist);
			}
			else
			{
				CEffectMgr::Get_Instance()->Effect_DistTrail(m_pTransCom->m_vPos, _vec3(0.0f, m_pTransCom->m_vAngle.y, 0.f)
					, false, fDist);
			}
			if (m_bisFireworkEffect == false)
			{
				m_bisFireworkEffect = true;
				_vec3 newPos = m_vOriginPos;
				newPos.y += 1.f;
				CEffectMgr::Get_Instance()->Effect_TextureEffect(
					L"Firework0",						// TextureTag
					_vec3(8.f, 8.0f, 1.0f),		// Scale
					_vec3(0.0f, 0.0f, 0.0f),		// Angle
					newPos,	// Pos
					FRAME(5, 8, 30.0f), false, false);
			
			}
		}

		if (nullptr != m_pDynamicCamera && m_wstrCollisionTag == L"ChargeArrow")
		{
			m_pDynamicCamera->Set_CameraAtParentMatrix(nullptr);
			m_pDynamicCamera->Set_At(m_pTransCom->m_vPos);
		}
	}

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);


	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	if (ARROW_TYPE::ARROW_FALL != m_eType && m_wstrCollisionTag == L"CollisionTick_ThisPlayer")
		m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CCollisionArrow::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsReturn)
		return NO_EVENT;

	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision();

	return NO_EVENT;
}

void CCollisionArrow::Process_Collision()
{
	for (auto& pDst : m_lstCollisionDst)
	{
		// Player Attack <---> Monster
		if (L"CollisionTick_ThisPlayer" == m_wstrCollisionTag &&
			L"Monster_SingleCollider" == pDst->Get_CollisionTag())
		{
			Set_IsReturnObject(true);
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Set_bisHitted(true);
			CEffectMgr::Get_Instance()->Effect_ArrowHitted(m_pTransCom->m_vPos);

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

			// Player Attack to Monster
			m_pPacketMgr->send_attackToMonster(pDst->Get_ServerNumber(), m_uiDamage, m_chAffect);
		}
		else if (L"CollisionTick_ThisPlayer" == m_wstrCollisionTag &&
				 L"Monster_MultiCollider" == pDst->Get_CollisionTag())
		{
			Collision_MonsterMultiCollider(pDst->Get_ColliderList(), pDst->Get_ServerNumber());
		}
	}
}

void CCollisionArrow::Render_GameObject(const _float& fTimeDelta, 
										ID3D12GraphicsCommandList* pCommandList,
										const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	m_pShaderMeshInstancing->Add_Instance(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass);
	_uint iInstanceIdx = m_pShaderMeshInstancing->Get_InstanceCount(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass) - 1;
	Set_ConstantTable(iContextIdx, iInstanceIdx);
}

void CCollisionArrow::Collision_MonsterMultiCollider(list<Engine::CColliderSphere*>& lstMonsterCollider, const _uint& uiSNum)
{
	for (auto iter_begin = m_lstCollider.begin(); iter_begin != m_lstCollider.end(); ++iter_begin)
	{
		for (auto iter_dst_begin = ++lstMonsterCollider.begin(); iter_dst_begin != lstMonsterCollider.end(); ++iter_dst_begin)
		{
			if (Engine::CCollisionMgr::Check_Sphere((*iter_begin)->Get_BoundingInfo(), (*iter_dst_begin)->Get_BoundingInfo()))
			{
				// Process Collision Event
				(*iter_begin)->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				(*iter_dst_begin)->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

				Set_IsReturnObject(true);

				CEffectMgr::Get_Instance()->Effect_ArrowHitted(m_pTransCom->m_vPos, 2.5f);

				// pDst->Set_bisHitted(true);

				// DmgFont
				Engine::CGameObject* pGameObj = nullptr;
				pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_DmgFontPool());
				if (nullptr != pGameObj)
				{
					random_device					rd;
					default_random_engine			dre{ rd() };
					uniform_int_distribution<_int>	uid{ -7, 7 };

					static_cast<CDmgFont*>(pGameObj)->Get_Transform()->m_vPos = _vec3((*iter_begin)->Get_BoundingInfo().Center);
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
				m_pPacketMgr->send_attackToMonster(uiSNum, m_uiDamage, m_chAffect);

				return;
			}
		}
	}
}

HRESULT CCollisionArrow::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	return S_OK;
}

void CCollisionArrow::Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
	_matrix matView = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW));
	_matrix matProj = *(Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION));

	Engine::CB_SHADER_MESH_INSTANCEING tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH_INSTANCEING));
	tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matView        = Engine::CShader::Compute_MatrixTranspose(matView);
	tCB_ShaderMesh.matProj        = Engine::CShader::Compute_MatrixTranspose(matProj);
	tCB_ShaderMesh.matLightView   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fDissolve      = m_fDissolve;
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.1f, 1.1f, 1.1f, 1.0f);

	m_pShaderMeshInstancing->Get_UploadBuffer_ShaderMesh(iContextIdx, m_wstrMeshTag, m_iMeshPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderMesh);
}

void CCollisionArrow::SetUp_Dissolve(const _float& fTimeDelta)
{
	if (m_bIsStartDissolve)
	{
		m_fDissolve += fTimeDelta * 1.0f;

		if (m_fDissolve >= 1.0f)
		{
			m_fDissolve = 1.0f;
			m_bIsReturn = true;
		}
	}
}

Engine::CGameObject* CCollisionArrow::Create(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList,
											 wstring wstrMeshTag, 
											 const _vec3& vScale, 
											 const _vec3& vAngle,
											 const _vec3& vPos, 
											 const ARROW_TYPE& eType, 
											 const _uint& uiDamage, 
											 const _float& fLifeTime)
{
	CCollisionArrow* pInstance = new CCollisionArrow(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, eType, uiDamage, fLifeTime)))
		Engine::Safe_Release(pInstance);

	// SetUp InstanceShader and Add Instance Count.
	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	Engine::CShaderMeshInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);

	return pInstance;
}

CCollisionArrow** CCollisionArrow::Create_InstancePool(ID3D12Device* pGraphicDevice,
													   ID3D12GraphicsCommandList* pCommandList,
													   wstring wstrMeshTag,
													   const _uint& uiInstanceCnt)
{
	CCollisionArrow** ppInstance = new (CCollisionArrow * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CCollisionArrow(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(wstrMeshTag, 
										_vec3(0.05f),
										_vec3(0.0f),
										_vec3(AWAY_FROM_STAGE),
										ARROW_TYPE::ARROW_TYPE_END,
										0,
										0.0f);

		Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
		Engine::CShaderMeshInstancing::Get_Instance()->Add_TotalInstanceCount(wstrMeshTag);
	}

	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_ConstantBuffer(pGraphicDevice, wstrMeshTag, uiInstanceCnt);

	return ppInstance;
}

void CCollisionArrow::Free()
{
	CCollisionTick::Free();

	Engine::Safe_Release(m_pMeshCom);
	m_pDynamicCamera = nullptr;
}
