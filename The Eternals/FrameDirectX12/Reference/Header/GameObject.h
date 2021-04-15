#pragma once
#include "ComponentMgr.h"
#include "Renderer.h"
#include "CollisionMgr.h"

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
	wstring				Get_CollisionTag()		{ return m_wstrCollisionTag; }
	_float				Get_DepthOfView()		{ return m_fViewZ; }
	_long				Get_UIDepth()			{ return m_UIDepth; }
	const _bool&		Get_IsDead()			{ return m_bIsDead; }
	const _bool&		Get_IsCollision()		{ return m_bIsCollision; }
	const _bool&		Get_IsRenderShadow()	{ return m_bIsRenderShadow;}
	const int&			Get_ServerNumber()		{ return m_iSNum; }
	const high_resolution_clock::time_point& Get_LastMoveTime() { return m_last_move_time; }
	const bool&			Get_MoveStop()			{ return m_bIsMoveStop; }
	const char&			Get_OType()				{ return m_chO_Type; }

	// Set
	void				Set_UIDepth(const _long& iUIDepth)						{ m_UIDepth = iUIDepth; }
	void				Set_DeadGameObject()									{ m_bIsDead = true; }
	void				Set_BoundingBoxColor(const _rgba& vColor)				{ m_pBoundingBoxCom->Set_Color(vColor); }
	void				Set_BoundingSphereColor(const _rgba& vColor)			{ m_pBoundingSphereCom->Set_Color(vColor); }
	void				Set_IsCollision(const _bool& bIsCollision)				{ m_bIsCollision = bIsCollision; }
	void				Set_IsRenderShadow(const _bool& bIsRenderShadow)		{ m_bIsRenderShadow = bIsRenderShadow; }
	void				Set_ServerNumber(const int& num)						{ m_iSNum = num; }
	void				Set_LastMoveTime(const steady_clock::time_point& last)	{ m_last_move_time = last; }
	void				Set_MoveStop(const bool& b)								{ m_bIsMoveStop = b; }
	void				Set_Attack(const bool& b)								{ m_bIsAttack = b; }
	void				Set_OType(const char& chType)							{ m_chO_Type = chType; }
	void				Set_DeadReckoning(const _vec3& vPos);
	void				Set_Info(int lev, int hp, int maxHp, int mp, int maxMp, int exp, int maxExp, int att, float spd);
	void				Set_State(int cur) { m_iMonsterStatus = cur; }
	void				Set_Other_direction(_vec3& vDir);
	float				Set_Other_Angle(_vec3& vDir);
	
	void				Set_IsStartPosInterpolation(const _bool& bIsStart)		{ m_tPosInterpolationDesc.is_start_interpolation = bIsStart; }
	void				Set_LinearPos(const _vec3& v1, const _vec3& v2)			{ m_tPosInterpolationDesc.v1 = v1; m_tPosInterpolationDesc.v2 = v2; }
	void				Set_IsStartAngleInterpolation(const _bool& bIsStart)	{ m_tAngleInterpolationDesc.is_start_interpolation = bIsStart; }
	void				Set_LinearAngle(const _float& v1, const _float& v2)		{ m_tAngleInterpolationDesc.v1 = v1; m_tAngleInterpolationDesc.v2 = v2; }

	void				Ready_AngleInterpolationValue(const _float& fEndAngle);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Ready_GameObjectPrototype();
	virtual HRESULT	Ready_GameObject(const _bool& bIsCreate_Transform = true,
									 const _bool& bIsCreate_Info = false,
									 const _bool& bIsCreate_BoundingBox = false,
									 const _bool& bIsCreate_BoundingSphere = false);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
	virtual void	Send_PacketToServer();

	// SingleThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);
	virtual void	Render_EdgeGameObject(const _float& fTimeDelta);
	virtual void	Render_CrossFilterGameObject(const _float& fTimeDelta);
	// MultiThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

	void			Add_CollisionList(CGameObject* pDst);
	void			Clear_CollisionList();
protected:
	HRESULT			Add_Component();
	void			SetUp_BillboardMatrix();
	void			SetUp_BoundingBox(_matrix* pParent, const _vec3& vParentScale, const _vec3& vCenter, const _vec3& vMin, const _vec3& vMax, const _float& fScaleOffset = 1.0f, const _vec3& vPosOffset = _vec3(0.0f));
	void			SetUp_BoundingSphere(_matrix* pParent, const _vec3& vParentScale, const _vec3& vScale, const _vec3& vPos);
	void			Compute_ViewZ(_vec4& vPosInWorld);
	CComponent*		Find_Component(wstring wstrComponentTag, const COMPONENTID& eID);
protected:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice & Mgr ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice = nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList	 = nullptr;
	CRenderer*					m_pRenderer		 = nullptr;
	CObjectMgr*					m_pObjectMgr	 = nullptr;
	CComponentMgr*				m_pComponentMgr	 = nullptr;
	CCollisionMgr*				m_pCollisonMgr   = nullptr;

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
	_float	m_fDeltaTime		= 0.f;
	_bool	m_bIsDead			= false;
	_bool	m_bIsLateInit		= false;
	_float	m_fViewZ			= 0.0f;
	_long	m_UIDepth			= 0;

	_bool	m_bIsCollision		= false;
	_bool	m_bIsBoundingBox	= false;
	_bool	m_bIsBoundingSphere = false;
	_bool	m_bIsRenderShadow	= false;
	
	/*__________________________________________________________________________________________________________
	[ Collision ]
	____________________________________________________________________________________________________________*/
	wstring				m_wstrCollisionTag;
	list<CGameObject*>	m_lstCollisionDst;

	/*__________________________________________________________________________________________________________
	SERVER
	__________________________________________________________________________________________________________*/
	int		m_iSNum				= 0;
	int		m_iMonsterStatus	= 0;
	bool	m_bIsMoveStop		= true;
	bool	m_bIsAttack			= false;
	char	m_chO_Type			= 0;

	// Linear Interpolation Desc
	LINEAR_INTERPOLATION_DESC<_vec3>	m_tPosInterpolationDesc;
	LINEAR_INTERPOLATION_DESC<_float>	m_tAngleInterpolationDesc;

	high_resolution_clock::time_point m_last_move_time;

public:
	virtual CGameObject* Clone_GameObject();
protected:
	virtual void Free();
};

END