#pragma once
#include "PCWeapon.h"

class CPCWeaponBow : public CPCWeapon
{
private:
	explicit CPCWeaponBow(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCWeaponBow() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
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

public:
	static CPCWeaponBow* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
								wstring wstrMeshTag, 
								const _vec3 & vScale, 
								const _vec3 & vAngle, 
								const _vec3 & vPos,
								Engine::HIERARCHY_DESC* pHierarchyDesc,
								_matrix* pParentMatrix,
								const _rgba& vEmissiveColor);

	static CPCWeaponBow** Create_InstancePool(ID3D12Device* pGraphicDevice,
											  ID3D12GraphicsCommandList* pCommandList, 
											  wstring wstrMeshTag,
											  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

