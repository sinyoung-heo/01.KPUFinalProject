#pragma once
#include "Include.h"
#include "Base.h"

#define ERR_CHECK

class CPacketMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CPacketMgr)

private:
	explicit CPacketMgr();
	virtual ~CPacketMgr() = default;

public:
	HRESULT Ready_Server();
	HRESULT Connect_Server();

public:
	void send_login();

private:
	void send_packet(void* packet);

private:
	virtual void Free();
};

