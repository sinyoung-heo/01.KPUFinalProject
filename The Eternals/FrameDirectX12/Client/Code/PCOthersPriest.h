#pragma once
#include "Include.h"
#include "GameObject.h"
#include "PriestAnimation.h"
#include "PCWeaponRod.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CNaviMesh;
}

class CPCOthersPriest : public Engine::CGameObject
{
private:
	explicit CPCOthersPriest(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCOthersPriest() = default;

public:
	virtual void Set_AnimationIdx(const _uint& iIdx) { m_uiAnimIdx = iIdx; m_pMeshCom->Set_AnimationKey(m_uiAnimIdx); }
	virtual void Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack);
	virtual void Set_OthersStance(const _bool& bIsStanceAttack);

	// CGameObject��(��) ���� ��ӵ�
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
	void	SetUp_WeaponRHand();
	void	SetUp_WeaponBack();

	//Effect
void			Effect_Loop(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom				= nullptr;
	Engine::CShaderMesh*	m_pShaderCom			= nullptr;
	Engine::CShaderMesh*	m_pEdgeObjectShaderCom	= nullptr;
	Engine::CShaderShadow*	m_pShadowCom			= nullptr;
	Engine::CNaviMesh*		m_pNaviMeshCom			= nullptr;
	Engine::CNaviMesh*		m_pOriginNaviMeshCom    = nullptr;
	Engine::CNaviMesh*		m_pVelikaNaviMeshCom	= nullptr;
	Engine::CNaviMesh*		m_pBeachNaviMeshCom		= nullptr;
	Engine::CNaviMesh*		m_pWinterNaviMeshCom	= nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CServerMath*		m_pServerMath             = nullptr;
	CPCWeaponRod*		m_pWeapon                 = nullptr;
	wstring				m_wstrMeshTag             = L"";
	Priest::STANCE		m_ePreStance              = Priest::STANCE_END;
	Priest::STANCE		m_eCurStance              = Priest::STANCE_END;
	_bool				m_bIsCompleteStanceChange = true;
	_bool				m_bIsResetNaviMesh        = false;
	_bool				m_bIsCreateWeapon		  = false;

	// Speed Linear Interpolation
	Engine::LINEAR_INTERPOLATION_DESC<_float> m_tMoveSpeedInterpolationDesc;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx          = 0;	// ���� �ִϸ��̼� Index
	_uint	m_ui3DMax_NumFrame   = 0;	// 3DMax���� �ִϸ��̼��� �� Frame ����
	_uint	m_ui3DMax_CurFrame   = 0;	// 3DMAx���� ���� �ִϸ��̼��� Frame ��ġ
	_float	m_fBlendingSpeed     = 0.001f;
	_float	m_fAnimationSpeed    = TPS;
	_bool	m_bIsRepeatAnimation = true;


	//Effect
	_bool m_bisEffect = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const char& chWeaponType);

	static CPCOthersPriest** Create_InstancePool(ID3D12Device* pGraphicDevice,
												 ID3D12GraphicsCommandList* pCommandList, 
												 const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

