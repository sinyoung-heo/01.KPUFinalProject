// stdafx.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#ifdef _DEBUG

#include <vld.h>

#endif

#include "framework.h"
#include <cstring>

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
// C 런타임 헤더 파일입니다.

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

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

#include "Base.h"
#include "GraphicDevice.h"
#include "Include.h"

#include "ToolFileInfo.h"


#ifdef _DEBUG

#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 
#endif

#endif

#endif //PCH_H
