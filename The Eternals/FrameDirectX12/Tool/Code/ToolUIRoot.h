#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRcTex;
	class CShaderTexture;
	class CRcCol;
	class CShaderColor;
}

class CToolUIChild;

class CToolUIRoot final : public Engine::CGameObject
{
private:
	explicit CToolUIRoot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CToolUIRoot() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrObjectTag,
									 wstring wstrDataFilePath,
									 const _vec3& vPos, 
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();
	HRESULT			Read_DataFromFilePath(wstring wstrDataFilePath);
	void			Set_ConstantTable();
	void			Update_SpriteFrame(const _float& fTimeDelta);
	void			Update_Rect();

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CRcTex*			m_pBufferCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;
	
	Engine::CTransform*		m_pTransColor   = nullptr;
	Engine::CRcCol*			m_pBufferColor	= nullptr;
	Engine::CShaderColor*	m_pShaderColor	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring					m_wstrObjectTag			= L"";
	wstring					m_wstrDataFilePath		= L"";
	wstring					m_wstrTextureTag		= L"";
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap	= nullptr;
	_uint					m_uiTexIdx				= 0;
	_bool					m_bIsSpriteAnimation	= false;
	FRAME					m_tFrame;

	RECT					m_tRect;
	_vec3					m_vRectOffset;
	_bool					m_bIsRenderRect = false;

	vector<CToolUIChild*>	m_vecUIChild;

private:
	_vec3 m_vConvert     = _vec3(0.0f);
	_vec3 m_vConvertRect = _vec3(0.0f);

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrObjectTag,							   
									   wstring wstrDataFilePath,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _bool& bIsSpriteAnimation,
									   const _float& fFrameSpeed,
									   const _vec3& vRectOffset,
									   const _vec3& vRectScale,
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

