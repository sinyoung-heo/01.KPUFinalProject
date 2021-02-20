#include "GameObject.h"

#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "Renderer.h"
#include "ComponentMgr.h"
#include "Renderer.h"

USING(Engine)

CGameObject::CGameObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_pObjectMgr(CObjectMgr::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
	, m_pComponentMgr(CComponentMgr::Get_Instance())
	, m_pCollisonMgr(CCollisionMgr::Get_Instance())
{
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice)
	, m_pCommandList(rhs.m_pCommandList)
	, m_pRenderer(rhs.m_pRenderer)
	, m_pObjectMgr(rhs.m_pObjectMgr)
	, m_pComponentMgr(rhs.m_pComponentMgr)
	, m_bIsDead(rhs.m_bIsDead)
	, m_bIsLateInit(rhs.m_bIsLateInit)
	, m_fViewZ(rhs.m_fViewZ)
	, m_UIDepth(rhs.m_UIDepth)
{
}



CComponent * CGameObject::Get_Component(wstring wstrComponentTag, COMPONENTID eID)
{
	CComponent* pComponent = Find_Component(wstrComponentTag, eID);
	NULL_CHECK_RETURN(pComponent, nullptr);

	return pComponent;
}

void CGameObject::Set_DeadReckoning(const _vec3& vPos)
{
	m_pInfoCom->m_vecArivePos = vPos;	
}

void CGameObject::Set_npc_moveDir(const char& dir)
{
	m_npc_moveDir = dir;

	switch (m_npc_moveDir)
	{
	case MV_FRONT:
		m_pTransCom->m_vAngle.y = 0.f;
		break;
	case MV_BACK:
		m_pTransCom->m_vAngle.y = 180.f;
		break;
	case MV_RIGHT:
		m_pTransCom->m_vAngle.y = 90.f;
		break;
	case MV_RIGHT_UP:
		m_pTransCom->m_vAngle.y = 45.f;
		break;
	case MV_RIGHT_DOWN:
		m_pTransCom->m_vAngle.y = 135.f;
		break;
	case MV_LEFT:
		m_pTransCom->m_vAngle.y = 270.f;
		break;
	case MV_LEFT_UP:
		m_pTransCom->m_vAngle.y = 315.f;
		break;
	case MV_LEFT_DOWN:
		m_pTransCom->m_vAngle.y = 225.f;
		break;
	}
}

void CGameObject::Set_Other_direction(_vec3& vDir)
{
	m_pTransCom->m_vAngle.y = vDir.Get_Angle(g_vLook);
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
		m_mapComponent[ID_DYNAMIC].emplace(L"Com_Transform", m_pTransCom);
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

void CGameObject::Render_GameObject(const _float & fTimeDelta)
{
}

void CGameObject::Render_ShadowDepth(const _float & fTimeDelta)
{
}

void CGameObject::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList * pCommandList, const _int& iContextIdx)
{
}

void CGameObject::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList * pCommandList, const _int& iContextIdx)
{
}

HRESULT CGameObject::Add_Component()
{
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

void CGameObject::SetUp_BoundingBox(_matrix* pParent, 
									const _vec3& vParentScale,
									const _vec3& vCenter, 
									const _vec3& vMin,
									const _vec3& vMax,
									const _float& fScaleOffset)
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
		m_pBoundingBoxCom->Set_Pos(vCenter);
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
}
