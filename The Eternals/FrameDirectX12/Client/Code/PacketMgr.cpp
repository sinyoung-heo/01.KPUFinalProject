#include "stdafx.h"
#include "PacketMgr.h"

IMPLEMENT_SINGLETON(CPacketMgr)

CPacketMgr::CPacketMgr()
{

}

HRESULT CPacketMgr::Ready_Server()
{
	// Initialize Windows Socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Windows Socket
	g_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Non-Blocking Socket���� ��ȯ */
	unsigned long ul = 1;
	ioctlsocket(g_hSocket, FIONBIO, (unsigned long*)&ul);

	return S_OK;
}

HRESULT CPacketMgr::Connect_Server()
{
	/* Socket Address Structure */
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(SERVER_PORT);
	sockAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(g_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK)
		{
			error_display("connect : ", err_no);
			closesocket(g_hSocket);
			return E_FAIL;
		}
	}

#ifdef ERR_CHECK
	cout << "������ ������ ��û�Ͽ����ϴ�. ��ø� ��ٷ��ּ���." << endl;
#endif 

	CPacketMgr::Get_Instance()->send_login();

#ifdef ERR_CHECK
	cout << "Login packet ���ۿϷ�" << endl;
#endif 

	return S_OK;
}

void CPacketMgr::recv_packet()
{
	char net_buf[MAX_BUF_SIZE];

	// Server Data Receive.
	int retval = recv(g_hSocket, net_buf, MAX_BUF_SIZE, 0);

#ifdef ERR_CHECK
	if (retval == SOCKET_ERROR)
	{
		int state = WSAGetLastError();
		if (state != WSAEWOULDBLOCK)
		{
			error_display("recv Error : ", state);
			closesocket(g_hSocket);
		}
	}
#endif 

	// ������ ���� ����
	if (retval > 0)
	{
		// ��Ŷ ������
		ProcessData(net_buf, static_cast<size_t>(retval));
	}
}

void CPacketMgr::ProcessData(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;

	/* ó���ؾ� �� Original Packet Size */
	static size_t in_packet_size = 0;
	/* ó������ ���� Remain Packet Size */
	static size_t saved_packet_size = 0;
	/* recv packet�� ������ Buffer */
	static char packet_buffer[MAX_BUF_SIZE];

	/* ó���ؾ� �� ��Ŷ�� ���� ��� */
	while (0 != io_byte)
	{
		/* ó���ؾ� �� Packet Size ���� */
		if (0 == in_packet_size)
			in_packet_size = ptr[0];

		/* ��Ŷ�� ó���� �� ���� ��� -> ��Ŷ ó�� */
		if (io_byte + saved_packet_size >= in_packet_size)
		{
			/* 
			������ ó������ ���� ��Ŷ(saved_packet_size)�� ������ �� �����Ƿ�,
			Packet_buffer�� ���� �޸� ����(packet_buffer + saved_packet_size)��
			���� ���� ��Ŷ(ptr) ����
			*/
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);

			/* ��Ŷ ó�� */
			ProcessPacket(packet_buffer);

			/* ���� recv ���� �޸� ���� ���� */
			ptr += in_packet_size - saved_packet_size;
		
			/* ��Ŷ�� ó���ϰ� ���� recv size ���� */
			io_byte -= in_packet_size - saved_packet_size;

			/* Original Packet�� ó�������Ƿ� ���� ���� ��Ŷ�� ���� Original Packet Size �ʱ�ȭ */
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		/* ��Ŷ�� ó���� �� ���� ��� -> ��Ŷ ���� */
		else
		{
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void CPacketMgr::ProcessPacket(char* ptr)
{
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
		cout << "Login OK! ���ӿϷ�!" << endl;
	}
	break;

	case SC_PACKET_LOGIN_FAIL:
		break;
	case SC_PACKET_ENTER:
		break;
	case SC_PACKET_MOVE:
		break;
	case SC_PACKET_LEAVE:
		break;

	default:
#ifdef ERR_CHECK
		printf("Unknown PACKET type [%d]\n", ptr[1]);
#endif 
		break;
	}
}

void CPacketMgr::send_login()
{
	cs_packet_login p;

	p.size = sizeof(p);
	p.type = CS_LOGIN;

	int t_id = GetCurrentProcessId();
	sprintf_s(p.name, "P%03d", t_id % 1000);
	sprintf_s(p.password, "%03d", t_id % 1000);

	send_packet(&p);
}

void CPacketMgr::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);

	int iSendBytes = send(g_hSocket, p, p[0], 0);

#ifdef ERR_CHECK
	if (iSendBytes == SOCKET_ERROR)
	{
		error_display("send Error : ", WSAGetLastError());
		closesocket(g_hSocket);
	}
#endif 
	
}

void CPacketMgr::Free()
{
}
