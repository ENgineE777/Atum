#pragma once

#ifdef PLATFORM_PC
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <stdio.h> 
#include <vector> 

/**
\ingroup gr_code_services_network
*/

class NetworkDelegate
{
public:
	virtual void OnDataRecieved(void* data, int size) {};
	virtual void OnClientConnected(int id) {};
};

class NetworkClient
{ 
	enum
	{
		CHUNKSIZE = 512,
		BUFFSIZE = 16384
	};

#ifdef PLATFORM_PC
	SOCKET tcpSocket = INVALID_SOCKET;
#endif
	char* send_buffer = nullptr;
	int  send_len = 0;
	int  send_buff_size = BUFFSIZE;

	char* recv_buffer = nullptr;
	int  recv_len = 0;
	int  recv_buff_size = BUFFSIZE;

public:

	inline void CheckCapacity(char* &buff, int &buff_size, int needed_size)
	{
		if (needed_size > send_buff_size)
		{
			while (needed_size > send_buff_size)
			{
				send_buff_size += BUFFSIZE;
			}

			realloc(send_buffer, send_buff_size);
		}
	}

	NetworkDelegate* delegedate = nullptr;

	NetworkClient();
	~NetworkClient();

	int  id = -1;
	bool Connect(const char* ip, int port);
#ifdef PLATFORM_PC
	void SetSocket(int id, SOCKET tcpSocket);
#endif
	void Send(void* data, int len);
	void Recive();
	void Update();
};

class NetworkServer
{
#ifdef PLATFORM_PC
	SOCKET listenSocket = INVALID_SOCKET;
#endif
	std::vector<NetworkClient*> clients;
	int client_id = 0;

public:

	NetworkDelegate* delegedate = nullptr;

	bool Start(const char* ip, int port);
	void Update();
	void Send2Client(int id, void* data, int len);
	void Send2All(void* data, int len);
};