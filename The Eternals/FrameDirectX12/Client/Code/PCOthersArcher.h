#pragma once
#include "Include.h"
#include "GameObject.h"
#include "ArcherAnimation.h"
#include "PCWeaponBow.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CNaviMesh;
}

class CPCOthersArcher :  public Engine::CGameObject
{
private:
	explicit CPCOthersArcher(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCOthersArcher() = default;

public:
	virtual void Set_AnimationIdx(const _uint& iIdx) { m_uiAnimIdx = iIdx; m_pMeshCom->Set_AnimationKey(m_uiAnimIdx); }
	virtual void Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack);
	virtual void Set_OthersStance(const _bool& bIsStanceAttack);
	virtual void Reset_AttackMoveInterpolationRatio() { m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f; }
	void Reset_CollisionArrowDesc();

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 wstring wstrNaviMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const char& chWeaponType);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_MiniMap(const _float& fTimeDelta);
	virtual void	Render_EdgeGameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
private:
	HRESULT	SetUp_PCWeapon();
	void	SetUp_StageID();
	void	Is_ChangeWeapon();
	void	Set_WeaponHierarchy();
	void	Set_IsRepeatAnimation();
	void	Set_AnimationSpeed();
	void	Set_BlendingSpeed();
	void	Set_ConstantTable();
	void	Set_ConstantTableShadowDepth();
	void	Set_ConstantTableMiniMap();
	void	Move_OnNaviMesh(const _float& fTimeDelta);
	void	SetUp_MoveSpeed(const _float& fTimeDelta);
	void	SetUp_StanceChange(const _float& fTimeDelta);
	void	SetUp_OthersAttackMove(const _uint& uiAniIdx, const _uint& uiStartTick, const _uint& uiStopTick, const _float& fMoveSpeed, const _float& fStopSpeed);
	void	AttackMove_OnNaviMesh(const _float& fTimeDelta);
	void	SetUp_WeaponLHand();
	void	SetUp_WeaponBack();
	void	SetUp_CollisionArrow(const _float& fTimeDelta);

	void Effect_Loop(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom	           = nullptr;
	Engine::CShaderMesh*	m_pShaderCom           = nullptr;
	Engine::CShaderMesh*	m_pEdgeObjectShaderCom = nullptr;
	Engine::CShaderShadow*	m_pShadowCom           = nullptr;
	Engine::CNaviMesh*		m_pNaviMeshCom         = nullptr;
	Engine::CNaviMesh*		m_pOriginNaviMeshCom    = nullptr;
	Engine::CNaviMesh*		m_pVelikaNaviMeshCom   = nullptr;
	Engine::CNaviMesh*		m_pBeachNaviMeshCom    = nullptr;
	Engine::CNaviMesh*		m_pWinterNaviMeshCom   = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CInstancePoolMgr*	m_pInstancePoolMgr = nullptr;
	CServerMath*		m_pServerMath             = nullptr;
	CPCWeaponBow*		m_pWeapon                 = nullptr;
	wstring				m_wstrMeshTag             = L"";
	Archer::STANCE		m_ePreStance              = Archer::STANCE_END;
	Archer::STANCE		m_eCurStance              = Archer::STANCE_END;
	_bool				m_bIsCompleteStanceChange = true;
	_bool				m_bIsResetNaviMesh        = false;
	_bool				m_bIsCreateWeapon		  = false;

	// CollisionTick
	_bool				m_bIsSetCollisionTick = false;
	COLLISION_TICK_DESC m_tCollisionTickDesc;
	_vec3				m_vArrowFallPos = _vec3(0.0f);

	_bool	m_bIsStartArrowFall    = false;
	_float	m_fArrowFallTime       = 0.0f;
	_float	m_fArrowFallUpdateTime = 1.0f / 60.0f;
	_int	m_iCurArrowFallCnt     = 0;
	_int	m_iMaxArrowFallCnt     = 96;

	//Effect
	_bool m_bisHandEffect = false;

	// Speed Linear Interpolation
	Engine::LINEAR_INTERPOLATION_DESC<_float> m_tMoveSpeedInterpolationDesc;
	Engine::LINEAR_INTERPOLATION_DESC<_float> m_tAttackMoveSpeedInterpolationDesc;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx          = 0;	// 현재 애니메이션 Index
	_uint	m_ui3DMax_NumFrame   = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint	m_ui3DMax_CurFrame   = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치
	_float	m_fBlendingSpeed     = 0.001f;
	_float	m_fAnimationSpeed    = TPS;
	_bool	m_bIsRepeatAnimation = true;

	bool m_bisUseShieldEffect = false;
	_float m_fUseShieldDelta = 0.f;
	_bool m_bisUseAxeEffect = false;
	_float m_fUseAxeDelta = 0.f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const char& chWeaponType);

	static CPCOthersArcher** Create_InstancePool(ID3D12Device* pGraphicDevice,
												 ID3D12GraphicsCommandList* pCommandList, 
												 const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

