#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

enum OVERLAP { OVERLAP_X, OVERLAP_Y, OVERLAP_Z, OVERLAP_END };

class CGameObject;

class ENGINE_DLL CCollisionMgr final :  public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr() = default;
	virtual ~CCollisionMgr() = default;

public:
	void	Add_CollisionCheckList(wstring wstrCollisionTag, CGameObject * pGameObj);
	void	Progress_SweapAndPrune();
	void	Check_Collision();
	void	Clear_CollisionContainer();
private:
	_bool	Check_OverlapSphere(const OVERLAP& eOverlap, CGameObject* pSrc, CGameObject* pDst);
	_bool	Check_Sphere(CGameObject* pSrc, CGameObject* pDst);

private:
	map<wstring, list<CGameObject*>>	m_mapCollisionCheckList;
	list<CGameObject*>					m_lstCollisionCheckList;

	map<CGameObject*, list<CGameObject*>> m_lstCollisionPairList_X;
	map<CGameObject*, list<CGameObject*>> m_lstCollisionPairList_XY;
	map<CGameObject*, list<CGameObject*>> m_lstCollisionPairList_XYZ;

private:
	virtual void Free();
};

END