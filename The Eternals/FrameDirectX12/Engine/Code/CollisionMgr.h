#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

enum OVERLAP_AXIS	{ AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
enum IS_OVERLAP		{ OVERLAP_UNDER, OVERLAP_TRUE, OVERLAP_OVER, OVERLAP_END };

class CGameObject;



class ENGINE_DLL CCollisionMgr final :  public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr() = default;
	virtual ~CCollisionMgr() = default;

public:
	void	Add_CollisionCheckList(CGameObject * pGameObj);
	void	Progress_SweapAndPrune();
	void	Clear_CollisionContainer();
private:
	_bool	Check_OverlapSphere(const OVERLAP_AXIS& eOverlap, CGameObject* pSrc, CGameObject* pDst, IS_OVERLAP& eIsOverlap);

private:
	list<CGameObject*>						m_lstCollisionCheckList;
	map<CGameObject*, list<CGameObject*>>	m_lstCollisionPairList_X;
	map<CGameObject*, list<CGameObject*>>	m_lstCollisionPairList_XY;
	map<CGameObject*, list<CGameObject*>>	m_lstCollisionPairList_XYZ;

private:
	virtual void Free();
};

END