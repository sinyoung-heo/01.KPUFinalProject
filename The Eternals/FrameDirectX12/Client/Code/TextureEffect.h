#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CTextureEffect : public Engine::CGameObject
{
private:
	explicit CTextureEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CTextureEffect() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag, 
									 const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos,
									 const FRAME& tFrame = FRAME(1, 1, 0.0f));
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

	void Set_EffectInfo(int PipelineState=2, bool PlayerFollow=false);
	void Set_BillBoard(bool isBillBoard) { m_bisBillBoard = isBillBoard; }
	void Set_ColorOffset(_vec4 Color) { m_vColorOffset = Color; }
	void Set_IsLoop(bool isLoop) { m_bisLoop = isLoop; }

	void Follow_PlayerHand(Engine::HIERARCHY_DESC* pHierarchyDesc= nullptr, Engine::CTransform* PlayerTransform= nullptr);
	void Set_FollowHand(bool isFollow) { m_bisFollowHand = isFollow; }
private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable();
	void			Update_SpriteFrame(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	wstring m_strTextag= L"";
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag	= L"";
	_uint	m_uiTexIdx			= 0;

	FRAME	m_tFrame			{ };

	_float m_fDeltatime = 0.f;
	_float m_fAlpha = 1.f;
	bool m_bisBillBoard = true;
	_vec4 m_vColorOffset;
	bool m_bisLoop = true;
//
	bool m_bisFollowHand = false;
	Engine::HIERARCHY_DESC* m_pHierarchyDesc = nullptr;
	Engine::CTransform* m_pPlayerTransform = nullptr;
	bool m_bisFollowHandInit = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrTextureTag,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos,
									   const FRAME& tFrame = FRAME(1, 1, 0.0f));
private:
	virtual void Free();
};

