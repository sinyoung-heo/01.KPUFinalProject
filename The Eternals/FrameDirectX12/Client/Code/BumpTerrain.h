#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	//  class CTerrainTex;
	// class CTexture;
	// class CShaderBumpTerrain;
	// class CShaderShadow;
}

class CBumpTerrain : public Engine::CGameObject
{
private:
	explicit CBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CBumpTerrain(const CBumpTerrain& rhs);
	virtual ~CBumpTerrain() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	// SingleThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta);

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	// Engine::CTerrainTex*		m_pBufferCom  = nullptr;
	// Engine::CTexture*			m_pTextureCom = nullptr;
	// Engine::CShaderBumpTerrain* m_pShaderCom  = nullptr;
	// Engine::CShaderShadow*		m_pShadowCom  = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/


public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrTextureTag);
private:
	virtual void Free();

};

