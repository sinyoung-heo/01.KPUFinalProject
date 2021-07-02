#pragma once
#include "Include.h"
#include "Base.h"

class CCinemaMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CCinemaMgr)

private:
	explicit CCinemaMgr();
	virtual ~CCinemaMgr() = default;

public:

private:
	virtual void Free();

};

