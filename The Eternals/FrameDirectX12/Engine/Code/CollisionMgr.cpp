#include "CollisionMgr.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(CCollisionMgr)

void CCollisionMgr::Add_CollisionCheckList(CGameObject* pGameObj)
{
	if (nullptr != pGameObj)
		m_lstCollisionCheckList.emplace_back(pGameObj);
}

void CCollisionMgr::Progress_SweapAndPrune()
{
	if (m_lstCollisionCheckList.empty())
		return;

	IS_OVERLAP eIsOverlap = OVERLAP_END;

	/*__________________________________________________________________________________________________________
	[ CollisionList에 등록된 Object를 X축 정렬을 한다. ]
	____________________________________________________________________________________________________________*/
	// Sort by PosX.
	m_lstCollisionCheckList.sort([](CGameObject* pSrc, CGameObject* pDst)
								 {
									return (pSrc->Get_Transform()->m_vPos.x < pDst->Get_Transform()->m_vPos.x);
								 });
	// X Overlap Check.
	auto iter_src_begin = m_lstCollisionCheckList.begin();
	auto iter_dst_begin = iter_src_begin;
	auto iter_end	    = m_lstCollisionCheckList.end();

	for (; iter_src_begin != iter_end;)
	{
		// src_begin 이전의 Object부터 비교 시작.
		for (; iter_dst_begin != iter_end; ++iter_dst_begin)
		{
			if (iter_src_begin == iter_dst_begin)
				continue;

			if (Check_OverlapSphere(OVERLAP_AXIS::AXIS_X, *iter_src_begin, *iter_dst_begin, eIsOverlap))
				m_lstCollisionPairList_X[*iter_src_begin].emplace_back(*iter_dst_begin);

			if (IS_OVERLAP::OVERLAP_OVER == eIsOverlap)
				break;
		} 

		iter_dst_begin = iter_src_begin;
		++iter_src_begin;
	}


	// Y Overlap Chcek.
	for (auto& pair : m_lstCollisionPairList_X)
	{
		pair.second.sort([](CGameObject* pSrc, CGameObject* pDst)
						 {
							return (pSrc->Get_Transform()->m_vPos.y < pDst->Get_Transform()->m_vPos.y);
						 });

		for (auto& pDst : pair.second)
		{
			if (Check_OverlapSphere(OVERLAP_AXIS::AXIS_Y, pair.first, pDst, eIsOverlap))
				m_lstCollisionPairList_XY[pair.first].emplace_back(pDst);

			if (IS_OVERLAP::OVERLAP_OVER == eIsOverlap)
				break;
		}
	}

	// Z Overlap Check.
	for (auto& pair : m_lstCollisionPairList_XY)
	{
		pair.second.sort([](CGameObject* pSrc, CGameObject* pDst)
						 {
							return (pSrc->Get_Transform()->m_vPos.z < pDst->Get_Transform()->m_vPos.z);
						 });

		for (auto& pDst : pair.second)
		{
			if (Check_OverlapSphere(OVERLAP_AXIS::AXIS_Z, pair.first, pDst, eIsOverlap))
				m_lstCollisionPairList_XYZ[pair.first].emplace_back(pDst);

			if (IS_OVERLAP::OVERLAP_OVER == eIsOverlap)
				break;
		}
	}

}

void CCollisionMgr::Clear_CollisionContainer()
{
	// Clear CollisionList
	m_lstCollisionCheckList.clear();

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

_bool CCollisionMgr::Check_OverlapSphere(const OVERLAP_AXIS& eOverlap,
										 CGameObject* pSrc,
										 CGameObject* pDst,
										 IS_OVERLAP& eIsOverlap)
{
	_float fSum	 = pSrc->Get_BoundingSphere()->Get_BoundingInfo().Radius + 
				   pDst->Get_BoundingSphere()->Get_BoundingInfo().Radius;
	_float fDist = 0.0f;

	_vec3 vSrc = pSrc->Get_Transform()->m_vPos;
	_vec3 vDst = pDst->Get_Transform()->m_vPos;

	if (AXIS_X == eOverlap)
	{
		fDist = abs(vSrc.x - vDst.x);

		if (vSrc.x < vDst.x)
			eIsOverlap = IS_OVERLAP::OVERLAP_OVER;
		else
			eIsOverlap = IS_OVERLAP::OVERLAP_UNDER;

	}

	else if (AXIS_Y == eOverlap)
	{
		fDist = abs(vSrc.y - vDst.y);

		if (vSrc.y < vDst.y)
			eIsOverlap = IS_OVERLAP::OVERLAP_OVER;
		else
			eIsOverlap = IS_OVERLAP::OVERLAP_UNDER;
	}

	else if (AXIS_Z == eOverlap)
	{
		fDist = abs(vSrc.z - vDst.z);

		if (vSrc.z < vDst.z)
			eIsOverlap = IS_OVERLAP::OVERLAP_OVER;
		else
			eIsOverlap = IS_OVERLAP::OVERLAP_UNDER;

		if (fDist <= fSum)
		{
			eIsOverlap = IS_OVERLAP::OVERLAP_TRUE;
			pSrc->Add_CollisionList(pDst);
			return true;
		}
	}

	if (fDist <= fSum)
	{
		eIsOverlap = IS_OVERLAP::OVERLAP_TRUE;
		return true;
	}

	return false;
}

void CCollisionMgr::Free()
{
	Clear_CollisionContainer();
}
