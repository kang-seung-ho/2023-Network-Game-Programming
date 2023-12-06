#include "header.h"
#include "player.h"
//#include "bullet.h"
//#include "item.h"

HANDLE			hSendEvent;
HANDLE			hCalculateEvent;

// ��������
int				thread_count = 0;	
bool			start_game = false;
const int		MAX_BULLET = 15;
unordered_map<int, Player> clients;
//array<bullet, MAX_BULLET> bullets;

DWORD WINAPI	clientThread(LPVOID arg);			
DWORD WINAPI	sendPacket(LPVOID arg);				

void err_display(const char* msg);
void err_quit(const char* msg);

void gameStart();
void send_login_ok_packet(SOCKET* client_socket, char client_id);
void send_other_info_packet(SOCKET* client_socket, int client_id, int other_id);
void send_start_game_packet(SOCKET* client_socket, int client_id);
void send_move_packet(SOCKET* client_socket, int client_id);
void process_client(int client_id, char* p);

void send_Init_Pos(SOCKET* client_socket, char client_id);

int main(int argc, char* argv[])
{
	int retval;		// ���� ���� ����

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("SOCKET()");

	// bind()
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR) err_quit("BIND()");

	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("LISTEN()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	int addrlen;

	// �̺�Ʈ ��� �غ�

	hSendEvent = CreateEvent(NULL, FALSE, TRUE, NULL);		// �ܼ�â�� ���°��� ���ȣ�� ����
	if (hSendEvent == NULL) return 1;
	hCalculateEvent = CreateEvent(NULL, FALSE, TRUE, NULL);		// �ܼ�â�� ���°��� ���ȣ�� ����
	if (hCalculateEvent == NULL) return 1;
	
	HANDLE hThread;
	while (thread_count < 3) {
		// accept()
		addrlen = sizeof(client_addr);
		client_sock = accept(sock, (SOCKADDR*)&client_addr, &addrlen);
		if (client_sock == INVALID_SOCKET) { err_display("ACCEPT()"); break; }
		int id = thread_count;
		Player* player = new Player(client_sock, id);
		clients.insert(std::make_pair(thread_count, *player));

		// Thread ������ ���� ����
		hThread = CreateThread(NULL, 0, clientThread, (LPVOID)player, 0, NULL);
		if (hThread == NULL) { closesocket(player->c_socket); }
		else { CloseHandle(hThread); }
		thread_count++;
	}

	// 3���� ���� �Ϸ�
	HANDLE hSend;
	hThread = CreateThread(NULL, 0, sendPacket, NULL, 0, NULL);
	if (hThread == NULL) { std::cout << "������ ���� ����" << std::endl; }
	SetEvent(hCalculateEvent);
		//�ݺ���, Ŭ���̾�Ʈ�� ��� �׾��°�?? �Ѹ� ���Ҵ°���?
		//�ݺ����� Ʈ���

	//Send ��� �÷��̾�� ��� �����ֱ�

	while (thread_count > 1) {
		
	}
	
	closesocket(sock);
	WSACleanup();
	return 0;
}


DWORD WINAPI clientThread(LPVOID arg)
{

	int retval; // ���� ���� ����

	// ���޵� ���� ����
	Player* player = (Player*)arg;
	SOCKET& client_sock = player->c_socket;
	char* buf;
	int len;
	char id = (char)thread_count;
	player->m_id = id;
	std::cout << player->m_id << std::endl;
	buf = player->m_buf;
	len = BUFSIZE;

	send_login_ok_packet(&client_sock, id);//id�ο�
	send_Init_Pos(&client_sock, id);//�ʱ���ġ�ο�
	while (true) {
		if (thread_count == 3) {
			gameStart();
			break;
		}
	}
	
	for (auto& cl : clients) {
		if (cl.second.m_id == id) continue;
		send_other_info_packet(&cl.second.c_socket, cl.second.m_id, id);
		send_other_info_packet(&client_sock, id, cl.second.m_id);
	}

	while (start_game == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	};

	// Ŭ���̾�Ʈ�� ������ ���
	while (1) {
		retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << id << "���� ���� ����" << std::endl;
			for (auto& cl : clients) {
				if (cl.second.m_id == id) continue;
				//send_dead_packet(&cl.second.c_socket, id);
			}
		}
		process_client(id, buf);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (clients[id].act == false) break;
	}

	while (1) {		// ������ �����ִ°��� �ޱ�� ������ ó���� ������ ����
		retval = recv(client_sock, buf, len, 0);
	}
	closesocket(client_sock);
	return 0;
}

DWORD WINAPI sendPacket(LPVOID arg) {
	while (1) {
		WaitForSingleObject(hCalculateEvent, INFINITE);
		for (auto& pl : clients) {
			for (int i = 1; i <= 3; ++i)
				send_move_packet(&pl.second.c_socket, i);

			// bullet
		}
		SetEvent(hSendEvent);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}
}

void gameStart()
{
	std::cout << "���ӽ���" << std::endl;
	//�̶� �ð��� �ʱ�ȭ �Ͽ� ������.


	for (auto& cl : clients) {
		send_start_game_packet(&cl.second.c_socket, cl.second.m_id);
		cl.second.update(0.001f);
	}
	start_game = true;
}

void send_Init_Pos(SOCKET* client_socket, char client_id)
{
	sc_InitPos packet;
	packet.size = sizeof(packet);
	packet.type = SC_P_INIT;
	if (client_id == 1) {
		clients[1].pos_y = 100;
		clients[1].pos_x = 200;
		packet.y = 100;
		packet.x = 200;
	}
	else if (client_id == 2) {
		clients[2].pos_y = 600;
		clients[2].pos_x = 600;
		packet.y = 600;
		packet.x = 600;
	}
	else if (client_id == 3) {
		clients[3].pos_y = 100;
		clients[3].pos_x = 700;
		packet.y = 100;
		packet.x = 700;
	}
	std::cout << packet.y << ", " << packet.x << std::endl;
	send(*client_socket, (char*)(&packet), sizeof(packet), 0);
}


void send_login_ok_packet(SOCKET* client_socket, char client_id)
{
	sc_login packet;
	//std::cout << sizeof(packet);
	packet.size = sizeof(packet);
	packet.type = SC_P_LOGIN;
	packet.id = client_id;
	send(*client_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void send_other_info_packet(SOCKET* client_socket, int client_id, int other_id)
{
	sc_other_info packet;
	packet.size = sizeof(packet);
	packet.type = SC_P_OTHER_INFO;
	packet.id = other_id;
	send(*client_socket, reinterpret_cast<const char*>(&packet), sizeof(packet), 0);
}

void send_start_game_packet(SOCKET* client_socket, int client_id)
{
	sc_game_start packet;
	packet.size = sizeof(packet);
	packet.type = SC_P_GAME_START;
	send(*client_socket, reinterpret_cast<const char*>(&packet), sizeof(packet), 0);
}

void send_move_packet(SOCKET* client_socket, int client_id)
{
	sc_move packet;
	packet.size = sizeof(packet);
	packet.type = SC_P_MOVE;
	packet.pos_x = clients[client_id].pos_x;
	packet.pos_y = clients[client_id].pos_y;
	packet.id = client_id;
	send(*client_socket, reinterpret_cast<const char*>(&packet), packet.size, 0);
}

void process_client(int client_id, char* p)
{
	unsigned char packet_type = p[1];
	Player& cl = clients[client_id];
	switch (packet_type)
	{
	case CS_P_MOVE: {
		cs_move* packet = reinterpret_cast<cs_move*>(p);

		switch (packet->direction)
		{
		case 0:

			cl.dy -= cl.speed * cl.power;
			break;
		case 1:
			cl.dy += cl.speed * cl.power;

			break;
		case 2:
			cl.dx -= cl.speed * cl.power;

			break;
		case 3:
			cl.dx += cl.speed * cl.power;

			break;
		default:
			std::cout << "�߸��Ȱ��� �Խ��ϴ� �����մϴ� " << client_id << std::endl;

			exit(-1);

		}

		cl.update(0.001f);

		break;
	}
	}
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	std::cout << "[" << msg << "] " << (char*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}