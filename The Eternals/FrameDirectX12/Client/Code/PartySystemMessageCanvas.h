#pragma once
#include "GameUIRoot.h"

class CPartySystemMessageCanvas : public CGameUIRoot
{
private:
	explicit CPartySystemMessageCanvas(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPartySystemMessageCanvas() = default;

public:
	const PARTY_SYSTEM_MESSAGE& Get_PartyMessagetState() { return m_ePartyMessageState; }
	void Set_PartyMessageState(const PARTY_SYSTEM_MESSAGE& eState);
	void Set_PartyMemberInfo(wstring wstrName, const char& chOtype, const _int& iServerNum);

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
	void KeyInput_Mouse(const _float& fTimeDelta);
	void SetUp_FontPosition(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsKeyPressing   = false;
	_float m_fLifeTime       = 0.0f;
	_float m_fUpdateLifeTime = 3.0f;

	PARTY_SYSTEM_MESSAGE	m_ePartyMessageState = PARTY_SYSTEM_MESSAGE::PARTY_MESSAGE_END;
	wstring					m_wstrName           = L"";
	wstring					m_wstrJob            = L"";
	_int					m_iServerNum         = 0;
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

