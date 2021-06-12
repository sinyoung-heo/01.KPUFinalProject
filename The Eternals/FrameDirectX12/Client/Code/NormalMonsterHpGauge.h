#pragma once
#include "Include.h"
#include "GameObject.h"

class CNormalMonsterHpGauge : public Engine::CGameObject
{
private:
	explicit CNormalMonsterHpGauge(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CNormalMonsterHpGauge() = default;

public:
	void Set_Percent(const _float& fPercent) { m_fPercent = fPercent; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3& vPos, const _vec3& vScale);
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
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_float	m_fPercent = 1.0f;
	_vec3	m_vConvert = _vec3(0.0f);

	wstring					m_wstrTextureTag     = L"";
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap = nullptr;
	_uint					m_uiTexIdx			 = 0;
	FRAME					m_tFrame;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   const _vec3& vPos,
									   const _vec3& vScale);
protected:
	virtual void Free();
};

