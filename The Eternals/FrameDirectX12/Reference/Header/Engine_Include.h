#ifndef Engine_Include_h__
#define Engine_Include_h__

#ifdef _DEBUG

//#include "vld.h"

#endif

// #include <Windows.h>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <ctime>
#include <fstream>
#include <io.h>
#include <tchar.h>
#include <comdef.h>
#include <memory>
#include <chrono>

using namespace std;
using namespace chrono;

/*__________________________________________________________________________________________________________
[ DirectX 12 ]
____________________________________________________________________________________________________________*/
#include <wrl.h>
#include <shellapi.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dx12.h>
#include "DDSTextureLoader.h"

/*__________________________________________________________________________________________________________
[ DirectX 11 ]
____________________________________________________________________________________________________________*/
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dwrite_2.h>

/*__________________________________________________________________________________________________________
[ DirectInput ]
____________________________________________________________________________________________________________*/
#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

/*__________________________________________________________________________________________________________
[ DirectSound ]
____________________________________________________________________________________________________________*/
#include <mmsystem.h>
#include <dsound.h>
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


#include "Engine_Const.h"
#include "Engine_Enum.h"
#include "Engine_Functor.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Template.h"
#include "../../../FrameServer/Server/Header/protocol.h"

#pragma warning(disable : 4251)

/*__________________________________________________________________________________________________________
[ Assimp - Mesh Loader ]
____________________________________________________________________________________________________________*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#endif // Engine_Include_h__
