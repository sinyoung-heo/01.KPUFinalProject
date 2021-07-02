#pragma once
#include "Include.h"
#include "GameObject.h"

constexpr _uint DMG_SIZE = 7;
constexpr _uint DMG_MAX  = 9'999'999;

typedef struct tagDmgTextureInfo
{
	wstring					wstrTextureTag     = L"";
	ID3D12DescriptorHeap*	pTexDescriptorHeap = nullptr;
	_uint					uiTexIdx           = 0;
	FRAME					tFrame;

} DMG_TEXTURE_INFO;

class CDmgFont : public Engine::CGameObject
{
private:
	explicit CDmgFont(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CDmgFont() = default;

public:
	void Set_DamageType(const DMG_TYPE& eType) { m_eDmgType = eType; }
	void Set_DamageList(const _uint& uiDmg);

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3& vPos,
									 const _vec3& vScale,
									 const _uint& uiDmg,
									 const DMG_TYPE& eType);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	void			Set_ConstantTable();
	virtual HRESULT Add_Component();
	HRESULT			Read_DataFromFilePath();
	void			SetUp_DmgFontWorldMatrix();
	void			Make_BillboardMatrix(_matrix& matrix, const _vec3& vScale);
protected:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	array<Engine::CRcTex*, DMG_SIZE>	m_arrBufferCom;
	Engine::CRcTex*						m_pBackBufferCom = nullptr;
	Engine::CShaderTexture*				m_pShaderCom     = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_matrix						m_matWorld[DMG_SIZE];
	vector<DMG_TEXTURE_INFO>	m_vecDmgTextureInfo[DMG_TYPE::DMGTYPE_END];
	DMG_TYPE					m_eDmgType = DMG_TYPE::DMGTYPE_END;
	_uint						m_uiDamage = 0;
	_uint						m_uiDmgListSize = 0;
	list<_uint>					m_lstDamage;
	_float						m_fAlpha   = 1.0f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3& vPos,
									   const _vec3& vScale,
									   const _uint& uiDmg,
									   const DMG_TYPE& eType);
protected:
	virtual void Free();

};

