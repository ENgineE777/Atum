#pragma once

#ifdef PLATFORM_PC
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <stdio.h> 
#include <stdlib.h> 
#include <vector> 

/**
\ingroup gr_code_services_network
*/

/**
\brief NetworkDelegate

Delegate that handles network callbacks.

*/

class NetworkDelegate
{
public:

	/**
	\brief Callback on reciving data 

	\param[out] client_id ID of a clinet
	\param[out] data Pointer to a recived data
	\param[out] size Size in bytes of a recived data
	*/
	virtual void OnDataRecieved(int client_id, void* data, int size) {};

	/**
	\brief OnClientConnected

	\param id Id of a newly connected client
	*/
	virtual void OnClientConnected(int id) {};
};

/**
\ingroup gr_code_services_network
*/

/**
\brief NetworkClient

Client that conects to a server via TCP connection. IP address and a port of a server is needed
for a connection. For handling callbacks from network client a delegate should be set.

*/

class NetworkClient
{ 
#ifndef DOXYGEN_SKIP
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
#endif
public:

#ifndef DOXYGEN_SKIP
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
#endif

	/**
	\brief Connect to a server

	\param[in] ip Address of a server
	\param[in] port Port of a server

	\return True willl be returned on successfull conection to a server. Otherwise false will be returned.
	*/
	bool Connect(const char* ip, int port);
#ifdef PLATFORM_PC
	void SetSocket(int id, SOCKET tcpSocket);
#endif

	/**
	\brief Send data via socket

	\param[in] data Pointer to a data
	\param[in] len Size in bytes of a data
	*/
	void Send(void* data, int len);

#ifndef DOXYGEN_SKIP
	void Recive();
	void Update();
#endif
};

/**
\ingroup gr_code_services_network
*/

/**
\brief NetworkServer

Server which can accept connection of clients via TCP connection.
For handling callbacks from network client a delegate should be set.

*/

class NetworkServer
{
#ifndef DOXYGEN_SKIP
#ifdef PLATFORM_PC
	SOCKET listenSocket = INVALID_SOCKET;
#endif
	std::vector<NetworkClient*> clients;
	int client_id = 0;
#endif

public:

#ifndef DOXYGEN_SKIP
	NetworkDelegate* delegedate = nullptr;
	void Update();
#endif

	/**
	\brief Start a server

	\param port Port of binding of a server
	*/
	bool Start(int port);

	/**
	\brief Send data to a particular client

	\param[in] id Id of a client
	\param[in] data Pointer to a sending data
	\param[in] len Size in bytes of a senfing data
	*/
	void Send2Client(int id, void* data, int len);

	/**
	\brief Send data to a all connected clients

	\param[in] data Pointer to a sending data
	\param[in] len Size in bytes of a senfing data
	*/
	void Send2All(void* data, int len);
};
