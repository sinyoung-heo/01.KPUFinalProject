#pragma once
#include "Include.h"
#include "GameObject.h"


namespace Engine
{
	class CRcCol;
	class CShaderColor;
}

class CRectObject : public Engine::CGameObject
{
private:
	explicit CRectObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CRectObject(const CRectObject& rhs);
	virtual ~CRectObject() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcCol*			m_pBufferCom = nullptr;
	Engine::CShaderColor*	m_pShaderCom = nullptr;

public:
	static CRectObject* Create(ID3D12Device* pGraphicDevice, 
							   ID3D12GraphicsCommandList* pCommandList,
							   const _vec3 & vScale,
							   const _vec3 & vAngle,
							   const _vec3 & vPos);
private:
	virtual void Free();
};

