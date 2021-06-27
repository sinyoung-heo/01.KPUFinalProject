// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

/*
ON: SERVER CONNECT MODE
OFF: SERVER DISCONNECT MODE
*/
#define SERVER

#ifdef _DEBUG

//#include "vld.h"

#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define IME_KOREAN_SAMPLE
// Windows 헤더 파일
#include <windows.h>

// Windows Socket 헤더 파일
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
// Server Protocol Header
#include "..\..\..\FrameServer\Server\Header\protocol.h"
#include "..\..\..\FrameServer\Server\Header\MonsterAnimation.h"
#include "..\..\..\FrameServer\Server\Header\NPCAnimation.h"
#include "..\..\..\FrameServer\Server\Header\PCWeaponType.h"
#include "..\..\..\FrameServer\Server\Header\item_Protocol.h"


// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <ctime>
#include <process.h>

using namespace std;

/*____________________________________________________________________
DirectX12 헤더
______________________________________________________________________*/
#include <wrl.h>
#include <shellapi.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

/*____________________________________________________________________
Manager Class
______________________________________________________________________*/
#include "ShadowLightMgr.h"
#include "PacketMgr.h"
#include "ServerMath.h"
#include "MouseCursorMgr.h"
#include "EffectMgr.h"
/*____________________________________________________________________
Window Console창.
______________________________________________________________________*/
#ifdef _DEBUG

#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#endif

#pragma warning(disable : 4251)

typedef struct tagItemInfo
{
	char	chItemType = NO_ITEM;
	char	chItemName = NO_ITEM;

	_vec3	vScale = _vec3(0.0f);
	_uint	uiItemIdx = 0;
	FRAME	tItemIconFrame;

} ITEM_INFO;