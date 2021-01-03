#pragma once
#include "Frame.h"

BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ 여러 개의 Frame들을 map컨테이너로 관리하는 Mgr클래스 ]
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