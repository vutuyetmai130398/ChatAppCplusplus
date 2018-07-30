#include<iostream>
#include<WS2tcpip.h>
#include<string>
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

void main() {

	//Initialize winsock
	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		cout << "Cant init winsock! Qitting" << endl;
		return;
	}

	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);	//Server listening

	if (listening == INVALID_SOCKET) {
		cout << "Cant create a socket!Qitting" << endl;
		return;
	}

	//Bind ip and port to server
	sockaddr_in hint;

	hint.sin_family = AF_INET;	//Can use TCP/ UDP
	hint.sin_port = htons(9999);	//Identity port	
	hint.sin_addr.S_un.S_addr = INADDR_ANY;	//IP Address

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];

			if (sock == listening) {
				//Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add the new connection to client list
				FD_SET(client, &master);

				//Send a welcome message
				string welcomeMsg = "Welcome to my room chat";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);

				if (bytesIn <= 0) {
					//Drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (sock != outSock && outSock!=listening) {
							send(outSock, buf, bytesIn, 0);
						}
					}
				}
			}
		}
	}

	////Wait for connection
	//sockaddr_in client;
	//int clientSize = sizeof(client);

	//SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	//char host[NI_MAXHOST];
	//char service[NI_MAXSERV];

	//ZeroMemory(host, NI_MAXHOST);
	//ZeroMemory(service, NI_MAXSERV);

	//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0)) {
	//	cout << host << "connected on port" << service << endl;
	//}
	//else
	//{
	//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	//	cout << host << "connected on port " << ntohs(client.sin_port) << endl;
	//}

	////Close listenning socket
	//closesocket(listening);

	////While loop accept and echo message back to clients
	//char buf[4096];

	//while (true)
	//{
	//	ZeroMemory(buf, 4096);

	//	//Wait client to send Data
	//	int bytesReceived = recv(clientSocket, buf, 4096, 0);

	//	if (bytesReceived == SOCKET_ERROR) {
	//		cout << "Error in rec(). Quitting \n";
	//		break;
	//	}
	//	else if(bytesReceived==0)
	//	{
	//		cout << "client disconnected \n";
	//		break;
	//	}

	//	cout << string(buf, 0, bytesReceived)<<endl;

	//	//Echo message 
	//	send(clientSocket, buf, bytesReceived + 1, 0);
	//}

	//Close socket
	//closesocket(clientSocket);

	//Shutdown WinSock
	WSACleanup();
	system("pause");
}