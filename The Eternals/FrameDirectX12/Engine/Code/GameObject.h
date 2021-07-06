#pragma once
#include "ComponentMgr.h"
#include "Renderer.h"
#include "CollisionMgr.h"

BEGIN(Engine)

class CObjectMgr;

class CFont;
class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject();
	explicit CGameObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	// Get
	CComponent*				Get_Component(wstring wstrComponentTag, COMPONENTID eID);
	CTransform*				Get_Transform()			{ return m_pTransCom; }
	CTransform*				Get_MiniMapTransform()	{ return m_pTransMiniMap; }
	CInfo*					Get_Info()				{ return m_pInfoCom; }
	CColliderBox*			Get_BoundingBox()		{ return m_pBoundingBoxCom;}
	CColliderSphere*		Get_BoundingSphere()	{ return m_pBoundingSphereCom; }
	wstring					Get_CollisionTag()		{ return m_wstrCollisionTag; }
	_float					Get_DepthOfView()		{ return m_fViewZ; }
	_long					Get_UIDepth()			{ return m_UIDepth; }
	const _bool&			Get_IsDead()			{ return m_bIsDead; }
	const _bool&			Get_IsCollision()		{ return m_bIsCollision; }
	const _bool&			Get_IsRenderShadow()	{ return m_bIsRenderShadow;}
	const int&				Get_ServerNumber()		{ return m_iSNum; }
	const bool&				Get_MoveStop()			{ return m_bIsMoveStop; }
	const char&				Get_OType()				{ return m_chO_Type; }
	list<CColliderSphere*>& Get_ColliderList()		{ return m_lstCollider; }
	const _uint&			Get_InstanceIdx()		{ return m_uiInstanceIdx; }
	const _bool&			Get_IsUsingInstance()	{ return m_bIsUsingInstance; }
	const _bool&			Get_IsAlphaObject()		{ return m_bisAlphaObject; }
	const map<int, PARTYMEMBER>& Get_PartyList()	{ return m_mapPartyList; }
	const _bool&			Get_PartyState()		{ return m_bIsPartyState; }
	const _int&				Get_PartySuggestSNum()	{ return m_iSuggesterNumber;}
	const _bool&			Get_RequestParty()		{ return m_bIsPartyRequest; }
	const _bool&			Get_PartyJoinRequest()	{ return m_bIsPartyJoinRequest; }
	const char&				Get_CurrentStageID()	{ return m_chCurStageID; }
	const char&				Get_WeaponType()		{ return m_chCurWeaponType; }
	const _bool&			Get_IsThisPlayerPartyMember()	{ return m_bIsThisPlayerPartyMember; }
	map<wstring, _int>&		Get_ThilsPlayerSkillKeyInput()	{ return m_mapSkillKeyInput; }
	const high_resolution_clock::time_point& Get_LastMoveTime() { return m_last_move_time; }
	// Set
	void	Set_CurrentStageID(const char& chStageID)				{ m_chCurStageID = chStageID; }
	void	Set_UIDepth(const _long& iUIDepth)						{ m_UIDepth = iUIDepth; }
	void	Set_DeadGameObject()									{ m_bIsDead = true; }
	void	Set_IsReturnObject(const _bool& bIsReturn)				{ m_bIsReturn = bIsReturn; }
	void	Set_BoundingBoxColor(const _rgba& vColor)				{ m_pBoundingBoxCom->Set_Color(vColor); }
	void	Set_BoundingSphereColor(const _rgba& vColor)			{ m_pBoundingSphereCom->Set_Color(vColor); }
	void	Set_IsCollision(const _bool& bIsCollision)				{ m_bIsCollision = bIsCollision; }
	void	Set_CollisionTag(wstring wstrCollisionTag)				{ m_wstrCollisionTag = wstrCollisionTag; }
	void	Set_IsRenderShadow(const _bool& bIsRenderShadow)		{ m_bIsRenderShadow = bIsRenderShadow; }
	void	Set_ServerNumber(const int& num)						{ m_iSNum = num; }
	void	Set_LastMoveTime(const steady_clock::time_point& last)	{ m_last_move_time = last; }
	void	Set_MoveStop(const bool& b)								{ m_bIsMoveStop = b; }
	void	Set_Attack(const bool& b)								{ m_bIsAttack = b; }
	void	Set_OType(const char& chType)							{ m_chO_Type = chType; }
	void	Set_DeadReckoning(const _vec3& vPos);
	void	Set_Info(const int& lev, const int& hp, const int& maxHp, const int& mp, const int& maxMp, const int& exp, const int& maxExp, const int& minatt, const int& maxatt, const int& spd, const int& money);
	void	Set_State(const int& cur) { m_iMonsterStatus = cur; }
	void	Set_Buff(const int& hp, const int& maxHp, const int& mp, const int& maxMp);
	void	Set_PotionAbility(const int& ability, const bool& bIsHP);
	void	Set_Money(const int& money) { m_pInfoCom->m_iMoney = money; }
	void	Set_Other_direction(_vec3& vDir);
	float	Set_Other_Angle(_vec3& vDir);
	void	Set_IsStartPosInterpolation(const _bool& bIsStart)		{ m_tPosInterpolationDesc.is_start_interpolation = bIsStart; }
	void	Set_LinearPos(const _vec3& v1, const _vec3& v2)			{ m_tPosInterpolationDesc.v1 = v1; m_tPosInterpolationDesc.v2 = v2; }
	void	Set_IsStartAngleInterpolation(const _bool& bIsStart)	{ m_tAngleInterpolationDesc.is_start_interpolation = bIsStart; }
	void	Set_LinearAngle(const _float& v1, const _float& v2)		{ m_tAngleInterpolationDesc.v1 = v1; m_tAngleInterpolationDesc.v2 = v2; }
	void	Set_IsUsingInstance(const _bool& bIsUsing)				{ m_bIsUsingInstance = bIsUsing; }
	void	Set_WeaponType(const char& chWeaponType)				{ m_chCurWeaponType = chWeaponType; }
	void	Request_Party(const int& suggester_num)					{ m_bIsPartyRequest = true; m_iSuggesterNumber = suggester_num; }
	void	Set_RequestParty(const _bool& bIsRequest)				{ m_bIsPartyRequest = bIsRequest; }
	void	JoinRequest_Party(const int& joinner_num)				{ m_bIsPartyJoinRequest = true; m_iSuggesterNumber = joinner_num;}
	void	Set_JoinRequest(const _bool& bIsRequest)				{ m_bIsPartyJoinRequest = bIsRequest; }
	void	Set_PartyState(const _bool& st)							{ m_bIsPartyState = st; }
	void	Set_PartySuggestSNum(const _int& iNum)					{ m_iSuggesterNumber = iNum; }
	void	Set_IsThisPlayerPartyMember(const _bool& bIsPartyMember) { m_bIsThisPlayerPartyMember = bIsPartyMember; }
	void	Set_ThilsPlayerSkillKeyInput(wstring wstrTag, const _uint& uiKey) { m_mapSkillKeyInput[wstrTag] = uiKey; }
	void	Ready_AngleInterpolationValue(const _float& fEndAngle);
	void	Ready_PositionInterpolationValue(const _vec3& vEndPosition, float fSpd = 3.f);

	virtual void Set_AnimationIdx(const _uint& iIdx) { }
	virtual void Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack) { }
	virtual void Set_OthersStance(const _bool& bIsStanceAttack) { }
	virtual void Reset_AttackMoveInterpolationRatio() { }

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

	// ServerNumber
	virtual void	CreateServerNumberFont();
	virtual void	Update_ServerNumberFont(const _float& fTimeDelta);
	// SingleThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);
	virtual void	Render_EdgeGameObject(const _float& fTimeDelta);
	virtual void	Render_CrossFilterGameObject(const _float& fTimeDelta);
	virtual void	Render_MiniMap(const _float& fTimeDelta);
	// MultiThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

	void			Add_CollisionList(CGameObject* pDst);
	void			Clear_CollisionList();

	// Setting Party List
	void			Enter_PartyMember(const int& iSNum, const PARTYMEMBER memberInfo)	{ m_mapPartyList[iSNum] = memberInfo; }
	void			Leave_PartyMember(const int& iSNum)									{ m_mapPartyList.erase(iSNum); }
	void			Update_PartyMember(const int& iSNum, const int& hp, const int& maxHp, const int& mp, const int& maxMp);
	void			Buff_AllPartyMemeber(const int& buff);
	void			Clear_PartyMember()													{ m_mapPartyList.clear(); }
	void			SetUp_OthersIsInMyParty();
	//Red value
	void			Render_HitEffect(const _float& fTimeDelta);
	void			Set_bisHitted(bool isHitted) { m_bisHitted = isHitted; }
protected:
	HRESULT			Add_Component();
	HRESULT			SetUp_MiniMapComponent(const _uint& uiMiniMapTexIdx);
	void			SetUp_BillboardMatrix();
	void			SetUp_BoundingBox(_matrix* pParent, const _vec3& vParentScale, const _vec3& vCenter, const _vec3& vMin, const _vec3& vMax, const _float& fScaleOffset = 1.0f, const _vec3& vPosOffset = _vec3(0.0f));
	void			SetUp_BoundingSphere(_matrix* pParent, const _vec3& vParentScale, const _vec3& vScale, const _vec3& vPos);
	void			Reset_Collider();
	void			Compute_ViewZ(_vec4& vPosInWorld);
	CComponent*		Find_Component(wstring wstrComponentTag, const COMPONENTID& eID);
	void			SetUp_MiniMapRandomY();
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

	// MiniMap
	CTransform*		m_pTransMiniMap		= nullptr;
	CRcTex*			m_pBufferMiniMap	= nullptr;
	CTexture*		m_pTextureMiniMap	= nullptr;
	CShaderTexture*	m_pShaderMiniMap	= nullptr;
	_uint			m_uiMiniMapTexIdx   = 0;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_float	m_fDeltaTime		= 0.f;
	_bool	m_bIsDead			= false;
	_bool	m_bIsReturn			= false;
	_bool	m_bIsLateInit		= false;
	_float	m_fViewZ			= 0.0f;
	_long	m_UIDepth			= 0;
	_bool	m_bIsInitMiniMapDepth = false;

	_bool	m_bIsCollision		= false;
	_bool	m_bIsBoundingBox	= false;
	_bool	m_bIsBoundingSphere = false;
	_bool	m_bIsRenderShadow	= false;

	_uint	m_uiInstanceIdx		= 0;
	_bool	m_bIsUsingInstance  = false;
	_bool   m_bisAlphaObject    = false;

	_float m_fRedColor    = 0.f; // 몬스터전용 변수
	_bool m_bisHitted     = false;
	_float m_fHitVelocity = 1.f;
	
	// Player Skill Key Input
	map<wstring, _int>	m_mapSkillKeyInput;

	/*__________________________________________________________________________________________________________
	[ Collision ]
	____________________________________________________________________________________________________________*/
	wstring					m_wstrCollisionTag;
	list<CGameObject*>		m_lstCollisionDst;
	list<CColliderSphere*>	m_lstCollider;

	/*__________________________________________________________________________________________________________
	[ SERVER ]
	__________________________________________________________________________________________________________*/
	int		m_iSNum				= 0;
	int		m_iMonsterStatus	= 0;
	bool	m_bIsMoveStop		= true;
	bool	m_bIsAttack			= false;
	char	m_chO_Type			= 0;

	char	m_chCurWeaponType	= -1;
	char	m_chPreWeaponType	= -1;

	char	m_chPreStageID		= -1;
	char	m_chCurStageID		= -1;
	
	CFont*	m_pFontServer		= nullptr;
	wstring	m_wstrText			= L"";
	_tchar	m_szText[MAX_STR]	= L"";

	// Party System
	map<int, PARTYMEMBER> m_mapPartyList;
	_bool	m_bIsPartyRequest		= false; // --> 파티 초대 (유/무)
	_bool	m_bIsPartyJoinRequest	= false; // --> 파티 참여 신청 요청 (유/무)
	_bool	m_bIsPartyState			= false; // --> 파티 참여 (유/무)
	_int	m_iSuggesterNumber		= -1;	 // --> 파티 초대자 or 참여 신청자 
	_bool	m_bIsThisPlayerPartyMember = false;

	// Linear Interpolation Desc
	LINEAR_INTERPOLATION_DESC<_vec3>	m_tPosInterpolationDesc;
	LINEAR_INTERPOLATION_DESC<_float>	m_tAngleInterpolationDesc;

	high_resolution_clock::time_point	m_last_move_time;

public:
	virtual CGameObject* Clone_GameObject();
protected:
	virtual void Free();
};

END