#pragma once
#include "PCWeapon.h"
#include "EffectTrail.h"

class CPCWeaponTwoHand : public CPCWeapon
{
private:
	explicit CPCWeaponTwoHand(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCWeaponTwoHand() = default;

public:
	const _bool& Get_IsRenderTrail() { return m_pTrail->Get_IsRenderTrail(); }

	void Set_IsRenderTrail(const _bool& bIsRenderTrail) { m_pTrail->Set_IsRenderTrail(bIsRenderTrail); }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 Engine::HIERARCHY_DESC* pHierarchyDesc,
									 _matrix* pParentMatrix,
									 const _rgba& vEmissiveColor);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	CEffectTrail*	m_pTrail       = nullptr;

public:
	static CPCWeaponTwoHand* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									wstring wstrMeshTag, 
									const _vec3 & vScale, 
									const _vec3 & vAngle, 
									const _vec3 & vPos,
									Engine::HIERARCHY_DESC* pHierarchyDesc,
									_matrix* pParentMatrix,
									const _rgba& vEmissiveColor);
private:
	virtual void Free();

};

