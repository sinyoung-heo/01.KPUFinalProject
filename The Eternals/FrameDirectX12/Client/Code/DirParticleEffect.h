#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

#define DIRPARTICLECNT 40
class CDirParticleEffect : public Engine::CGameObject
{
private:
	explicit CDirParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CDirParticleEffect() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f), const _int& PipeLine=9, const _int& ParticleCnt = 20);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_RenderingParticleCnt(_int iParticleCnt) { m_iParticleCnt = iParticleCnt; }
	void Set_ColorOffset(_rgba Color) { m_vecColorOffset = Color; }

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable(float fTimeDelta);
	void			Update_SpriteFrame(const _float& fTimeDelta, FRAME* pFrame);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CShaderTexture* m_pShaderCom;

	_vec3 ParticlePos[DIRPARTICLECNT];
	_vec3 ParticleScale[DIRPARTICLECNT];
	_vec3 ParticleAngle[DIRPARTICLECNT];
	_matrix m_matWorld[DIRPARTICLECNT];
	FRAME m_ParticleFrame[DIRPARTICLECNT];
	_vec3 m_vecRandomvector[DIRPARTICLECNT];
	_float m_fRandomVelocity[DIRPARTICLECNT];
	float m_fDist[DIRPARTICLECNT];
	float m_fCurDist[DIRPARTICLECNT];
	float ParticleAlpha[DIRPARTICLECNT];
	_vec3 m_DstPos;
	wstring m_strTextag = L"";
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag = L"";
	_uint	m_uiTexIdx = 0;

	_rgba m_vecColorOffset;
	FRAME	m_tFrame{ };
	_float m_fAlpha = 1.f;
	_int m_Pipeline = 0;
	_int m_iParticleCnt = 40;

	ID3D12DescriptorHeap* m_pTextureHeap = nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos, wstring TexTag = L"Snow",
		const _vec3& StartPos = _vec3(0.f),
		const _vec3& DstPos = _vec3(0.f),
		const FRAME& tFrame = FRAME(1, 1, 0.0f), const _int& PipeLine = 9, const _int& ParticleCnt = 20
	);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrTextureTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const FRAME& tFrame = FRAME(1, 1, 0.0f), const _int& PipeLine=9, const _int& ParticleCnt=20);


	static CDirParticleEffect** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt,wstring TexTag=L"Snow");
private:
	virtual void Free();
};

