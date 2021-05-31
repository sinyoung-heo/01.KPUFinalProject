#pragma once
#include "CollisionTick.h"

namespace Engine
{
	class CMesh;
}

const _float ARROW_MAX_DISTANCE = 15.0f;

class CEffectTrail;
class CCollisionArrow : public CCollisionTick
{
private:
	explicit CCollisionArrow(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCollisionArrow() = default;

public:
	wstring Get_MeshTag() { return m_wstrMeshTag; }

	void Set_ArrowType(const ARROW_TYPE& eType) { m_eType = eType; }
	void Set_OriginPos(const _vec3& vPos)		{ m_vOriginPos = vPos; }
	void Set_IsReadyArrowFall(const _bool& bIsReady) { m_bIsReadyArrowFall = bIsReady; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const ARROW_TYPE& eType,
									 const _uint& uiDamage,
									 const _float& fLifeTime);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx);
	void			SetUp_Dissolve(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring		m_wstrMeshTag		       = L"";
	_uint		m_iMeshPipelineStatePass   = 0;
	_uint		m_iShadowPipelineStatePass = 0;

	ARROW_TYPE		m_eType             = ARROW_TYPE::ARROW_TYPE_END;
	ARROW_POOL_TYPE m_ePoolType         = ARROW_POOL_TYPE::ARROW_POOL_END;
	_float			m_fSpeed            = 40.0f;
	_vec3			m_vOriginPos        = _vec3(0.0f);
	_bool			m_bIsReadyArrowFall = false;

	/*__________________________________________________________________________________________________________
	[ Trail ]
	____________________________________________________________________________________________________________*/
	CEffectTrail* m_pTrail           = nullptr;

	//Effect
	int m_iEffectCnt = 5;
	float m_fEffectDeltatime = 1.f;

	_bool	m_bIsCreateCollisionTick = false;
	_bool	m_bIsStartDissolve       = false;
	_float	m_fDissolve              = -0.05f;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const ARROW_TYPE& eType,
									   const _uint& uiDamage,
									   const _float& fLifeTime);

	static CCollisionArrow**	Create_InstancePool(ID3D12Device* pGraphicDevice,
													ID3D12GraphicsCommandList* pCommandList,
													wstring wstrMeshTag,
													const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

