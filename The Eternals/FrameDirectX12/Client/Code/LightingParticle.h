#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CLightingParticle : public Engine::CGameObject
{
private:
	explicit CLightingParticle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CLightingParticle() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f));
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_Pivot(bool bisPivot) { m_bisPivot = bisPivot; }
	void Set_Speed(float fSpeed) { m_fSpeed = fSpeed; }
private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable();
	void			Update_SpriteFrame(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CShaderTexture* m_pShaderCom = nullptr;

	wstring m_strTextag = L"";
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag = L"";
	_uint	m_uiTexIdx = 0;

	FRAME	m_tFrame{ };
	_vec3 m_vecRandomvector;
	float m_fAlpha = 1.f;
	float m_fSpeed = 0.8f;
	bool m_bisPivot;
	bool m_bisAddPos = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f));
private:
	virtual void Free();
};

