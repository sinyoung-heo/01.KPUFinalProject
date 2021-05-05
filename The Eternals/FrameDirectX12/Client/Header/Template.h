#pragma once
namespace Engine
{
	class CGameObject;
}

template<class T1>
struct INSTANCE_POOL_DESC
{
	T1**	ppInstances       = nullptr;
	_uint	uiInstanceSize    = 0;
	_uint	uiCurrentIdx      = 0;

};

template<class T1>
Engine::CGameObject* Pop_Instance(INSTANCE_POOL_DESC<T1>* ppInstancePool)
{
	_uint uiIdx   = ppInstancePool->uiCurrentIdx;
	_uint uiSize  = ppInstancePool->uiInstanceSize;

	if (uiIdx == uiSize)
	{
		for (_uint i = 0; i < uiSize; ++i)
		{
			if (!ppInstancePool->ppInstances[i]->Get_IsUsingInstance())
			{
				ppInstancePool->uiCurrentIdx = i + 1;
				ppInstancePool->ppInstances[i]->AddRef();

				return ppInstancePool->ppInstances[i];
			}
		}
	}
	else
	{
		// CurrentIdx 사용중인지 검사.
		if (!ppInstancePool->ppInstances[uiIdx]->Get_IsUsingInstance())
		{
			++ppInstancePool->uiCurrentIdx;
			ppInstancePool->ppInstances[uiIdx]->AddRef();

			return ppInstancePool->ppInstances[uiIdx];
		}
		// 시용중이라면 사용중이지 않은 Indstnace 탐색.
		else
		{
			for (_uint i = uiIdx + 1; i < uiSize; ++i)
			{
				if (!ppInstancePool->ppInstances[i]->Get_IsUsingInstance())
				{
					ppInstancePool->uiCurrentIdx = i + 1;
					ppInstancePool->ppInstances[i]->AddRef();

					return ppInstancePool->ppInstances[i];
				}
			}

			for (_uint i = 0; i < uiIdx; ++i)
			{
				if (!ppInstancePool->ppInstances[i]->Get_IsUsingInstance())
				{
					ppInstancePool->uiCurrentIdx = i + 1;
					ppInstancePool->ppInstances[i]->AddRef();

					return ppInstancePool->ppInstances[i];
				}
			}
		}
	}

	return nullptr;
}

template<class T1>
void Return_Instance(INSTANCE_POOL_DESC<T1>* ppInstancePool, const _uint& uiInstanceIdx)
{
	ppInstancePool->ppInstances[uiInstanceIdx]->Set_IsUsingInstance(false);
	ppInstancePool->ppInstances[uiInstanceIdx]->Set_IsReturnObject(false);
	Engine::Safe_Release(ppInstancePool->ppInstances[uiInstanceIdx]);
}