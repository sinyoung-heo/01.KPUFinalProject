#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CFont;
}


typedef struct tagMainMenuState
{
	FRAME	tFrame;
	_vec3	vPos           = _vec3(0.0f);
	_vec3	vScale         = _vec3(1.0f);
	_vec3	vRectPosOffset = _vec3(0.0f);
	_vec3	vRectScale     = _vec3(1.0f);

} MAIN_MENU_STATE;

class CGameUIRoot : public Engine::CGameObject
{
protected:
	explicit CGameUIRoot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CGameUIRoot() = default;

public:
	vector<Engine::CGameObject*>&	Get_ChildUIList() { return m_vecUIChild; };
	RECT&							Get_Rect()		  { return m_tRect; }
	void							Set_IsRender(const _bool& IsRender) { m_bIsRender = IsRender; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrObjectTag,
									 wstring wstrDataFilePath,
									 const _vec3& vPos, 
									 const _vec3& vScale,
									 const _bool& bIsSpriteAnimation,
									 const _float& fFrameSpeed,
									 const _vec3& vRectOffset,
									 const _vec3& vRectScale,
									 const _long& iUIDepth,
									 const _bool& bIsCreateFont = false,
									 wstring wstrFontTag = L"Font_NetmarbleLight");
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

	void			Add_ChildUI(Engine::CGameObject* pChildUI) { if (nullptr != pChildUI) m_vecUIChild.emplace_back(pChildUI); }
protected:
	void			Set_ConstantTable();
	void			Update_SpriteFrame(const _float& fTimeDelta);
	void			Update_Rect();
private:
	virtual HRESULT Add_Component();
	HRESULT			Read_DataFromFilePath(wstring wstrDataFilePath);

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
	vector<Engine::CGameObject*> m_vecUIChild;

	wstring	m_wstrObjectTag			= L"";
	wstring	m_wstrTextureTag		= L"";

	ID3D12DescriptorHeap*	m_pTexDescriptorHeap	= nullptr;
	_uint					m_uiTexIdx				= 0;
	_bool					m_bIsSpriteAnimation	= false;
	FRAME					m_tFrame;

	RECT	m_tRect;
	_vec3	m_vRectOffset = _vec3(0.0f);

	_vec3	m_vConvert     = _vec3(0.0f);
	_vec3	m_vConvertRect = _vec3(0.0f);

	_bool	m_bIsRender = true;

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont           = nullptr;
	wstring			m_wstrText        = L"";
	_tchar			m_szText[MAX_STR] = L"";

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
									   const _long& iUIDepth = 1000,
									   const _bool& bIsCreateFont = false,
									   wstring wstrFontTag = L"Font_NetmarbleLight");
protected:
	virtual void Free();
};

