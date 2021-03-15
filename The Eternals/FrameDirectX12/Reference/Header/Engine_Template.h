#pragma once
#include "Engine_Typedef.h"
#include "Engine_Macro.h"

/*__________________________________________________________________________________________________________
[ inline 함수 ]

- 함수는 함수가 호출될 때마다 발생하는 일정량의 성능 오버헤드가 있다는 단점이 있다. 
- 이는 CPU가 다른 레지스터와 함께 실행 중인 현재 명령어의 주소를 저장해야 하므로(나중에 반환할 위치를 알 수 있도록)
모든 함수 매개 변수를 생성해야 한다. 
- 할당된 값을 사용하면 프로그램이 새 위치로 분기된다. 내부에서 작성된 코드(인스턴트 코드)가 훨씬 더 빠르다.

- 크거나 복잡한 태스크를 수행하는 함수의 경우 함수 호출의 오버헤드는 함수가 실행되는 데 걸리는 시간과 비교할 때 중요하지 않다. 
- 그러나 일반적으로 사용하는 작은 함수의 경우, 
함수 호출에 필요한 시간이 실제로 함수 코드를 실행하는 데 필요한 시간보다 훨씬 많은 경우가 있다. 
- 이로 인해 상당한 성능 저하가 발생할 수 있다.



- C++은 인라인 함수(inline function)라는 내부에서 작성된 코드의 속도와 함수의 장점을 결합하는 방법을 제공한다. 
- inline 키워드는 컴파일러에서 함수를 인라인 함수로 처리하도록 요청한다. 
- 컴파일러가 코드를 컴파일하면 모든 인라인 함수가 인-플레이스(in-place) 확장된다. 
- 즉, 함수 호출이 함수 자체의 내용 복사본으로 대체되어 함수 오버헤드가 제거된다! 

- 단점은 인라인 함수가 모든 함수 호출에 대해 적절한 위치에서 확장되므로 
인라인 함수가 길거나 인라인 함수를 여러 번 호출하는 경우 컴파일된 코드를 약간 더 크게 만들 수 있다는 것이다.

- 코드를 부풀릴 가능성이 있으므로 함수를 인라인화 하는 것은 내부 루프가 없는 짧은 함수에 가장 적합하다. 
- 또한 inline 키워드는 권장 사항일 뿐이다. 
- 컴파일러는 인라인에 대한 요청을 자유롭게 무시할 수 있다. (긴 함수를 인라인화 하려고 하면 무시할 가능성이 있다.)

- 마지막으로, 현대 컴파일러는 자동으로 함수를 인라인화 하는 데 매우 뛰어나다. 
- 함수를 인라인으로 표시하지 않더라도 컴파일러는 성능이 향상될 것으로 생각하는 함수를 인라인화 한다. 
- 따라서 대부분의 경우 inline 키워드를 사용할 필요가 없다.

- 인라인 함수를 알고 있어야 하지만 최신 컴파일러는 함수를 적절하게 인라인화 하므로 inline 키워드를 사용할 필요가 없다.
____________________________________________________________________________________________________________*/

namespace Engine
{
	/*__________________________________________________________________________________________________________
	[ delete pointer ]
	____________________________________________________________________________________________________________*/
	template <typename T>
	inline void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	inline void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	inline DWORD Safe_Release(T& pointer)
	{
		DWORD	dwRefCnt = 0;
		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();
			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}

	/*__________________________________________________________________________________________________________
	[ Check nullptr ]
	____________________________________________________________________________________________________________*/
	template<typename T>
	inline void NULL_CHECK(T& ptr)
	{
		if (nullptr == ptr)
		{
			__debugbreak();
			return;
		}
	}

	template<typename T1, typename T2>
	inline T2 NULL_CHECK_RETURN(T1& ptr, const T2& return_value)
	{
		if (nullptr == ptr)
		{
			__debugbreak();
			return return_value;
		}

		return S_OK;
	}

	/*__________________________________________________________________________________________________________
	[ Check HRESULT ]
	____________________________________________________________________________________________________________*/
	inline void FAILED_CHECK(const HRESULT& hr)
	{
		if (hr < 0)
		{
			MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);
			__debugbreak();

			return;
		}
	}

	template<typename T1>
	inline T1 FAILED_CHECK_RETURN(const HRESULT& hr, const T1& return_value)
	{
		if (hr < 0)
		{
			MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);
			__debugbreak();

			return return_value;
		}

		return S_OK;
	}


	/*____________________________________________________________________
	[ UploadBuffer ]
	______________________________________________________________________*/
	template<typename T>
	class CUploadBuffer
	{
	public:
		CUploadBuffer(ID3D12Device* pGraphicDevice)
			: m_pGraphicDevice(pGraphicDevice)
		{}
		~CUploadBuffer() { Free(); }

	public:
		ID3D12Resource*	Resource() const { return m_pUploadBuffer; }
		const _uint&	GetElementByteSize() { return m_uiElementByteSize; }
		const _uint&	GetElementCount() { return m_uiElementCount; }
		void			CopyData(_int iElementIndex, const T& data) { memcpy(&m_pMappedData[iElementIndex * m_uiElementByteSize], &data, sizeof(T)); }
	private:
		HRESULT			Ready_UploadBuffer(_uint uiElementCount = 1, _bool bIsConstantBuffer = true)
		{
			m_uiElementCount	= uiElementCount;
			m_bIsConstantBuffer = bIsConstantBuffer;
			m_uiElementByteSize = sizeof(T);

			/*__________________________________________________________________________________________________________
			상수 버퍼 원소의 크기는 반드시 256바이트의 배수이어야 한다.
			이는 하드웨어가 m * 256바이트 오프셋에서 시작하는 n * 256바이트 길이의 상수 자료만 볼 수 있기 때문이다.

			typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC
			{
				UINT64 OffsetInBytes;			// 256의 배수.
				UINT   SizeInBytes;				// 256의 배수.
			} D3D12_CONSTANT_BUFFER_VIEW_DESC;
			____________________________________________________________________________________________________________*/
			if (m_bIsConstantBuffer)
				m_uiElementByteSize = (sizeof(T) + 255) & ~255;

			FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(m_uiElementByteSize * m_uiElementCount),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&m_pUploadBuffer)), 
																		  E_FAIL);

			FAILED_CHECK_RETURN(m_pUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMappedData)), E_FAIL);

			/*__________________________________________________________________________________________________________
			자원을 다 사용하기 전에는 대응을 해제할 필요가 없다.
			그러나, 자원을 GPU가 사용하는 중에는 CPU에서 자원을 갱신하지 않아야 한다. (따라서, 반드시 동기화 기법을 사용해아 한다.)
			____________________________________________________________________________________________________________*/

			return S_OK;
		}

	public:
		static CUploadBuffer<T>* Create(ID3D12Device* pGraphicDevice, _uint uiElementCount = 1, _bool bIsConstantBuffer = true)
		{
			CUploadBuffer<T>* pInstance = new CUploadBuffer<T>(pGraphicDevice);

			if (FAILED(pInstance->Ready_UploadBuffer(uiElementCount, bIsConstantBuffer)))
				Safe_Delete(pInstance);

			return pInstance;
		}

	private:
		ID3D12Device*	m_pGraphicDevice	= nullptr;
		ID3D12Resource* m_pUploadBuffer		= nullptr;
		BYTE*			m_pMappedData		= nullptr;
											
		_uint			m_uiElementCount	= 0;
		_uint			m_uiElementByteSize = 0;
											
		_bool			m_bIsConstantBuffer = false;

	private:
		void Free()
		{
			if (m_pUploadBuffer != nullptr)
				m_pUploadBuffer->Unmap(0, nullptr);

			m_pMappedData = nullptr;
		}
	};

}