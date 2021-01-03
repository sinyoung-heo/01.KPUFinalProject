#pragma once
#include "Frame.h"

BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ ���� ���� Frame���� map�����̳ʷ� �����ϴ� MgrŬ���� ]
____________________________________________________________________________________________________________*/

class ENGINE_DLL CFrameMgr final : public CBase
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	virtual ~CFrameMgr() = default;

public:
	_bool	IsPermit_Call(wstring wstrFrameTag, const _float& fTimeDelta);
	HRESULT	Ready_Frame(wstring wstrFrameTag, const _float& fCallLimit);

private:
	CFrame*	Find_Frame(wstring wstrFrameTag);

private:
	map<wstring, CFrame*>	m_mapFrame;

public:
	virtual void Free();
};


END