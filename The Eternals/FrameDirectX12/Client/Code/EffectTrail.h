#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CTrailTex;
	class CTexture;
	class CShaderTexture;
}

// const _uint TRAIL_SIZE = 25;


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

	void SetUp_TrailByCatmullRom(_vec3* vMin, _vec3* vMax);

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
private:
	void Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CTrailTex*		m_pBufferCom  = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CShaderTexture*	m_pShaderCom  = nullptr;

	/*__________________________________________________________________________________________________________
	[ Trail Buffer ]
	____________________________________________________________________________________________________________*/
	array<_vec3, Engine::TRAIL_SIZE - 1> m_arrMax;

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

