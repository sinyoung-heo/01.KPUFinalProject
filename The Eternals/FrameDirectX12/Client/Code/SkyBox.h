#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTexture;
	class CShaderSkyBox;
}

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag, 
									 const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos);
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
	Engine::CCubeTex*		m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderSkyBox*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag				= L"";
	_uint	m_uiTexIdx						= 0;

public:
	static CSkyBox* Create(ID3D12Device* pGraphicDevice, 
						   ID3D12GraphicsCommandList* pCommandList,
						   wstring wstrTextureTag,
						   const _vec3 & vScale,
						   const _vec3 & vAngle,
						   const _vec3 & vPos);
private:
	virtual void Free();
};

