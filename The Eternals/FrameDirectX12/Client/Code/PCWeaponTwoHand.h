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
	void Set_TrailTextureIdx(const _uint& uiTexIdx)		{ m_pTrail->Set_TextureIdx(uiTexIdx); }
	void Set_IsRenderTrail(const _bool& bIsRenderTrail) { m_pTrail->Set_IsRenderTrail(bIsRenderTrail); /*m_pDistortionTrail->Set_IsRenderTrail(bIsRenderTrail);*/ }

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
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	CEffectTrail* m_pTrail = nullptr;
	// CEffectTrail* m_pDistortionTrail = nullptr;

public:
	static CPCWeaponTwoHand* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									wstring wstrMeshTag, 
									const _vec3 & vScale, 
									const _vec3 & vAngle, 
									const _vec3 & vPos,
									Engine::HIERARCHY_DESC* pHierarchyDesc,
									_matrix* pParentMatrix,
									const _rgba& vEmissiveColor);

	static CPCWeaponTwoHand** Create_InstancePool(ID3D12Device* pGraphicDevice,
												  ID3D12GraphicsCommandList* pCommandList, 
												  wstring wstrMeshTag,
												  const _uint& uiInstanceCnt);
private:
	virtual void Free();

};

