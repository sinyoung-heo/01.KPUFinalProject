#pragma once
#include "Include.h"
#include "GameObject.h"
#include "ArcherAnimation.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CColliderSphere;
	class CColliderBox;
	class CFont;
	class CNaviMesh;
}

class CDynamicCamera;
class CPCWeaponBow;
class CInstancePoolMgr;

class CPCArcher : public Engine::CGameObject
{
private:
	explicit CPCArcher(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCArcher() = default;

public:
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
	virtual void	Process_Collision();
	virtual void	Send_PacketToServer();
	// SingleThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta);
	void			Render_AfterImage(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	HRESULT			SetUp_PCWeapon();
	HRESULT			SetUp_ClassFrame();
	void			SetUp_StageID();
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			Set_IsRepeatAnimation();
	void			Set_AnimationSpeed();
	void			Set_BlendingSpeed();

	void Key_Input(const _float& fTimeDelta);
	void KeyInput_Move(const _float& fTimeDelta);
	void KeyInput_Attack(const _float& fTimeDelta);
	void KeyInput_StanceChange(const _float& fTimeDelta);
	void KeyInput_AttackArrow(const _float& fTimeDelta);
	void KeyInput_SkillAttack(const _float& fTimeDelta);
	void KeyInput_BackDash(const _float& fTimeDelta);

	void Move_OnNaviMesh(const _float& fTimeDelta);
	void AttackMove_OnNaviMesh(const _float& fTimeDelta);
	void SetUp_RunMoveSpeed(const _float& fTimeDelta);
	void SetUp_RunAnimation();
	void SetUp_RunToIdleAnimation(const _float& fTimeDelta);
	void SetUp_PlayerStance_FromAttackToNoneAttack();
	void SetUp_PlayerStance_FromNoneAttackToAttack();
	void Change_PlayerStance(const _float& fTimeDelta);
	void SetUp_AttackMove(const _uint& uiAniIdx, const _uint& uiStartTick, const _uint& uiStopTick, const _float& fMoveSpeed, const _float& fStopSpeed);

	void Send_Player_Move();
	bool Is_Change_CamDirection();
	void Is_ChangeWeapon();
	void SetUp_AttackSetting();
	void SetUp_WeaponLHand();
	void SetUp_WeaponBack();
	void Ready_AngleInterpolationValue(const _float& fEndAngle);
	void SetUp_AngleInterpolation(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom           = nullptr;
	Engine::CShaderMesh*	m_pShaderCom         = nullptr;
	Engine::CShaderShadow*	m_pShadowCom         = nullptr;
	Engine::CNaviMesh*		m_pNaviMeshCom       = nullptr;
	Engine::CNaviMesh*		m_pVelikaNaviMeshCom = nullptr;
	Engine::CNaviMesh*		m_pBeachNaviMeshCom  = nullptr;

	/*__________________________________________________________________________________________________________
	[ Manager ]
	____________________________________________________________________________________________________________*/
	CPacketMgr*			m_pPacketMgr       = nullptr;
	CServerMath*		m_pServerMath      = nullptr;
	CInstancePoolMgr*	m_pInstancePoolMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CDynamicCamera*	m_pDynamicCamera = nullptr;
	CPCWeaponBow*	m_pWeapon        = nullptr;
	wstring			m_wstrMeshTag    = L"";

	// Speed Linear Interpolation
	Engine::LINEAR_INTERPOLATION_DESC<_float> m_tMoveSpeedInterpolationDesc;
	Engine::LINEAR_INTERPOLATION_DESC<_float> m_tAttackMoveSpeedInterpolationDesc;

	// AfterImage
	list<_matrix>	m_lstAFWorldMatrix;
	list<_rgba>		m_lstAFAlpha;
	_uint			m_uiAfterImgSize    = 0;
	_float			m_fAfterImgTime     = 0.f;
	_float			m_fAfterImgMakeTime = 0.f;
	_float			m_fAfterSubAlpha    = 0.f;
	_bool			m_bIsMakeAfterImage = false;

	// CollisionTick
	_bool				m_bIsSetCollisionTick = false;
	COLLISION_TICK_DESC m_tCollisionTickDesc;

	// Server
	_bool			m_bIsKeyDown      = false;
	_bool			m_bIsSameDir      = false;
	_bool			m_bIsAttack       = false;
	_bool			m_bIsSendMoveStop = false;
	_float			m_fBazierSpeed    = 0.f;
	MVKEY			m_eKeyState       = MVKEY::K_END;
	_float			m_fPreAngle       = 0.f;

	/*__________________________________________________________________________________________________________
	[ Animation ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx          = 0;	// 현재 애니메이션 Index
	_uint	m_ui3DMax_NumFrame   = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint	m_ui3DMax_CurFrame   = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치
	_float	m_fAnimationSpeed    = TPS;
	_bool	m_bIsRepeatAnimation = true;

	Archer::STANCE		m_eStance                 = Archer::STANCE_END;
	_bool				m_bIsCompleteStanceChange = true;
	_bool				m_bIsSkill				  = false;
	_bool				m_bIsSkillLoop			  = false;
	map<wstring, _int>	m_mapSkillKeyInput;
	_float				m_fBlendingSpeed		  = 0.001f;

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont           = nullptr;
	wstring			m_wstrText        = L"";
	_tchar			m_szText[MAX_STR] = L"";

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const char& chWeaponType);
private:
	virtual void Free();
};

