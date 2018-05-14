#include "Network.h"

#ifdef PLATFORM_PC
#pragma comment (lib, "Ws2_32.lib")
#endif

bool NetworkClient::Connect(const char* ip, int port)
{
#ifdef PLATFORM_PC
	struct addrinfo *result = nullptr, *ptr = nullptr;
	struct addrinfo hints;

	WSADATA wsaData;

	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (res != 0)
	{
		printf("WSAStartup failed with error: %d\n", res);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	char strPort[128];
	sprintf(strPort, "%i", port);

	res = getaddrinfo(ip, strPort, &hints, &result);

	if (res != 0)
	{
		printf("getaddrinfo failed with error: %d\n", res);
		return false;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		tcpSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (tcpSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			return false;
		}

		res = connect(tcpSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (res == SOCKET_ERROR)
		{
			closesocket(tcpSocket);
			tcpSocket = INVALID_SOCKET;
			printf("The server is down... did not connect");
		}
	}

	freeaddrinfo(result);

	if (tcpSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		return false;
	}
	u_long iMode = 1;

	res = ioctlsocket(tcpSocket, FIONBIO, &iMode);
	
	if (res == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(tcpSocket);
		return false;
	}

	char value = 1;
	setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
#endif

	return true;
}

#ifdef PLATFORM_PC
void NetworkClient::SetSocket(int id, SOCKET socket)
{
	this->tcpSocket = socket;
	this->id = id;
}
#endif

void NetworkClient::Send(void* data, int len)
{
	memcpy(&sendbuffer[send_len], data, len);
	send_len += len;
}

void NetworkClient::Recive()
{
#ifdef PLATFORM_PC
	char chunk[CHUNKSIZE];

	char* buffer = recvbuffer;

	while (true)
	{
		int rec = recv(tcpSocket, chunk, CHUNKSIZE, 0);

		if (rec == -1)
		{
			break;
		}

		if (rec == 0)
		{
			printf("Connection closed\n");
			closesocket(tcpSocket);
			break;
		}

		recv_len += rec;

		memcpy(buffer, chunk, rec);

		if (rec < CHUNKSIZE)
		{
			break;
		}
	}
#endif
}

void NetworkClient::Update()
{
	Recive();

	if (recv_len)
	{
		if (delegedate)
		{
			delegedate->OnDataRecieved(recvbuffer, recv_len);
		}

		recv_len = 0;
	}

#ifdef PLATFORM_PC
	if (send_len)
	{
		send(tcpSocket, sendbuffer, send_len, 0);
		send_len = 0;
	}
#endif
}

bool NetworkServer::Start(const char* ip, int port)
{
#ifdef PLATFORM_PC
	WSADATA wsaData;

	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (res != 0)
	{
		printf("WSAStartup failed with error: %d\n", res);
		return false;
	}

	struct addrinfo *result = nullptr;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	char strport[32];
	sprintf(strport, "%i", port);

	res = getaddrinfo(NULL, strport, &hints, &result);

	if (res != 0)
	{
		printf("getaddrinfo failed with error: %d\n", res);
		return false;
	}

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		return false;
	}

	u_long iMode = 1;
	res = ioctlsocket(listenSocket, FIONBIO, &iMode);

	if (res == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		return false;
	}

	res = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (res == SOCKET_ERROR)
	{
		int k = WSAGetLastError();
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		return false;
	}

	freeaddrinfo(result);

	res = listen(listenSocket, SOMAXCONN);

	if (res == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		return false;
	}
#endif

	return true;
}

void NetworkServer::Update()
{
#ifdef PLATFORM_PC
	SOCKET clientSocket = accept(listenSocket, NULL, NULL);

	if (clientSocket != INVALID_SOCKET)
	{
		char value = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		client_id++;

		NetworkClient* client = new NetworkClient();
		client->SetSocket(client_id, clientSocket);

		client->delegedate = delegedate;

		clients.push_back(client);

		if (delegedate)
		{
			delegedate->OnClientConnected(client_id);
		}
	}
#endif

	for (auto client : clients)
	{
		client->Update();
	}
}

void NetworkServer::Send2Client(int id, void* data, int len)
{
	for (auto client : clients)
	{
		if (client->id == id)
		{
			client->Send(data, len);
			break;
		}
	}
}

void NetworkServer::Send2All(void* data, int len)
{
	for (auto client : clients)
	{
		client->Send(data, len);
	}
}