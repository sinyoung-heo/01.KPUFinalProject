#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE		{ MODE_FULL, MODE_WIN, MODE_END };
	enum SWAPCHAIN		{ FRONT_BUFFER, BACK_BUFFER, BUFFER_END  };
	enum CMDID			{ CMD_MAIN, CMD_LOADING, CMD_END };
	enum MATRIXID		{ VIEW, PROJECTION, ORTHO, TOP_VIEW, MATRIX_END };
	enum INSTANCE		{ INSTANCE_ALPHA, INSTANCE_DISTORTION, INSTANCE_END };
	enum COLOR_BUFFER	{ BUFFER_RECT, BUFFER_CUBE, BUFFER_BOX, BUFFER_SPHERE, COLORBUFFER_END };

	/*__________________________________________________________________________________________________________
	2020.06.07 Thread Contex추가.
	____________________________________________________________________________________________________________*/
	enum CONTEXT	
	{ 
		CONTEXT0, 
		CONTEXT1, 
		CONTEXT2,
		CONTEXT3,
		CONTEXT_END 
	};

	enum THREADID			{ SHADOW, SCENE, THREADID_END };
	enum TEXTURE_MAPPING	{ TEX_DIFFUSE, TEX_NORMAL, TEX_SPECULAR, TEXTURE_END };
	enum RENDERID			{ MULTI_THREAD, SINGLE_THREAD, RENDERID_END };


	/*__________________________________________________________________________________________________________
	ID_DYNAMIC : 매 프레임마다 업데이트가 되어야 할 컴포넌트들의 집합
	ID_STATIC  : 매 프레임마다 업데이트가 필요 없는 컴포넌트들의 집합
	____________________________________________________________________________________________________________*/
	enum COMPONENTID	{ ID_DYNAMIC, ID_STATIC, ID_END };
	enum TEXTYPE		{ TEX_2D, TEX_CUBE, TEX_END };

	enum MOUSEBUTTON	{ DIM_LB, DIM_RB, DIM_WB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum SCENEID		
	{ 
		SCENE_LOGO, 
		SCENE_MENU,
		SCENE_STAGE, 
		SCENE_END 
	};

	enum STAGEID
	{
		STAGE_VELIKA,
		STAGE_BEACH,
		STAGE_WINTER,
		STAGE_END
	};

	/*__________________________________________________________________________________________________________
	[ Light Type ]
	____________________________________________________________________________________________________________*/
	typedef enum tagD3DLightType
	{
		D3DLIGHT_DIRECTIONAL,
		D3DLIGHT_POINT,
		D3DLIGHT_SPOT,
		D3DLIGHT_END,
		D3DLIGHT_FORCE_DWORD	= 0x7fffffff,

	} LIGHTTYPE;

}

// Sound
enum SOUNDID { SOUND_BGM, SOUND_PLAYER, SOUND_MONSTER, SOUND_EFFECT, SOUND_UI, SOUND_MOVIE, SOUND_ONCE, SOUND_REPEA, SOUND_END };


#endif // Engine_Enum_h__
