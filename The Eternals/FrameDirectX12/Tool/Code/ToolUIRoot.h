#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CToolUIChild;

class CToolUIRoot final : public Engine::CGameObject
{
private:
	explicit CToolUIRoot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolUIRoot() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrDataFilePath,
									 const _vec3& vPos, 
									 const _vec3& vScale, 
									 const _long& iUIDepth);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap = nullptr;
	_uint					m_uiTexIdx	         = 0;
	FRAME					m_tFrameTexUV;

	vector<CToolUIChild*>	m_vecUIChild;

private:
	_vec3 m_vConvert	= _vec3(0.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrDataFilePath,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

