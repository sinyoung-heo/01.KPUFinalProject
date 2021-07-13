#include "GameObject.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "Renderer.h"
#include "ComponentMgr.h"
#include "Renderer.h"
#include "Font.h"
#include <random>

USING(Engine)

CGameObject::CGameObject()
	: m_pObjectMgr(CObjectMgr::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
	, m_pComponentMgr(CComponentMgr::Get_Instance())
	, m_pCollisonMgr(CCollisionMgr::Get_Instance())
	, m_pFontServer(nullptr)
{
}

CGameObject::CGameObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_pObjectMgr(CObjectMgr::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
	, m_pComponentMgr(CComponentMgr::Get_Instance())
	, m_pCollisonMgr(CCollisionMgr::Get_Instance())
	, m_pFontServer(nullptr)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice)
	, m_pCommandList(rhs.m_pCommandList)
	, m_pRenderer(rhs.m_pRenderer)
	, m_pObjectMgr(rhs.m_pObjectMgr)
	, m_pComponentMgr(rhs.m_pComponentMgr)
	, m_bIsDead(rhs.m_bIsDead)
	, m_bIsLateInit(rhs.m_bIsLateInit)
	, m_fViewZ(rhs.m_fViewZ)
	, m_UIDepth(rhs.m_UIDepth)
	, m_bIsAttack(rhs.m_bIsAttack)
	, m_bIsMoveStop(rhs.m_bIsMoveStop)
	, m_iMonsterStatus(0)
	, m_pFontServer(rhs.m_pFontServer)
{
}



CComponent * CGameObject::Get_Component(wstring wstrComponentTag, COMPONENTID eID)
{
	CComponent* pComponent = Find_Component(wstrComponentTag, eID);
	NULL_CHECK_RETURN(pComponent, nullptr);

	return pComponent;
}

SKILL_COOLDOWN_DESC* CGameObject::Get_ThisPlayerSkillCoolDown(wstring wstrTag)
{
	auto iter_find = m_mapSkillCoolDown.find(wstrTag);

	if (iter_find != m_mapSkillCoolDown.end())
		return &(iter_find->second);

	return nullptr;
}

void CGameObject::Set_DeadReckoning(const _vec3& vPos)
{
	m_pInfoCom->m_vArrivePos = vPos;	
}

void CGameObject::Set_Info(const int& lev, const int& hp, const int& maxHp, const int& mp, const int& maxMp, const int& exp, const int& maxExp, const int& minatt, const int& maxatt, const int& spd, const int& money)
{
	m_pInfoCom->m_iLev			= lev;
	m_pInfoCom->m_iHp			= hp;
	m_pInfoCom->m_iMaxHp		= maxHp;
	m_pInfoCom->m_iMp			= mp;
	m_pInfoCom->m_iMaxMp		= maxMp;
	m_pInfoCom->m_iExp			= exp;
	m_pInfoCom->m_iMaxExp		= maxExp;
	m_pInfoCom->m_iMinAttack	= minatt;
	m_pInfoCom->m_iMaxAttack	= maxatt;
	m_pInfoCom->m_fSpeed		= spd;
	m_pInfoCom->m_iMoney		= money;
}

void CGameObject::Set_Buff(const int& hp, const int& maxHp, const int& mp, const int& maxMp)
{
	m_pInfoCom->m_iHp		= hp;
	m_pInfoCom->m_iMaxHp	= maxHp;
	m_pInfoCom->m_iMp		= mp;
	m_pInfoCom->m_iMaxMp	= maxMp;
}

void CGameObject::Set_PotionAbility(const int& ability, const bool& bIsHP)
{
	if (bIsHP)
	{
		m_pInfoCom->m_iHp = ability;
	}
	else
	{
		m_pInfoCom->m_iMp = ability;
	}
}

void CGameObject::Set_Other_direction(_vec3& vDir)
{
	m_pTransCom->m_vAngle.y = vDir.Get_Angle(g_vLook);
}

float CGameObject::Set_Other_Angle(_vec3& vDir)
{
	return vDir.Get_Angle(g_vLook);
}

void CGameObject::Ready_AngleInterpolationValue(const _float& fEndAngle)
{
	m_tAngleInterpolationDesc.is_start_interpolation = true;
	m_tAngleInterpolationDesc.v1 = m_pTransCom->m_vAngle.y;
	m_tAngleInterpolationDesc.v2 = fEndAngle;
	m_tAngleInterpolationDesc.linear_ratio = Engine::MIN_LINEAR_RATIO;
	m_tAngleInterpolationDesc.interpolation_speed = 3.0f;
}

void CGameObject::Ready_PositionInterpolationValue(const _vec3& vEndPosition, float fSpd)
{
	m_tPosInterpolationDesc.is_start_interpolation = true;
	m_tPosInterpolationDesc.v1 = m_pTransCom->m_vPos;
	m_tPosInterpolationDesc.v2 = vEndPosition;
	m_tPosInterpolationDesc.linear_ratio = Engine::MIN_LINEAR_RATIO;
	m_tPosInterpolationDesc.interpolation_speed = fSpd;
}

HRESULT CGameObject::Ready_GameObjectPrototype()
{
	return S_OK;
}

HRESULT CGameObject::Ready_GameObject(const _bool & bIsCreate_Transform, 
									  const _bool & bIsCreate_Info,
									  const _bool& bIsCreate_BoundingBox,
									  const _bool& bIsCreate_BoundingSphere)
{
	if (bIsCreate_Transform)
	{
		m_pTransCom = CTransform::Create();
		NULL_CHECK_RETURN(m_pTransCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Transform", m_pTransCom);
	}

	if (bIsCreate_Info)
	{
		m_pInfoCom =CInfo::Create();;
		NULL_CHECK_RETURN(m_pInfoCom, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(L"Com_Info", m_pInfoCom);
	}

	m_bIsBoundingBox = bIsCreate_BoundingBox;
	if (bIsCreate_BoundingBox)
	{
		m_pBoundingBoxCom = static_cast<CColliderBox*>(m_pComponentMgr->Clone_Component(L"ColliderBox", ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pBoundingBoxCom, E_FAIL);
		m_pBoundingBoxCom->AddRef();
		m_mapComponent[ID_DYNAMIC].emplace(L"Com_BoundingBox", m_pBoundingBoxCom);
	}

	m_bIsBoundingSphere = bIsCreate_BoundingSphere;
	if (bIsCreate_BoundingSphere)
	{
		m_pBoundingSphereCom = static_cast<CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pBoundingSphereCom, E_FAIL);
		m_pBoundingSphereCom->AddRef();
		m_mapComponent[ID_DYNAMIC].emplace(L"Com_BoundingSphere", m_pBoundingSphereCom);
	}

	return S_OK;
}

HRESULT CGameObject::LateInit_GameObject()
{
	if (!m_bIsLateInit)
	{
		FAILED_CHECK_RETURN(this->LateInit_GameObject(), E_FAIL);
		m_bIsLateInit = true;
	}

	return S_OK;
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	Reset_Collider();

	// Skill CoolDown
	for (auto& pair : m_mapSkillCoolDown)
		pair.second.Update_CoolDownTime(fTimeDelta);

	if (nullptr != m_pTransCom)
		m_pTransCom->Update_Component(fTimeDelta);

	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return NO_EVENT;
}

_int CGameObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	return NO_EVENT;
}

void CGameObject::Process_Collision()
{
}

void CGameObject::Send_PacketToServer()
{
}

void CGameObject::CreateServerNumberFont()
{

	m_pFontServer = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFontServer, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFontServer->Ready_GameObject(L"", _vec2(500.f, 0.f), D2D1::ColorF::Yellow), E_FAIL);
}

void CGameObject::Update_ServerNumberFont(const _float& fTimeDelta)
{	
	m_wstrText = wstring(L"[ Server Number  ] - \n") +
		to_wstring(Get_ServerNumber());

	wsprintf(m_szText, m_wstrText.c_str());

	m_pFontServer->Update_GameObject(fTimeDelta);
	m_pFontServer->Set_Text(wstring(m_szText));

	_vec3 ScreenPos = m_pTransCom->m_vPos.Convert_ProjectionToScreen(
		*Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW),
			*Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION),
			Engine::CGraphicDevice::Get_Instance()->Get_Viewport());
		m_pFontServer->Set_Pos(_vec2(ScreenPos.x, ScreenPos.y));
		m_pFontServer->Update_GameObject(fTimeDelta);
		m_pFontServer->Set_Text(wstring(m_szText));
}

void CGameObject::Render_GameObject(const _float & fTimeDelta)
{
}

void CGameObject::Render_ShadowDepth(const _float & fTimeDelta)
{
}

void CGameObject::Render_EdgeGameObject(const _float& fTimeDelta)
{
}

void CGameObject::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
}

void CGameObject::Render_MiniMap(const _float& fTimeDelta)
{
}

void CGameObject::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList * pCommandList, const _int& iContextIdx)
{
}

void CGameObject::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList * pCommandList, const _int& iContextIdx)
{
}

void CGameObject::Update_PartyMember(const int& iSNum, const int& hp, const int& maxHp, const int& mp, const int& maxMp)
{
	m_mapPartyList[iSNum].Update_Info(hp, maxHp, mp, maxMp);
}

void CGameObject::Buff_AllPartyMemeber(const int& buff)
{
	for (auto& p : m_mapPartyList)
	{
		switch (buff)
		{
		// 이속
		case 7:
		{
		}
		break;

		// 베리어
		case 8:
		{
		}
		break;

		// hp
		case 9:
		{
			p.second.iHp += 100;
			if (p.second.iHp >= p.second.iMaxHp)
				p.second.iHp = p.second.iMaxHp;
		}
		break;

		// mp
		case 12:
		{
			p.second.iMp += 50;
			if (p.second.iMp >= p.second.iMaxMp)
				p.second.iMp = p.second.iMaxMp;
		}
		break;
		}
	}	
}

void CGameObject::SetUp_OthersIsInMyParty()
{
	// 미니맵 && 외곽선 재설정.
	OBJLIST* pOthersList = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"Others");
	if (nullptr != pOthersList && !pOthersList->empty())
	{
		for (auto& pOthers : *pOthersList)
			pOthers->Set_IsThisPlayerPartyMember(false);

		if (!m_mapPartyList.empty())
		{
			for (auto& pOthers : *pOthersList)
			{
				auto iter_find = m_mapPartyList.find(pOthers->Get_ServerNumber());

				if (iter_find != m_mapPartyList.end())
					pOthers->Set_IsThisPlayerPartyMember(true);
			}
		}
	}
}

void CGameObject::Render_HitEffect(const _float& fTimeDelta)
{
	if (m_bisHitted)
	{
		m_fHitVelocity += fTimeDelta;
		m_fRedColor =sin(XMConvertToRadians(m_fHitVelocity * 180.f * 7.f) ) *0.5f;
		if (m_fRedColor < 0.f)
		{
			m_fRedColor = 0.f;
			m_bisHitted = false;
			m_fHitVelocity = 0.f;
		}
	}
}

HRESULT CGameObject::Add_Component()
{
	return S_OK;
}

HRESULT CGameObject::SetUp_MiniMapComponent(const _uint& uiMiniMapTexIdx)
{
	m_uiMiniMapTexIdx = uiMiniMapTexIdx;

	m_pTransMiniMap = CTransform::Create();
	NULL_CHECK_RETURN(m_pTransMiniMap, E_FAIL);

	// Buffer
	m_pBufferMiniMap = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferMiniMap, E_FAIL);

	// Texture
	m_pTextureMiniMap = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"S1UI_MIniMap", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureMiniMap, E_FAIL);

	// Shader
	m_pShaderMiniMap = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderMiniMap, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShaderMiniMap->Set_PipelineStatePass(7), E_FAIL);

	m_pShaderMiniMap->SetUp_ShaderConstantBuffer();

	return S_OK;
}

void CGameObject::Add_CollisionList(CGameObject* pDst)
{
	if (pDst != this)
		m_lstCollisionDst.emplace_back(pDst);
}

void CGameObject::Clear_CollisionList()
{
	m_lstCollisionDst.clear();
}

void CGameObject::SetUp_BillboardMatrix()
{
	if (nullptr != m_pTransCom)
	{
		_matrix* pmatView = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::VIEW);
		if (nullptr == pmatView)
			return;

		_matrix matBillboard = (*pmatView);
		memset(&matBillboard._41, 0, sizeof(_vec3));

		matBillboard = MATRIX_INVERSE(matBillboard);

		/*__________________________________________________________________________________________________________
		[ GameObject의 Scale값 반영. ]
		____________________________________________________________________________________________________________*/
		_float fScale[3] = { m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z };

		for (_int i = 0; i < 3; ++i)
		{
			for (_int j = 0; j < 4; ++j)
			{
				matBillboard(i, j) *= fScale[i];
			}
		}

		/*__________________________________________________________________________________________________________
		[ Scale + Rotate 입력 ]
		____________________________________________________________________________________________________________*/
		memcpy(&m_pTransCom->m_matWorld._11, &matBillboard._11, sizeof(_vec3));
		memcpy(&m_pTransCom->m_matWorld._21, &matBillboard._21, sizeof(_vec3));
		memcpy(&m_pTransCom->m_matWorld._31, &matBillboard._31, sizeof(_vec3));
	}

}
void CGameObject::SetUp_BillboardMatrix(_matrix* matrix, _vec3 vecScale)
{
	_matrix* pmatView = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::VIEW);
	if (nullptr == pmatView)
		return;

	_matrix matBillboard = (*pmatView);
	memset(&matBillboard._41, 0, sizeof(_vec3));
	matBillboard = MATRIX_INVERSE(matBillboard);
	_float fScale[3] = { vecScale.x, vecScale.y,vecScale.z };

	for (_int i = 0; i < 3; ++i)
	{
		for (_int j = 0; j < 4; ++j)
		{
			matBillboard(i, j) *= fScale[i];
		}
	}
	memcpy(&(*matrix)._11, &matBillboard._11, sizeof(_vec3));
	memcpy(&(*matrix)._21, &matBillboard._21, sizeof(_vec3));
	memcpy(&(*matrix)._31, &matBillboard._31, sizeof(_vec3));


}
void CGameObject::SetUp_BoundingBox(_matrix* pParent,
									const _vec3& vParentScale,
									const _vec3& vCenter, 
									const _vec3& vMin,
									const _vec3& vMax,
									const _float& fScaleOffset,
									const _vec3& vPosOffset)
{
	if (nullptr != m_pBoundingBoxCom)
	{
		_vec3 vScale = _vec3(0.0f);
		vScale.x = abs(vMax.x - vMin.x);
		vScale.y = abs(vMax.y - vMin.y);
		vScale.z = abs(vMax.z - vMin.z);
		vScale *= fScaleOffset;

		m_pBoundingBoxCom->Set_ParentMatrix(pParent);
		m_pBoundingBoxCom->Set_Scale(vScale);
		m_pBoundingBoxCom->Set_Extents(vParentScale);
		m_pBoundingBoxCom->Set_Pos(vCenter + vPosOffset);
	}
}

void CGameObject::SetUp_BoundingSphere(_matrix* pParent, 
									   const _vec3& vParentScale,
									   const _vec3& vScale,
									   const _vec3& vPos)
{
	if (nullptr != m_pBoundingSphereCom)
	{
		m_pBoundingSphereCom->Set_Pos(vPos);
		m_pBoundingSphereCom->Set_ParentMatrix(pParent);
		m_pBoundingSphereCom->Set_Scale(vScale);
		m_pBoundingSphereCom->Set_Radius(vParentScale);

	}
}

void CGameObject::Reset_Collider()
{
	for (auto& pCollider : m_lstCollider)
		pCollider->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
}

void CGameObject::Compute_ViewZ(_vec4& vPosInWorld)
{
	_matrix*	pmatView	= CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::VIEW);
	_matrix		matViewInv	= XMMatrixInverse(nullptr, *pmatView);

	_vec4 vCamPositionInWorld;
	memcpy(&vCamPositionInWorld, &matViewInv._41, sizeof(_vec4));

	_vec4 vDir = (vPosInWorld) - vCamPositionInWorld;

	m_fViewZ = vDir.Get_Length();
}


CComponent * CGameObject::Find_Component(wstring wstrComponentTag, const COMPONENTID & eID)
{
	auto iter_find = m_mapComponent[eID].find(wstrComponentTag);

	if (iter_find == m_mapComponent[eID].end())
		return nullptr;

	return iter_find->second;
}

void CGameObject::SetUp_MiniMapRandomY()
{
	if (nullptr != m_pTransMiniMap && !m_bIsInitMiniMapDepth)
	{
		m_bIsInitMiniMapDepth = true;

		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<_int>	uid{ 0, 10'000 };

		_float random_value = (_float)(uid(dre));
		m_pTransMiniMap->m_vPos.y = random_value * 0.0001f;
	}
}

CGameObject * CGameObject::Clone_GameObject()
{
	return nullptr;
}

void CGameObject::Free()
{
	if (m_bIsBoundingBox)
		Safe_Release(m_pBoundingBoxCom);

	if (m_bIsBoundingSphere)
		Safe_Release(m_pBoundingSphereCom);

	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	if(nullptr != m_pFontServer)
		Safe_Release(m_pFontServer);

	if (nullptr != m_pTransMiniMap)
		Safe_Release(m_pTransMiniMap);

	if (nullptr != m_pBufferMiniMap)
		Safe_Release(m_pBufferMiniMap);

	if (nullptr != m_pTextureMiniMap)
		Safe_Release(m_pTextureMiniMap);

	if (nullptr != m_pShaderMiniMap)
		Safe_Release(m_pShaderMiniMap);

	m_mapPartyList.clear();
		
}
