#include "stdafx.h"
#include "NetworkModule.h"

HANDLE g_hiocp;

high_resolution_clock::time_point last_connect_time;

array<CLIENT, MAX_CLIENTS>	g_clients;
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
	for (auto& cl : g_clients) 
	{
		cl.m_bIsConnect = false;
		cl.m_sNum = INVALID_ID;
	}

	for (auto& cl : client_map) 
		cl = -1;

	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 6; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });

	//test_thread = thread{ Test_Thread };
}

void Adjust_Number_Of_Client()
{
	static int delay_multiplier = 1;
	static int max_limit = MAXINT;
	static bool increasing = true;

	if (active_clients >= MAX_TEST) return;
	if (num_connections >= MAX_CLIENTS) return;

	auto duration = high_resolution_clock::now() - last_connect_time;
	if (ACCEPT_DELY * delay_multiplier > duration_cast<milliseconds>(duration).count()) return;

	int t_delay = global_delay;

	if (DELAY_LIMIT2 < t_delay)
	{
		if (true == increasing)
		{
			max_limit = active_clients;
			increasing = false;
		}

		if (100 > active_clients) 
			return;

		if (ACCEPT_DELY * 10 > duration_cast<milliseconds>(duration).count()) 
			return;

		last_connect_time = high_resolution_clock::now();
		DisconnectClient(client_to_close);
		client_to_close++;

		return;
	}
	else
	{
		if (DELAY_LIMIT < t_delay)
		{
			delay_multiplier = 10;
			return;
		}
	}	

	if (max_limit - (max_limit / 20) < active_clients) 
		return;

	increasing = true;
	last_connect_time = high_resolution_clock::now();
	g_clients[num_connections].client_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);


	int Result = WSAConnect(g_clients[num_connections].client_socket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) 
	{
		error_display("WSAConnect : ", GetLastError());
	}

	g_clients[num_connections].curr_packet_size = 0;
	g_clients[num_connections].prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].recv_over, sizeof(g_clients[num_connections].recv_over));
	g_clients[num_connections].recv_over.event_type = OP_RECV;
	g_clients[num_connections].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR*>(g_clients[num_connections].recv_over.IOCP_buf);
	g_clients[num_connections].recv_over.wsabuf.len = sizeof(g_clients[num_connections].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].client_socket), g_hiocp, num_connections, 0);

	cs_packet_login l_packet;

	// 변경
	int temp = num_connections;
	sprintf_s(l_packet.name, "%d", temp);
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_LOGIN;
	SendPacket(num_connections, &l_packet);


	int ret = WSARecv(g_clients[num_connections].client_socket, &g_clients[num_connections].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recv_over.over, NULL);
	if (SOCKET_ERROR == ret) 
	{
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
			goto fail_to_connect;
		}
	}
	num_connections++;
fail_to_connect:
	return;
}

void Test_Thread()
{
	while (true) 
	{
		//Sleep(max(20, global_delay));
		Adjust_Number_Of_Client();

		for (int i = 0; i < num_connections; ++i)
		{
			if (false == g_clients[i].m_bIsConnect) continue;
			if (g_clients[i].last_move_time + 1s > high_resolution_clock::now()) continue;

			g_clients[i].last_move_time = high_resolution_clock::now();

			cs_packet_move my_packet;
			my_packet.size = sizeof(my_packet);
			my_packet.type = CS_MOVE;

			/*switch (rand() % 4)
			{
			case 0: my_packet.dir = MV_FRONT; break;
			case 1: my_packet.dir = MV_BACK; break;
			case 2: my_packet.dir = MV_LEFT; break;
			case 3: my_packet.dir = MV_RIGHT; break;
			}*/
			my_packet.move_time = static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count());
			SendPacket(i, &my_packet);
		}
	}
}

void Worker_Thread()
{
	while (true) 
	{
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx* over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci, reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);

		int client_id = static_cast<int>(ci);
		if (FALSE == ret) 
		{
			int err_no = WSAGetLastError();

			if (64 == err_no) DisconnectClient(client_id);
			else 
			{
				DisconnectClient(client_id);
			}
			if (OP_SEND == over->event_type) delete over;
		}

		if (0 == io_size) 
		{
			DisconnectClient(client_id);
			continue;
		}

		if (OP_RECV == over->event_type) 
		{
			//std::cout << "RECV from Client :" << ci;
			//std::cout << "  IO_SIZE : " << io_size << std::endl;

			unsigned char* buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].curr_packet_size;
			unsigned pr_size = g_clients[ci].prev_packet_data;

			while (io_size > 0) 
			{
				if (0 == psize) 
					psize = buf[0];

				if (io_size + pr_size >= psize) 
				{
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}

				else 
				{
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}

			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;

			DWORD recv_flag = 0;

			int ret = WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);

			if (SOCKET_ERROR == ret) 
			{
				int err_no = WSAGetLastError();

				if (err_no != WSA_IO_PENDING)
				{
					error_display("RECV ERROR", err_no);
					DisconnectClient(client_id);
				}
			}
		}
		else if (OP_SEND == over->event_type) 
		{
			if (io_size != over->wsabuf.len) 
			{
				std::cout << "Send Incomplete Error!\n";
				DisconnectClient(client_id);
			}
			delete over;
		}

		else if (OP_DO_MOVE == over->event_type) 
		{
			// Not Implemented Yet
			delete over;
		}

		else 
		{
			std::cout << "Unknown GQCS event!\n";
			while (true);
		}
	}
}

void DisconnectClient(int ci)
{
	bool status = true;
	if (true == atomic_compare_exchange_strong(&g_clients[ci].m_bIsConnect, &status, false)) 
	{
		closesocket(g_clients[ci].client_socket);
		active_clients--;
	}
}

void SendPacket(int cl, void* packet)
{
	int psize = reinterpret_cast<unsigned char*>(packet)[0];
	int ptype = reinterpret_cast<unsigned char*>(packet)[1];

	OverlappedEx* over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR*>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0, &over->over, NULL);

	if (0 != ret) 
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
	// std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}

void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1])
	{
	case SC_PACKET_MOVE:
	{
		sc_packet_move* move_packet = reinterpret_cast<sc_packet_move*>(packet);
		if (move_packet->id < MAX_CLIENTS)
		{
			int my_id = client_map[move_packet->id];

			if (-1 != my_id)
			{
				g_clients[my_id].m_vPos.x = move_packet->posX;
				g_clients[my_id].m_vPos.z = move_packet->posZ;
			}
			if (ci == my_id)
			{
				if (0 != move_packet->move_time)
				{
					auto d_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - move_packet->move_time;

					if (global_delay < d_ms) global_delay++;
					else if (global_delay > d_ms) global_delay--;
				}
			}
		}
	}
	break;
	case SC_PACKET_ENTER: break;
	case SC_PACKET_LEAVE: break;
	case SC_PACKET_LOGIN_OK:
	{
		g_clients[ci].m_bIsConnect = true;

		active_clients++;

		sc_packet_login_ok* login_packet = reinterpret_cast<sc_packet_login_ok*>(packet);

		int my_id = ci;

		client_map[login_packet->id] = my_id;
		g_clients[my_id].m_sNum = login_packet->id;
		g_clients[my_id].m_vPos = _vec3(login_packet->posX, 0, login_packet->posZ);
	
		//cs_packet_teleport t_packet;
		//t_packet.size = sizeof(t_packet);
		//t_packet.type = CS_TELEPORT;
		//SendPacket(my_id, &t_packet);
	}
	break;
	case SC_PACKET_CHAT:
	{
	}
	break;
	case SC_PACKET_LOGIN_FAIL:
	{

		break;
	}
	case SC_PACKET_STAT_CHANGE:
	{

		break;
	}

	default: MessageBox(hWnd, L"Unknown Packet Type", L"ERROR", 0);
		while (true);
	}
}


void GetPointCloud(int* size, float** points)
{
	int index = 0;
	for (int i = 0; i < num_connections; ++i)
		if (true == g_clients[i].m_bIsConnect) 
		{
			point_cloud[index * 2] = static_cast<float>(g_clients[i].m_vPos.x);
			point_cloud[index * 2 + 1] = static_cast<float>(g_clients[i].m_vPos.z);
			index++;
		}

	*size = index;
	*points = point_cloud;
}
