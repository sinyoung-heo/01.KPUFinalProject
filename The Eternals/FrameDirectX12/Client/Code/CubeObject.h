#pragma once
#include "Include.h"
#include "GameObject.h"


namespace Engine
{
	class CCubeCol;
	class CShaderColorInstancing;
}

class CCubeObject : public Engine::CGameObject
{
private:
	explicit CCubeObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCubeObject() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();
	void			Set_ConstantTable(const Engine::COLOR_BUFFER& eBuffer, const _int& iInstanceIdx);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CCubeCol*				m_pBufferCom			 = nullptr;
	Engine::CShaderColorInstancing* m_pShaderColorInstancing = nullptr;

	_uint m_uiColorPipelineStatePass = 0;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos);
private:
	virtual void Free();
};

