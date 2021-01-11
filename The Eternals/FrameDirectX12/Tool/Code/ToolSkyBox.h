#pragma once
#include "GameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTexture;
	class CShaderSkyBox;
}

class CToolSkyBox : public Engine::CGameObject
{
private:
	explicit CToolSkyBox(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CToolSkyBox(const CToolSkyBox& rhs);
	virtual ~CToolSkyBox() = default;

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

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CCubeTex*		m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderSkyBox*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag	= L"";
	_uint	m_uiTexIdx			= 0;
	_bool	m_bIsUpdate			= true;

public:
	static CToolSkyBox* Create(ID3D12Device* pGraphicDevice, 
							   ID3D12GraphicsCommandList* pCommandList,
							   wstring wstrTextureTag,
							   const _vec3 & vScale,
							   const _vec3 & vAngle,
							   const _vec3 & vPos);
private:
	virtual void Free();
};

