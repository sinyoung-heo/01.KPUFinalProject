#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CEffectTrail : public Engine::CGameObject
{
private:
	explicit CEffectTrail(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CEffectTrail() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag,
									 const _uint& uiTexIdx,
									 const _vec3& vScale,
									 const _vec3& vAngle);
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
	wstring	m_wstrTextureTag = L"";
	_uint	m_uiTexIdx		 = 0;

public:
	static CEffectTrail* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
								wstring wstrTextureTag,
								const _uint& uiTexIdx,
								const _vec3& vScale = _vec3(0.1f),
								const _vec3& vAngle = _vec3(0.0f));
private:
	virtual void Free();
};

