#pragma once
#include "GameUIRoot.h"

class CMiniMapCanvas : public CGameUIRoot
{
private:
	explicit CMiniMapCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMiniMapCanvas() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
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
	void			Render_MiniMapTarget(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CShaderTexture*	m_pShaderTargetCom = nullptr;

	_float fOffsetX = 0.5f;
	_float fOffsetY = 0.5f;

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

