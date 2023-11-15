#include "Common.h"
#include <iostream>
#include <stdio.h>
#include <thread>

#define SERVERPORT 9000
#define BUFSIZE    4096

HANDLE mutexx;

struct clientData {
	char ipaddr[INET_ADDRSTRLEN];
	int portNum;
	int percent;
};
clientData clientarr[10] = {};



DWORD WINAPI ClientFunction(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE];

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	for (int i = 0; i < sizeof(clientarr) / sizeof(clientData); ++i) {
		if (clientarr[i].portNum == 0) { //ºóÄ­¿¡ ³Ö°í ³¡³»±â À§ÇØ
			strcpy(clientarr[i].ipaddr, addr);
			clientarr[i].portNum = ntohs(clientaddr.sin_port);
			break;
		}
	}
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		closesocket(client_sock);
		return 1;
	}


	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return 1;
	}
	while (1) {
		retval = recv(client_sock, buf, BUFSIZE, MSG_WAITALL);

		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}
	}

	closesocket(client_sock);
	return 0;
}

int main(int argc, char* argv[])
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	HANDLE hThread;
	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

		hThread = CreateThread(NULL, 0, ClientFunction, (LPVOID)client_sock, 0, NULL);


		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}

	}

	closesocket(listen_sock);
	WSACleanup();
	CloseHandle(mutexx);
	return 0;
}