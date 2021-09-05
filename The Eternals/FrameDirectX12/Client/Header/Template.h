#pragma once

template<class T1>
struct INSTANCE_POOL_DESC
{
	T1**	ppInstances       = nullptr;
	_uint	uiInstanceSize    = 0;
	_uint	uiCurrentIdx      = 0;

};