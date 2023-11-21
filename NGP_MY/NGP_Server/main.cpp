#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>

#include "Common.h"
#include "Network.h"
#include "WaitingPlayerFunc.h"

#define SERVERPORT 9000
#define MAX_BUFFER_SIZE 52

std::list<Data*> MsgList{};
std::list<SOCKET> SocketList;

std::mutex cs;
std::mutex socketlist;   // cs�� ����Ʈ

void sendMSG(SOCKET sock, MSG data);
Data* recvMSG(SOCKET sock);

DWORD WINAPI ClientThread(LPVOID arg)
{
	std::string userName;
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	std::lock_guard<std::mutex> lock(socketlist);
	SocketList.push_back(client_sock);

	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	char buf[MAX_BUFFER_SIZE];
	Data* data;

	while (1) {
		retval = recv(client_sock, buf, sizeof(buf), 0);
		if (retval == SOCKET_ERROR)
			break;

		int msg;
		memcpy(&msg, buf, sizeof(msg));

		std::lock_guard<std::mutex> lock(cs);

		switch (msg)
		{
		case MSG_WAITING: // �÷��̾� �κ����(���� ���� ��) �޼���
			data = new WaitingPlayer;
			::ZeroMemory(data, sizeof(data));  // memory �ʱ�ȭ
			memcpy(((WaitingPlayer*)data), buf, sizeof(buf));   // ���� �����͸� ��ü�� ����
			MsgList.push_front((WaitingPlayer*)data);   // �޼����Լ� �� �տ� �߰�
			userName = std::string{ ((WaitingPlayer*)data)->getId() };  // �̸��� ������ ����
			break;

		case  MSG_GAME: // �÷��̾� �ΰ��� ���� �޼���
			data = new GamePlayer;
			::ZeroMemory(data, sizeof(data));  // memory �ʱ�ȭ
			memcpy(((GamePlayer*)data), buf, sizeof(buf));   // ���� �����͸� ��ü�� ����
			MsgList.push_front((GamePlayer*)data);   // �޼����Լ� �� �տ� �߰�
			break;

		case MSG_COLLIDE: // �浹 �޼���
			break;

		case MSG_CLEAR: // ���� Ŭ���� �޼���
			break;
		}
	}

	std::lock_guard<std::mutex> socklist_lock(socketlist);

	char* c_name = (char*)userName.c_str();
	data = new LeavePlayer{ c_name, MSG_LEAVE };  // ��Ż
	std::lock_guard<std::mutex> lock(cs);
	MsgList.push_front((LeavePlayer*)data);

	auto p = find(SocketList.begin(), SocketList.end(), client_sock);
	closesocket(*p);
	SocketList.remove(*p);

	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));
	std::cout << "Ŭ���̾�Ʈ ũ�� - " << SocketList.size() << std::endl;
	return 0;
}


DWORD WINAPI msgThread(LPVOID arg)
{
	bool b_connect = false;
	while (!b_connect) {
		std::lock_guard<std::mutex> lock(socketlist);
		if (SocketList.size() >= 3) {
			std::cout << " ��� ������ ���� " << std::endl;
			b_connect = true;  

			std::lock_guard<std::mutex> lock(cs);
			while (!MsgList.empty()) {
				Data* data = MsgList.front();  // �� �� ������

				// ���� ������ list�ȿ� �ִ� ��� Ŭ�󿡰� ����

				MsgList.pop_front();   // �� �� ������ ����
			}
		}
	}

	while (b_connect) {
		std::lock_guard<std::mutex> lock(cs);

		while (!MsgList.empty()) {
			std::lock_guard<std::mutex> socklist_lock(socketlist);
			if (SocketList.empty()) {
				MsgList.clear();
				break;
			}

			Data* data = MsgList.front();

			// ó���ؾ� �� �޼��� �ڵ�
			switch (data->getMsg()) {

			case MSG_WAITING: // �÷��̾� �κ����(���� ���� ��) �޼���
				for (auto p = SocketList.begin(); p != SocketList.end(); ++p) {
					std::cout << " SOCKET - " << *p << std::endl << std::endl;

					sendMSG(*p, MSG_WAITING);

					// ������ ������
					int retval = sendWaitingPlayer(*p, WaitingPlayer{ ((WaitingPlayer*)data)->getId(), ((WaitingPlayer*)data)->getState(), data->getMsg()});
					if (retval == -1) {
						err_display("SendPlayerInfoLobby");
						break;
					}
				}
				break;

			case  MSG_GAME: // �÷��̾� �ΰ��� ���� �޼���
				break;

			case MSG_COLLIDE: // �浹 �޼���
				break;

			case MSG_LEAVE: // �÷��̾� ��Ż �޼���
				break;

			case MSG_CLEAR: // ���� Ŭ���� �޼���
				break;
			}
			MsgList.pop_front();
		}
	}
}


int main(int argc, char* argv[])
{
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");


	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) 
		err_quit("bind()");


	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) 
		err_quit("listen()");

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	////////////////////////////////////////////

	std::mutex cs;
	std::mutex socketlist;

	HANDLE hThread;
	HANDLE hCacul_ExecuteThread;

	hCacul_ExecuteThread = CreateThread(NULL, 0, msgThread, NULL, 0, NULL);
	CloseHandle(hCacul_ExecuteThread);

	//////////////////////////////////////////

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);

		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}

	}
	closesocket(listen_sock);
	WSACleanup();
	return 0;
}

void sendMSG(SOCKET sock, MSG data)
{
	int retval;
	retval = send(sock, (char*)&data, sizeof(4), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return;
	}
}

Data* recvMSG(SOCKET sock)
{
	int retval;
	Data data;
	retval = recv(sock, (char*)&data, 52, 0);
	Data* rdata = new Data(data);
	return rdata;
}