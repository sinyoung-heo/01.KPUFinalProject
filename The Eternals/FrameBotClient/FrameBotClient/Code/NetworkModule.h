#pragma once

/* READY NETWORK */
void InitializeNetwork();

/* CONNECT */
void Adjust_Number_Of_Client();
void Test_Thread();

/* PLAY */
void Worker_Thread();
void DisconnectClient(int ci);

/* PACKET SEND&RECV */
void ProcessPacket(int ci, unsigned char packet[]);
void SendPacket(int cl, void* packet);

/* DRAW */
void GetPointCloud(int* size, float** points);

