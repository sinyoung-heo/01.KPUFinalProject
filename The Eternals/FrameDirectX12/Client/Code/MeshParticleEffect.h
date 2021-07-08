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
class CMeshParticleEffect : public Engine::CGameObject
{
private:
	explicit CMeshParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMeshParticleEffect() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,const _int& PipeLine=9, const _int& ParticleCnt = 20);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_Pivot(_bool bisPivot) { m_bisPivot = bisPivot; }

	void Set_RenderingParticleCnt(_int iParticleCnt) { m_iParticleCnt = iParticleCnt; }
	void Set_ColorOffset(_rgba Color) { m_vecColorOffset = Color; }

	void Set_TexInfo(_int D=0, _int N=0, _int S=0, _int Sha=0, _int Dis=0) { m_uiDiff = D, m_uiNorm = N, m_uiSpec = S, m_uiShadow = Sha, m_uiDissove = Dis; }

	void Make_ParticleTrail(const _float& fTimeDelta);
	void Make_TextureTrail(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable(float fTimeDelta=0.f);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/

	
	_vec3 ParticlePos[PARTICLECNT];
	_vec3 ParticleScale[PARTICLECNT];
	_vec3 ParticleAngle[PARTICLECNT];
	bool ParticleInit[PARTICLECNT];
	_vec3 m_vecRandomvector[PARTICLECNT];
	_matrix						m_matWorld[PARTICLECNT];

	_float m_fCurGravity[PARTICLECNT];
	_vec2  ParticleSpeed_Weight[PARTICLECNT];

	_int m_iParticleCnt = 20;
	wstring	m_wstrMeshTag = L"";

	_uint m_uiDiff=0;
	_uint m_uiNorm=0;
	_uint m_uiSpec=0;
	_uint m_uiShadow = 0;
	_uint m_uiDissove = 0;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiTexIdx = 0;
	_rgba m_vecColorOffset;

	_float m_fAlpha = 1.f;
	_float m_fSpeed = 0.8f;
	_float m_fDegree = 0.f;
	_bool m_bisPivot;
	_int m_Pipeline = 0;

	ID3D12DescriptorHeap* m_pTextureHeap = nullptr;

	Engine::CMesh* m_pMeshCom = nullptr;
	Engine::CShaderMeshEffect* m_pShaderCom = nullptr;

	_bool m_bisMakeTexTrail = false;
	float m_fMakeTexTrail = 0.f;
	_bool m_bisMakeParticleTrail = false;
public:
	void Set_CreateInfo(const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos, _vec2 SpeedWeight, _bool isMakeTexTrail = false, _bool isMakeParticleTrail = false,
		const _int& PipeLine = 9, const _int& ParticleCnt = 20
	);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos, const _int& PipeLine=5, const _int& ParticleCnt=20);


	static CMeshParticleEffect** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList, wstring MeshTag = L"publicAxe",
		const _uint& uiInstanceCnt=20);
private:
	virtual void Free();
};

