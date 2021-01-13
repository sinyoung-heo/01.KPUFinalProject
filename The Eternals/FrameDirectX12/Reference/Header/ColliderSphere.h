#pragma once
#include "VIBuffer.h"

#include "Transform.h"
#include "ShaderColor.h"


BEGIN(Engine)

class ENGINE_DLL CColliderSphere final : public CVIBuffer
{
private:
	explicit CColliderSphere(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CColliderSphere(const CColliderSphere& rhs);
	virtual ~CColliderSphere() = default;

public:
	// Get
	CTransform*		Get_Transform()		{ return m_pTransCom; }
	BoundingSphere&	Get_BoundingInfo() 	{ return m_BoundingInfo; }

	// Set
	void					Set_Pos(const _vec3& vPos)						{ m_pTransCom->m_vPos	= vPos; }
	void					Set_Scale(const _vec3& vScale)					{ m_pTransCom->m_vScale = vScale; }
	void					Set_Radius(const _vec3& vParentScale)			{ m_BoundingInfo.Radius *= m_pTransCom->m_vScale.x * vParentScale.x; };
	void					Set_Color(const _rgba& vColor)					{ m_vColor = vColor; }
	void					Set_ParentMatrix(_matrix* pMatrix)				{ m_pParentMatrix = pMatrix; }
	void					Set_SkinningMatrix(SKINNING_MATRIX* pMatrix)	{ m_pSkinningMatrix = pMatrix; }
	HRESULT					Set_PipelineStatePass(const _uint& iIdx = 0)	{ return m_pShaderCom->Set_PipelineStatePass(iIdx); }

	// Method
	HRESULT					Ready_Buffer(const _uint& uiSliceCnt, const _uint& uiStackCnt);	// Vertex & Index 정보 초기화.
	HRESULT					Ready_Collider();												// Component & Radius & 뼈 부모행렬 초기화.
	
	// CComponent을(를) 통해 상속됨
	virtual void			Update_Component(const _float& fTimeDelta);
	virtual void			Render_Component(const _float& fTimeDelta);

	void					Begin_Buffer();
	void					Render_Buffer();

private:
	HRESULT					Add_Component();
	void					Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CTransform*			m_pTransCom			= nullptr;
	CShaderColor*		m_pShaderCom		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	BoundingSphere		m_BoundingInfo;
	_rgba				m_vColor			= _rgba(0.0f, 1.0f, 0.0f, 1.0f);

	SKINNING_MATRIX*	m_pSkinningMatrix	= nullptr;
	_matrix*			m_pParentMatrix		= nullptr;

public:
	virtual CComponent*			Clone();
	static	CColliderSphere*	Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   const _uint& uiSliceCnt,
									   const _uint& uiStackCnt);
private:
	virtual void				Free();
};

END