#pragma once

/* SYSTEM HEADER */
//#include "vld.h"
#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <chrono>
#include <queue>
#include <atomic>
#include <sqlext.h>
#include <string>

/* NAMESPACE */
using namespace std;
using namespace chrono;

/* LINK TO LIBRARY */
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "odbc32.lib")

/* USER HEADER */
#include "define.h"
#include "dataType.h"
#include "protocol.h"
#include "enum.h"
#include "struct.h"
#include "extern.h"
#include "function.h"

#include "ObjPoolMgr.h"
#include "ObjMgr.h"
#include "PacketMgr.h"
#include "DBMgr.h"
#include "SectorMgr.h"