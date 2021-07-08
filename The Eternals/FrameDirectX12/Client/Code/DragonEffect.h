#pragma once

#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderDynamicMeshEffect;
}

class CNormalMonsterHpGauge;

class CDragonEffect : public Engine::CGameObject
{
private:
	explicit CDragonEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CDragonEffect() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	
	virtual void	Render_GameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	//virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom				= nullptr;
	Engine::CShaderDynamicMeshEffect*		m_pShaderCom			= nullptr;
	
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrMeshTag            = L"";
	
	_float	m_fDissolve              = -0.05f;
	_rgba	m_vEmissiveColor         = _rgba(1.0f, 0.0f, 0.0f, 1.0f);

	_uint	m_uiAnimIdx			= 0;	// 현재 애니메이션 Index
	_float m_fGridTime = 0.f;

	_float m_fFireDecalDelta = 0.f;
	_bool m_bisFireDecal = false;
	
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);

	static CDragonEffect** Create_InstancePool(ID3D12Device* pGraphicDevice,
											  ID3D12GraphicsCommandList* pCommandList, 
											  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

