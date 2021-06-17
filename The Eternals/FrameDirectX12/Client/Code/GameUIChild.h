#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CFont;
}

typedef struct tagUIChildState
{
	FRAME	tFrame;
	_vec3	vPos           = _vec3(0.0f);
	_vec3	vScale         = _vec3(1.0f);
	_vec3	vRectPosOffset = _vec3(0.0f);
	_vec3	vRectScale     = _vec3(1.0f);

} UI_CHILD_STATE;

class CGameUIChild : public Engine::CGameObject
{
protected:
	explicit CGameUIChild(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CGameUIChild() = default;

public:
	const FRAME&		Get_Frame()				{ return m_tFrame; }
	RECT&				Get_Rect()				{ return m_tRect; }
	Engine::CFont*		Get_Font()				{ return m_pFont; }
	Engine::CTransform* Get_TransformColor()	{ return m_pTransColor; }
	const _vec3&		Get_RectOffset()		{ return m_vRectOffset; }

	void Set_IsRenderUI(const _bool& bIsRender) { m_bIsRender = bIsRender; }
	void Set_IsActive(const _bool& bIsActive)	{ m_bIsActive = bIsActive; }
	void Reverse_IsActive()						{ m_bIsActive = !m_bIsActive; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrRootObjectTag,
									 wstring wstrObjectTag,							   
									 wstring wstrDataFilePath,
									 const _vec3& vPos,
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth = 1000,
									 const _bool& bIsCreateFont = false,
									 wstring wstrFontTag = L"Font_NetmarbleLight");
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
protected:
	HRESULT			Read_DataFromFilePath(wstring wstrDataFilePath);
	void			Update_SpriteFrame(const _float& fTimeDelta);
	void			Update_Rect();
private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable();

protected:
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
	wstring	m_wstrRootObjectTag		= L"";
	wstring	m_wstrObjectTag			= L"";
	wstring	m_wstrTextureTag		= L"";

	ID3D12DescriptorHeap*	m_pTexDescriptorHeap	= nullptr;
	_uint					m_uiTexIdx				= 0;
	_bool					m_bIsSpriteAnimation	= false;
	FRAME					m_tFrame;

	RECT		m_tRect;
	_vec3		m_vRectOffset;

	_matrix*	m_pmatRoot     = nullptr;
	_vec3		m_vConvert     = _vec3(0.0f);
	_vec3		m_vConvertRect = _vec3(0.0f);

	_bool		m_bIsRender	= true;
	_bool		m_bIsActive = true;


	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont           = nullptr;
	wstring			m_wstrText        = L"";
	_tchar			m_szText[MAX_STR] = L"";
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrRootObjectTag,
									   wstring wstrObjectTag,							   
									   wstring wstrDataFilePath,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _bool& bIsSpriteAnimation,
									   const _float& fFrameSpeed,
									   const _vec3& vRectOffset,
									   const _vec3& vRectScale,
									   const _long& iUIDepth = 1000,
									   const _bool& bIsCreateFont = false,
									   wstring wstrFontTag = L"Font_NetmarbleLight");
protected:
	virtual void Free();
};

