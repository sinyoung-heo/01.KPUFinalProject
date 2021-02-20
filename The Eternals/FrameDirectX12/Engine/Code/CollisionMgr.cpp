#include "CollisionMgr.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(CCollisionMgr)

void CCollisionMgr::Add_CollisionCheckList(wstring wstrCollisionTag, CGameObject* pGameObj)
{
	auto iter_find = m_mapCollisionCheckList.find(wstrCollisionTag);
	
	if (iter_find == m_mapCollisionCheckList.end())
	{
		m_mapCollisionCheckList[wstrCollisionTag] = list<CGameObject*>();
		m_mapCollisionCheckList[wstrCollisionTag].emplace_back(pGameObj);
	}
	else
		m_mapCollisionCheckList[wstrCollisionTag].emplace_back(pGameObj);

	m_lstCollisionCheckList.emplace_back(pGameObj);
}

void CCollisionMgr::Progress_SweapAndPrune()
{
	set<CGameObject*> setCollisionObject;

	/*__________________________________________________________________________________________________________
	[ CollisionList에 등록된 Object를 X축 정렬을 한다. ]
	____________________________________________________________________________________________________________*/
	// OriginPos 저장.
	for (auto& pObject : m_lstCollisionCheckList)
		pObject->Get_Transform()->Set_OriginPos();

	// Sort by PosX.
	m_lstCollisionCheckList.sort([](CGameObject* pSrc, CGameObject* pDst)
								 {
									return (pSrc->Get_Transform()->m_vPos.x < pDst->Get_Transform()->m_vPos.x);
								 });
	// X Overlap Check.
	for (auto& pSrc : m_lstCollisionCheckList)
	{
		for (auto& pDst : m_lstCollisionCheckList)
		{
			if (pSrc == pDst)
				continue;

			if (Check_OverlapSphere(OVERLAP::OVERLAP_X, pSrc, pDst))
			{
				m_lstCollisionPairList_X[pSrc].emplace_back(pDst);
				setCollisionObject.emplace(pSrc);
				setCollisionObject.emplace(pDst);
			}
			else
				break;
		}
	}

	// Sort by PosY
	list<CGameObject*> lstOverlapX{ setCollisionObject.begin(), setCollisionObject.end() };
	lstOverlapX.sort([](CGameObject* pSrc, CGameObject* pDst)
					 {
					 return (pSrc->Get_Transform()->m_vPos.y < pDst->Get_Transform()->m_vPos.y);
					 });
	setCollisionObject.clear();

	// Y Overlap Check.
	for (auto& pSrc : lstOverlapX)
	{
		for (auto& pDst : lstOverlapX)
		{
			if (pSrc == pDst)
				continue;

			if (Check_OverlapSphere(OVERLAP::OVERLAP_Y, pSrc, pDst))
			{
				m_lstCollisionPairList_XY[pSrc].emplace_back(pDst);
				setCollisionObject.emplace(pSrc);
				setCollisionObject.emplace(pDst);
			}
			else
				break;
		}
	}


	// Sort by PosZ
	list<CGameObject*> lstOverlapXY{ setCollisionObject.begin(), setCollisionObject.end() };
	lstOverlapXY.sort([](CGameObject* pSrc, CGameObject* pDst)
					 {
					 return (pSrc->Get_Transform()->m_vPos.z < pDst->Get_Transform()->m_vPos.z);
					 });
	setCollisionObject.clear();

	// Y Overlap Check.
	for (auto& pSrc : lstOverlapXY)
	{
		for (auto& pDst : lstOverlapXY)
		{
			if (pSrc == pDst)
				continue;

			if (Check_OverlapSphere(OVERLAP::OVERLAP_Z, pSrc, pDst))
			{
				m_lstCollisionPairList_XYZ[pSrc].emplace_back(pDst);
			}
			else
				break;
		}
	}

	// Reset Pos by Origin.
	for (auto& pObject : m_lstCollisionCheckList)
		pObject->Get_Transform()->Set_PosByOrigin();
}

void CCollisionMgr::Check_Collision()
{
	for (auto& pSrc : m_lstCollisionCheckList)
	{
		for (auto& pDst : m_lstCollisionCheckList)
		{
			if (pSrc == pDst)
				continue;

			if (Check_Sphere(pSrc, pDst))
			{
				pSrc->Set_BoundingSphereColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pSrc->Set_BoundingBoxColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDst->Set_BoundingSphereColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDst->Set_BoundingBoxColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}
}

void CCollisionMgr::Clear_CollisionContainer()
{
	m_lstCollisionCheckList.clear();

	// Clear CheckList
	for (auto& pair : m_mapCollisionCheckList)
		pair.second.clear();
	m_mapCollisionCheckList.clear();

	// Clear PairList
	for (auto& pair : m_lstCollisionPairList_X)
		pair.second.clear();
	m_lstCollisionPairList_X.clear();

	for (auto& pair : m_lstCollisionPairList_XY)
		pair.second.clear();
	m_lstCollisionPairList_XY.clear();

	for (auto& pair : m_lstCollisionPairList_XYZ)
		pair.second.clear();
	m_lstCollisionPairList_XYZ.clear();

}

_bool CCollisionMgr::Check_OverlapSphere(const OVERLAP& eOverlap, CGameObject* pSrc, CGameObject* pDst)
{
	_float	fSrcRadius = pSrc->Get_BoundingSphere()->Get_BoundingInfo().Radius;
	_float	fDstRadius = pDst->Get_BoundingSphere()->Get_BoundingInfo().Radius;
	_float	fSum       = fSrcRadius + fDstRadius;

	_vec3	vSrcCenter = pSrc->Get_Transform()->m_vPos;
	_vec3	vDstCenter = pDst->Get_Transform()->m_vPos;
	_float	fDist      = 0.0f;

	if (OVERLAP_X == eOverlap)
		fDist = abs(vSrcCenter.x - vDstCenter.x);

	else if (OVERLAP_Y == eOverlap)
		fDist = abs(vSrcCenter.y - vDstCenter.y);

	else if (OVERLAP_Z == eOverlap)
	{
		fDist = abs(vSrcCenter.z - vDstCenter.z);
		if (fDist <= fSum)
		{
			pSrc->Add_CollisionList(pDst);
			return true;
		}
	}

	if (fDist <= fSum)
		return true;

	return false;
}

_bool CCollisionMgr::Check_Sphere(CGameObject* pSrc, CGameObject* pDst)
{
	_float	fSrcRadius = pSrc->Get_BoundingSphere()->Get_BoundingInfo().Radius;
	_float	fDstRadius = pDst->Get_BoundingSphere()->Get_BoundingInfo().Radius;
	_float	fSum = fSrcRadius + fDstRadius;

	_vec3	vSrcCenter = _vec3(pSrc->Get_BoundingSphere()->Get_BoundingInfo().Center);
	_vec3	vDstCenter = _vec3(pDst->Get_BoundingSphere()->Get_BoundingInfo().Center);
	_float	fDist = 0.0f;

	if (fSum >= vSrcCenter.Get_Distance(vDstCenter))
		return true;

	return false;
}

void CCollisionMgr::Free()
{
	Clear_CollisionContainer();
}
