#pragma once
#include "GameUIChild.h"

class CPartySuggestResponseCanvas;

class CPartySuggestResponseChoice : public CGameUIChild
{
private:
	explicit CPartySuggestResponseChoice(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPartySuggestResponseChoice() = default;

public:
	const PARTY_REQUEST_STATE& Get_PartyRequestState() { return m_ePartyRequestState; }
	
	void Set_CanvasClass(CPartySuggestResponseCanvas* pCanvas) { m_pCanvas = pCanvas; }
	void Set_PartyRequestState(const PARTY_REQUEST_STATE& eState) { m_ePartyRequestState = eState; }

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
									 const _long& iUIDepth = 1000);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
private:
	void SetUp_FontPosition(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CPartySuggestResponseCanvas* m_pCanvas = nullptr;

	_bool m_bIsKeyPressing = false;
	PARTY_REQUEST_STATE m_ePartyRequestState = PARTY_REQUEST_STATE::PARTY_STATE_END;

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
									   const _long& iUIDepth = 1000);
private:
	virtual void Free();
};

