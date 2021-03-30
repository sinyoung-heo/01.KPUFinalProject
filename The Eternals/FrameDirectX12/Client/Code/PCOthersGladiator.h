#pragma once
#include "Include.h"
#include "GameObject.h"
#include "GladiatorAnimation.h"
#include "PCWeaponTwoHand.h"

namespace PCOthersGladiatorConst
{
	const _float MAX_SPEED = 4.5f;
	const _float MIN_SPEED = 0.0f;
	const _float MOVE_STOP_SPEED = 3.0f;
}

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CNaviMesh;
}

class CPCOthersGladiator : public Engine::CGameObject
{
private:
	explicit CPCOthersGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCOthersGladiator() = default;

public:
	void Set_AnimationIdx(const _uint& iIdx) { m_uiAnimIdx = iIdx; }
	void Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack);

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
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			Move_OnNaviMesh(const _float& fTimeDelta);
	void			SetUp_MoveSpeed(const _float& fTimeDelta);
	void			SetUp_StanceChange(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom	   = nullptr;
	Engine::CShaderMesh*	m_pShaderCom   = nullptr;
	Engine::CShaderShadow*	m_pShadowCom   = nullptr;
	Engine::CNaviMesh*		m_pNaviMeshCom = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CPCWeaponTwoHand*	m_pWeapon                 = nullptr;
	wstring				m_wstrMeshTag             = L"";
	Gladiator::STANCE	m_ePreStance              = Gladiator::STANCE_END;
	Gladiator::STANCE	m_eCurStance              = Gladiator::STANCE_END;
	_bool				m_bIsCompleteStanceChange = true;
	char				m_chWeaponType            = -1;

	// Speed Linear Interpolation
	_float	m_fLinearRatio = 0.0f;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint m_uiAnimIdx        = 0;	// 현재 애니메이션 Index
	_uint m_ui3DMax_NumFrame = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint m_ui3DMax_CurFrame = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치

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

