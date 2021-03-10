#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CBumpTerrainTex;
	class CTexture;
	class CShaderBumpTerrain;
}


class CBumpTerrain : public Engine::CGameObject
{
private:
	explicit CBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CBumpTerrain() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
	void			Set_ConstantTable();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CBumpTerrainTex*	m_pBufferCom  = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CShaderBumpTerrain*	m_pShaderCom  = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring					m_wstrTextureTag      = L"";
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap  = nullptr;
	ID3D12DescriptorHeap*	m_pTexShadowDepthHeap = nullptr;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrTextureTag);
private:
	virtual void Free();
};

