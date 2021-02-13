#pragma once
#include "ComponentMgr.h"
#include "Renderer.h"

BEGIN(Engine)

class CObjectMgr;


class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	// Get
	CComponent*			Get_Component(wstring wstrComponentTag, COMPONENTID eID);
	CTransform*			Get_Transform()			{ return m_pTransCom; }
	CInfo*				Get_Info()				{ return m_pInfoCom; }
	CColliderBox*		Get_BoundingBox()		{ return m_pBoundingBoxCom;}
	CColliderSphere*	Get_BoundingSphere()	{ return m_pBoundingSphereCom; }
	_float				Get_DepthOfView()		{ return m_fViewZ; }
	_long				Get_UIDepth()			{ return m_UIDepth; }
	const _bool&		Get_IsDead()			{ return m_bIsDead; }
	const _bool&		Get_IsCollision()		{ return m_bIsCollision; }
	const _bool&		Get_IsRenderShadow()	{ return m_bIsRenderShadow;}
	const int&			Get_ServerNumber()		{ return m_iSNum; }
	const high_resolution_clock::time_point& Get_LastMoveTime() { return m_last_move_time; }

	// Set
	void				Set_DeadGameObject()								{ m_bIsDead = true; }
	void				Set_BoundingBoxColor(const _rgba& vColor)			{ m_pBoundingBoxCom->Set_Color(vColor); }
	void				Set_BoundingSphereColor(const _rgba& vColor)		{ m_pBoundingSphereCom->Set_Color(vColor); }
	void				Set_IsCollision(const _bool& bIsCollision)			{ m_bIsCollision = bIsCollision; }
	void				Set_IsRenderShadow(const _bool& bIsRenderShadow)	{ m_bIsRenderShadow = bIsRenderShadow; }
	void				Set_ServerNumber(const int& num)					{ m_iSNum = num; }
	void				Set_LastMoveTime(const steady_clock::time_point& last) { m_last_move_time = last; }
	void				Set_DeadReckoning(const _vec3& vPos1, const _vec3& vPos2, const _vec3& vPos3, const _vec3& vPos4);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_GameObjectPrototype();
	virtual HRESULT	Ready_GameObject(const _bool& bIsCreate_Transform = true,
									 const _bool& bIsCreate_Info = false,
									 const _bool& bIsCreate_BoundingBox = false,
									 const _bool& bIsCreate_BoundingSphere = false);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	// SingleThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);

	// MultiThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

protected:
	virtual HRESULT Add_Component();
	void			SetUp_BillboardMatrix();
	void			SetUp_BoundingBox(_matrix* pParent, const _vec3& vParentScale, const _vec3& vCenter, const _vec3& vMin, const _vec3& vMax, const _float& fScaleOffset = 1.0f);
	void			SetUp_BoundingSphere(_matrix* pParent, const _vec3& vParentScale, const _vec3& vScale, const _vec3& vPos);
	void			Compute_ViewZ(_vec4& vPosInWorld);

private:
	CComponent*		Find_Component(wstring wstrComponentTag, const COMPONENTID& eID);

protected:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice & Mgr ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;
	CRenderer*					m_pRenderer			= nullptr;
	CObjectMgr*					m_pObjectMgr		= nullptr;
	CComponentMgr*				m_pComponentMgr		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	map<wstring, CComponent*>	m_mapComponent[COMPONENTID::ID_END];
	CTransform*					m_pTransCom				= nullptr;
	CInfo*						m_pInfoCom				= nullptr;
	CColliderBox*				m_pBoundingBoxCom		= nullptr;
	CColliderSphere*			m_pBoundingSphereCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_float m_fDeltaTime = 0.f;
	_bool	m_bIsDead			= false;
	_bool	m_bIsLateInit		= false;
	_float	m_fViewZ			= 0.0f;
	_long	m_UIDepth			= 0;

	_bool	m_bIsCollision		= false;
	_bool	m_bIsBoundingBox	= false;
	_bool	m_bIsBoundingSphere = false;
	_bool	m_bIsRenderShadow	= false;
	
	/* server */
	int		m_iSNum				= 0;
	high_resolution_clock::time_point m_last_move_time;

public:
	virtual CGameObject* Clone_GameObject();
protected:
	virtual void Free();
};

END