#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

#define PARTICLECNT 20
class CParticleEffect : public Engine::CGameObject
{
private:
	explicit CParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CParticleEffect() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f), const int PipeLine=9);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_Pivot(bool bisPivot) { m_bisPivot = bisPivot; }
	void Set_Speed(float fSpeed) { m_fSpeed = fSpeed; }
private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable(int i);
	void			Update_SpriteFrame(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex* m_pBufferCom[PARTICLECNT] ;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CShaderTexture* m_pShaderCom[PARTICLECNT] ;
	_vec3 TempPos[PARTICLECNT];
	wstring m_strTextag = L"";
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag = L"";
	_uint	m_uiTexIdx = 0;

	FRAME	m_tFrame{ };
	_vec3 m_vecRandomvector[20];
	float m_fAlpha = 1.f;
	float m_fSpeed = 0.8f;
	float m_fDegree = 0.f;
	bool m_bisPivot;
	_int m_Pipeline = 0;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f), const int PipeLine=9);
private:
	virtual void Free();
};

