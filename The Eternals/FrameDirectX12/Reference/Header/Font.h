#pragma once
#include "Engine_Include.h"
#include "GameObject.h"

/*__________________________________________________________________________________________________________
[ �� ���� Font�� �����ϴ� Ŭ���� ]
____________________________________________________________________________________________________________*/

BEGIN(Engine)

class ENGINE_DLL CFont final : public CGameObject
{
private:
	explicit CFont(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CFont(const CFont& rhs);
	virtual ~CFont() = default;

public:
	wstring Get_FontText() { return m_wstrText; }

	// Set
	void			Set_Text(wstring wstrText)				{ m_wstrText = wstrText; }
	void			Set_Pos(const _vec2& vPos)				{ m_vPos = vPos; }
	void			Set_Color(const D2D1::ColorF& Color)	{ m_pTextBrush->SetColor(Color); }

	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObjectPrototype(wstring wstrFontName,
											  const _float& fSize,
											  const D2D1::ColorF& Color);
	virtual HRESULT	Ready_GameObject(wstring wstrText, 
									 const _vec2& vPos, 
									 const D2D1::ColorF& Color = D2D1::ColorF::White);



	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	HRESULT			Calc_RectOffset();

private:
	/*__________________________________________________________________________________________________________
	[ DirectX 11 Interface ]
	____________________________________________________________________________________________________________*/
	ID2D1DeviceContext2*	m_pD2D_Context	 = nullptr;
	IDWriteFactory2*		m_pDWriteFactory = nullptr;

	/*__________________________________________________________________________________________________________
	[ Font Value ]
	____________________________________________________________________________________________________________*/
	ID2D1SolidColorBrush*	m_pTextBrush	= nullptr;
	IDWriteTextFormat*		m_pTextFormat	= nullptr;

	wstring					m_wstrText		= L"";
	wstring					m_wstrPreText	= L"";
	D2D1_RECT_F				m_TextRect;

	_vec2					m_vPos			= _vec2(0.0f, 0.0f);
	_float					m_fFontSize		= 0.0f;
	_vec2					m_vPosOffset	= _vec2(0.0f, 0.0f);

	_bool					m_bIsClone		= false ;

public:
	virtual CGameObject*	Clone_GameObject();
	static	CFont*			Create_Prototype(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList,
											 wstring wstrFontName,
											 const _float& fSize,
											 const D2D1::ColorF& Color);
private:
	virtual void			Free();
};

END