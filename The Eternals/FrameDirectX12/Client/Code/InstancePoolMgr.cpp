#include "stdafx.h"
#include "InstancePoolMgr.h"
#include "ObjectMgr.h"
#include "CollisionTick.h"

IMPLEMENT_SINGLETON(CInstancePoolMgr)

CInstancePoolMgr::CInstancePoolMgr()
	: m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
{

}

void CInstancePoolMgr::Ready_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	_uint uiInstanceSize = 0;

	// CollisionTick
	uiInstanceSize                       = 32;
	m_pCollisionTickPool                 = new INSTANCE_POOL_DESC<CCollisionTick>;
	m_pCollisionTickPool->ppInstances    = CCollisionTick::Create_Instance(pGraphicDevice, pCommandList, uiInstanceSize);
	m_pCollisionTickPool->uiInstanceSize = uiInstanceSize;
}

CCollisionTick* CInstancePoolMgr::Pop_CollisionTickInstance()
{
	_uint uiIdx   = m_pCollisionTickPool->uiCurrentIdx;
	_uint uiSize  = m_pCollisionTickPool->uiInstanceSize;

	if (uiIdx == uiSize)
	{
		for (_uint i = 0; i < uiSize; ++i)
		{
			if (!m_pCollisionTickPool->ppInstances[i]->Get_IsUsingInstance())
			{
				++m_pCollisionTickPool->uiCurrentIdx;
				m_pCollisionTickPool->ppInstances[i]->AddRef();

				return m_pCollisionTickPool->ppInstances[i];
			}
		}
	}
	else
	{
		// CurrentIdx ��������� �˻�.
		if (!m_pCollisionTickPool->ppInstances[uiIdx]->Get_IsUsingInstance())
		{
			++m_pCollisionTickPool->uiCurrentIdx;
			m_pCollisionTickPool->ppInstances[uiIdx]->AddRef();

			return m_pCollisionTickPool->ppInstances[uiIdx];
		}
		// �ÿ����̶�� ��������� ���� Indstnace Ž��.
		else
		{
			for (_uint i = uiIdx + 1; i < uiSize; ++i)
			{
				if (!m_pCollisionTickPool->ppInstances[i]->Get_IsUsingInstance())
				{
					m_pCollisionTickPool->uiCurrentIdx = i + 1;
					m_pCollisionTickPool->ppInstances[i]->AddRef();

					return m_pCollisionTickPool->ppInstances[i];
				}
			}

			for (_uint i = 0; i < uiIdx; ++i)
			{
				if (!m_pCollisionTickPool->ppInstances[i]->Get_IsUsingInstance())
				{
					m_pCollisionTickPool->uiCurrentIdx = i + 1;
					m_pCollisionTickPool->ppInstances[i]->AddRef();

					return m_pCollisionTickPool->ppInstances[i];
				}
			}
		}
	}

	return nullptr;
}

void CInstancePoolMgr::Return_CollisionTickInstance(const _uint& uiInstanceIdx)
{
	Engine::Safe_Release(m_pCollisionTickPool->ppInstances[uiInstanceIdx]);
	m_pCollisionTickPool->ppInstances[uiInstanceIdx]->Set_IsUsingInstance(false);
}

void CInstancePoolMgr::Free()
{
	for (_uint i = 0; i < m_pCollisionTickPool->uiInstanceSize; ++i)
		Engine::Safe_Release(m_pCollisionTickPool->ppInstances[i]);

	Engine::Safe_Delete(m_pCollisionTickPool);
}
