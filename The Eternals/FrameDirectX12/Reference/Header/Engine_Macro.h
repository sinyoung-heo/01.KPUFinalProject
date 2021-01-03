#ifndef Engine_Macro_h__
#define Engine_Macro_h__

namespace Engine
{
	#define	USING(NAMESPACE)	using namespace NAMESPACE;
	#define	BEGIN(NAMESPACE)	namespace NAMESPACE {
	#define	END										}

	#define	MSGERR_BOX(_message)			MessageBoxW(NULL, _message, L"System Message", MB_OK)
	#define	TAGMSGERR_BOX(_tag,_message)	MessageBoxW(NULL, TEXT(_message), _tag, MB_OK)

#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL _declspec(dllexport)
#else
	#define ENGINE_DLL _declspec(dllimport)
#endif // ENGINE_EXPORTS

	/*__________________________________________________________________________________________________________
	[ Singleton ]
	____________________________________________________________________________________________________________*/
	#define NO_COPY(CLASSNAME)								\
	private:												\
	CLASSNAME(const CLASSNAME&);							\
	CLASSNAME& operator = (const CLASSNAME&);	
	
	#define DECLARE_SINGLETON(CLASSNAME)					\
	NO_COPY(CLASSNAME)										\
	private:												\
		static CLASSNAME* m_pInstance;						\
	public:													\
		static CLASSNAME* Get_Instance(void);				\
	public:													\
		unsigned long Destroy_Instance(void);
	
	#define IMPLEMENT_SINGLETON(CLASSNAME)					\
	CLASSNAME* CLASSNAME::m_pInstance = nullptr;			\
	CLASSNAME* CLASSNAME::Get_Instance(void)				\
	{														\
		if(nullptr == m_pInstance)							\
		{													\
			m_pInstance = new CLASSNAME;					\
		}													\
		return m_pInstance;									\
	}														\
	unsigned long CLASSNAME::Destroy_Instance(void)			\
	{														\
		unsigned long dwRefCnt = 0;							\
		if (nullptr != m_pInstance)							\
		{													\
			dwRefCnt = m_pInstance->Release();				\
			if (0 == dwRefCnt)								\
				m_pInstance = nullptr;						\
		}													\
		return dwRefCnt;									\
	}

	/*____________________________________________________________________
	[ Random RGB ]
	______________________________________________________________________*/
	#define RANDOM_COLOR	_rgba(rand() / _float(RAND_MAX), rand() / _float(RAND_MAX), rand() / _float(RAND_MAX), 1.f)
	
	/*____________________________________________________________________
	[ Shader ConstBuffer ]
	______________________________________________________________________*/
	#define INIT_CB_256(input)	((sizeof(input) + 255) & ~255)
}


/*____________________________________________________________________
[ Matrix / Vector ]
______________________________________________________________________*/
#define INIT_VEC2(input)		_vec2(input, input)
#define INIT_VEC3(input)		_vec3(input, input, input)
#define INIT_VEC4(input)		_vec4(input, input, input, input)

#define INIT_MATRIX				XMMatrixIdentity()
#define MATRIX_INVERSE(input)	XMMatrixInverse(&XMMatrixDeterminant(input), input)


/*____________________________________________________________________
[ DirectInput ]
______________________________________________________________________*/
#define KEY_PRESSING(bykey)							CDirectInput::Get_Instance()->Key_Pressing(bykey)
#define KEY_UP(byKey)								CDirectInput::Get_Instance()->Key_Up(byKey)
#define KEY_DOWN(byKey)								CDirectInput::Get_Instance()->Key_Down(byKey)
#define KEY_COMBINE(byFirstKey, bySecondKey)		CDirectInput::Get_Instance()->Key_Combine(byFirstKey, bySecondKey)
#define KEY_COMBINE_UP(byFirstKey, bySecondKey)		CDirectInput::Get_Instance()->Key_CombineUp(byFirstKey, bySecondKey)
#define KEY_COMBINE_DOWN(byFirstKey, bySecondKey)	CDirectInput::Get_Instance()->Key_CombineDown(byFirstKey, bySecondKey)

#define MOUSE_PRESSING(eMouseKey)					CDirectInput::Get_Instance()->Mouse_KeyPressing(eMouseKey)
#define MOUSE_KEYUP(eMouseKey)						CDirectInput::Get_Instance()->Mouse_KeyUp(eMouseKey)
#define MOUSE_KEYDOWN(eMouseKey)					CDirectInput::Get_Instance()->Mouse_KeyDown(eMouseKey)

#define GetDIKeyState(byKeyID)						CDirectInput::Get_Instance()->Get_DIKeyState(byKeyID)
#define GetDIMouseState(eMouseID)					CDirectInput::Get_Instance()->Get_DIMouseState(eMouseID)
#define GetDIMouseMove(eMouseMoveState)				CDirectInput::Get_Instance()->Get_DIMouseMove(eMouseMoveState)

/*____________________________________________________________________
[ COUT DEBUG ]
______________________________________________________________________*/
#ifdef _DEBUG

#define ENGINE_LOG 0
#define COUT_STR(STR)	cout << STR << endl;

#endif













#endif // Engine_Macro_h__
