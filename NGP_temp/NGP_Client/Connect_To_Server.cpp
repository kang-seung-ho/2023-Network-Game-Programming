#include "Common.h"
#include<iostream>
#include <stdio.h>
char* SERVERIP = (char*)"";
#define SERVERPORT 9000
#define BUFSIZE    4096

struct KEYINPUT {
	char Direction_keyDOWN;
	bool ATTACK;
};

int connect(KEYINPUT key)
{
	int retval{};
	char* IP林家{}; //
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, IP林家, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");


	retval = send(sock, (char*)&key, sizeof(KEYINPUT), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return 1;
	}

	closesocket(sock);

	WSACleanup();
	return 0;
}
