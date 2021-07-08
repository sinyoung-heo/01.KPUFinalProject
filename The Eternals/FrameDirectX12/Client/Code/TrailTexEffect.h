#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

#define PARTICLECNT 40
class CTrailTexEffect : public Engine::CGameObject
{
private:
	explicit CTrailTexEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CTrailTexEffect() = default;

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

	void SetTransform(_vec3 * parent) { m_pParentTransform = parent; }
private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable(_int i);
	void			Update_SpriteFrame(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	float idx = 0.f;
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

	_rgba m_vecColorOffset;
	FRAME	m_tFrame{ };
	_float m_fAlpha = 1.f;
	_int m_Pipeline = 0;
	_int m_iParticleCnt = 20;

	ID3D12DescriptorHeap* m_pTextureHeap = nullptr;
	_vec3* m_pParentTransform = nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos, wstring TexTag = L"Snow",Engine::CTransform* ParentTransform=nullptr,
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


	static CTrailTexEffect** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt,wstring TexTag=L"Snow");
private:
	virtual void Free();
};

