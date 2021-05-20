#include "stdafx.h"
#include "NetworkModule.h"

HANDLE g_hiocp;

high_resolution_clock::time_point last_connect_time;

//array<CLIENT, MAX_CLIENTS>	g_clients;
array<int, MAX_CLIENTS>		client_map;

atomic_int					num_connections;
atomic_int					client_to_close;

atomic_int					active_clients;
int							global_delay;				// ms단위, 1000이 넘으면 클라이언트 증가 종료

vector <thread*>			worker_threads;
thread						test_thread;

float						point_cloud[MAX_TEST * 2];

void InitializeNetwork()
{
	/*for (auto& cl : g_clients) 
	{
		cl.connected = false;
		cl.id = INVALID_ID;
	}*/

	for (auto& cl : client_map) 
		cl = -1;

	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	//for (int i = 0; i < 6; ++i)
		//worker_threads.push_back(new std::thread{ Worker_Thread });

	//test_thread = thread{ Test_Thread };
}

void GetPointCloud(int* size, float** points)
{
}
