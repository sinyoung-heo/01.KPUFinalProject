#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CShaderTexture;
}

class CSmokeParticle : public Engine::CGameObject
{
private:
	explicit CSmokeParticle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CSmokeParticle() = default;

public:
	void Set_Instnace(wstring wstrTextueTag, const FRAME& tFrame, const _vec3& vPos, const _vec3& vDir);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale, const _vec3 & vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();
	void			Update_SpriteFrame(const _float& fTimeDelta);
	void			Make_BillboardMatrix(_matrix& matrix, const _vec3& vScale);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag	= L"";
	_uint	m_uiTexIdx			= 0;
	FRAME	m_tFrame			{ };
	_float	m_fAlpha = 0.0f;
	_bool	m_bIsAlphaDecrease = false;
	_vec3	m_vScaleSpeed = _vec3(9.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vPos);

	static CSmokeParticle** Create_InstancePool(ID3D12Device* pGraphicDevice,
												ID3D12GraphicsCommandList* pCommandList,
												const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

