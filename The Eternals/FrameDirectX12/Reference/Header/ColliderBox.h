#pragma once
#include "VIBuffer.h"

#include "Transform.h"
#include "ShaderColor.h"


BEGIN(Engine)

class ENGINE_DLL CColliderBox final : public CVIBuffer
{
private:
	explicit CColliderBox(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CColliderBox(const CColliderBox& rhs);
	virtual ~CColliderBox() = default;

public:
	// Get
	CTransform*			Get_Transform()		{ return m_pTransCom; }
	BoundingBox&		Get_BoundingInfo() 	{ return m_BoundingInfo; }
	const _vec3&		Get_Length() const	{ return m_vLength; }

	// Set
	void				Set_Pos(const _vec3& vPos)						{ m_pTransCom->m_vPos	= vPos; }
	void				Set_Scale(const _vec3& vScale)					{ m_pTransCom->m_vScale = vScale; }
	void				Set_Color(const _rgba& vColor)					{ m_vColor = vColor; }
	void				Set_ParentMatrix(_matrix* pMatrix)				{ m_pParentMatrix = pMatrix; }
	void				Set_SkinningMatrix(SKINNING_MATRIX* pMatrix)	{ m_pSkinningMatrix = pMatrix; }
	HRESULT				Set_PipelineStatePass(const _uint& iIdx = 0)	{ return m_pShaderCom->Set_PipelineStatePass(iIdx); }
	void				Set_Extents(const _vec3& vParentScale);
	
	// Method
	HRESULT				Ready_Buffer();			
	HRESULT				Ready_Collider();		

	// CComponent을(를) 통해 상속됨
	virtual void		Update_Component(const _float& fTimeDelta);
	virtual void		Render_Component(const _float& fTimeDelta);

	void				Begin_Buffer();
	void				Render_Buffer();

private:
	HRESULT				Add_Component();
	void				Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CTransform*								m_pTransCom			= nullptr;
	CShaderColor*							m_pShaderCom		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	BoundingBox								m_BoundingInfo;
	array<_vec3, BoundingBox::CORNER_COUNT> m_arrCorners;

	_vec3									m_vLength			= INIT_VEC3(0.f);
	_rgba									m_vColor			= _rgba(0.0f, 1.0f, 0.0f, 1.0f);

	SKINNING_MATRIX*						m_pSkinningMatrix	= nullptr;
	_matrix*								m_pParentMatrix		= nullptr;

public:
	virtual CComponent*		Clone();
	static	CColliderBox*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

END