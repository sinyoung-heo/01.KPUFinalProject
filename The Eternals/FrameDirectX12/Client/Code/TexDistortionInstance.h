#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CShaderTextureInstancing;
}

class CDynamicCamera;

class CTexDistortionInstance final : public Engine::CGameObject
{
private:
	explicit CTexDistortionInstance(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTexDistortionInstance(const CTexDistortionInstance& rhs);
	virtual ~CTexDistortionInstance() = default;

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

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable(const Engine::INSTANCE& eInstanceID, const _int& iInstanceIdx);
	void			Update_SpriteFrame(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*						m_pBufferCom               = nullptr;
	Engine::CShaderTextureInstancing*	m_pShaderTextureInstancing = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag	    = L"";
	_uint	m_uiTexIdx			    = 0;
	_int	m_iTexPipelineStatePass = 0;
	FRAME	m_tFrame				{ };

	CDynamicCamera* m_pDynamicCamara = nullptr;
	_vec3*			m_pParentPosition = nullptr;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrTextureTag,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos,
									   const FRAME& tFrame = FRAME(1, 1, 0.0f));
private:
	virtual void Free();
};

