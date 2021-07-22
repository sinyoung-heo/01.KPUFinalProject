#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CShaderTexture;
}

class CBreathParticle : public Engine::CGameObject
{
private:
	explicit CBreathParticle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CBreathParticle() = default;

public:
	void Set_Instnace(wstring wstrTextueTag, const _uint& uiTexSize, const _vec3& vPos, const _vec3& vDir);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vPos,
									 const _vec3& vDir);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();
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
	_float	m_fTexIdx			= 0.0f;
	_uint	m_uiTexSize         = 0;
	_float	m_fFrameSpeed       = 0.0f;
	_float	m_fAlpha            = 1.f;
	_bool	m_bIsCalcAlpha      = false;

	_float m_fLifeTime       = 0.0f;
	_float m_fUpdateLifeTime = 0.0f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vPos,
									   const _vec3& vDir);

	static CBreathParticle** Create_InstancePool(ID3D12Device* pGraphicDevice,
												 ID3D12GraphicsCommandList* pCommandList,
												 const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

