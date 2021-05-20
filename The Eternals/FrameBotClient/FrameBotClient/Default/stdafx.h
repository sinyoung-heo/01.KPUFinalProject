// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

/* SYSTEM HEADER */
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Draw Header
#include <Windows.h>
#include <math.h>			
#include <stdio.h>			
#include <stdarg.h>			
#include <gl\gl.h>			
#include <gl\glu.h>			
#include <atomic>
#include <memory>

// Network Header
#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <queue>
#include <array>

/* NAMESPACE */
using namespace std;
using namespace chrono;

/* LINK TO LIBRARY */
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "ws2_32.lib")

/* USER HEADER */
#include "define.h"
#include "enum.h"
#include "struct.h"
#include "extern.h"
#include "function.h"

#include "..\..\..\FrameServer\Server\Header\protocol.h"
#include "..\..\..\FrameServer\Server\Header\dataType.h"