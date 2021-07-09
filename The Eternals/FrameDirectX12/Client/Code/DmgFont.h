#pragma once
#include "Include.h"
#include "GameObject.h"

constexpr _uint		DMG_SIZE          = 7;
constexpr _uint		DMG_MAX           = 9'999'999;
constexpr _float	FONT_SCALE_OFFSET = 0.2f;
constexpr _float	FONT_POS_OFFSET   = 0.2f;

typedef struct tagDmgTextureDesc
{
	wstring					wstrTextureTag     = L"";
	ID3D12DescriptorHeap*	pTexDescriptorHeap = nullptr;
	_uint					uiTexIdx           = 0;
	FRAME					tFrame;

} DMG_TEXTURE_DESC;

class CDmgFont : public Engine::CGameObject
{
private:
	explicit CDmgFont(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CDmgFont() = default;

public:
	void Set_DamageType(const DMG_TYPE& eType);
	void Set_DamageList(const _uint& uiDmg);
	void Set_RandomDir();
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3& vPos,
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
	Engine::CRcTex*						m_pBufferCom     = nullptr;
	Engine::CShaderTexture*				m_pShaderCom     = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_matrix						m_matWorld[DMG_SIZE];
	vector<DMG_TEXTURE_DESC>	m_vecDmgTextureInfo[DMG_TYPE::DMGTYPE_END];
	DMG_TYPE					m_eDmgType = DMG_TYPE::DMGTYPE_END;
	_uint						m_uiDamage = 0;
	_uint						m_uiDmgListSize = 0;
	vector<_uint>				m_vecDamage;
	_float						m_fAlpha   = 1.0f;
	_vec3						m_vRight = _vec3(0.0f);
	_float						m_fSpeed = 1.0;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   const _vec3& vScale,
									   const _uint& uiDmg,
									   const DMG_TYPE& eType);
	static CDmgFont** Create_InstancePool(ID3D12Device* pGraphicDevice,
										  ID3D12GraphicsCommandList* pCommandList,
										  const _uint& uiInstanceCnt);
protected:
	virtual void Free();

};

