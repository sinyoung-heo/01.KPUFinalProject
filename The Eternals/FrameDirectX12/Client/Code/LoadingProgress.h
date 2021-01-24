#pragma once
#include "Include.h"
#include "GameObject.h"

class CLoadingProgress : public Engine::CGameObject
{
private:
	explicit CLoadingProgress(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CLoadingProgress() = default;

public:
	void			Set_LoadingPercent(const _float& fPercent) { m_fPercent = fPercent; }

	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiTexIdx	= 0;
	_vec3	m_vConvert	= _vec3(0.0f, 0.0f, 0.0f);
	_matrix	m_matView	= INIT_MATRIX;
	_matrix	m_matProj	= INIT_MATRIX;

	_float	m_fPercent	= 0.0f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrTextureTag);
private:
	virtual void Free();
};

