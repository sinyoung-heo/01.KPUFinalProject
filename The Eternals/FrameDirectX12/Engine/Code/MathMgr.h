#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMathMgr final : public CBase
{
	DECLARE_SINGLETON(CMathMgr)

private:
	explicit CMathMgr();
	virtual ~CMathMgr() = default;


private:
	virtual void Free();
};

END
