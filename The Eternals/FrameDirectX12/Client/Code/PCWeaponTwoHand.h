#pragma once
#include "PCWeapon.h"

class CPCWeaponTwoHand : public CPCWeapon
{
private:
	explicit CPCWeaponTwoHand(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCWeaponTwoHand() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 Engine::HIERARCHY_DESC* pHierarchyDesc,
									 _matrix* pParentMatrix);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

public:
	static CPCWeaponTwoHand* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									wstring wstrMeshTag, 
									const _vec3 & vScale, 
									const _vec3 & vAngle, 
									const _vec3 & vPos,
									Engine::HIERARCHY_DESC* pHierarchyDesc,
									_matrix* pParentMatrix);
private:
	virtual void Free();

};

