#include "header.h"
#include "player.h"
#include "obstacle.h"
#include "item.h"
#include "bullet.h"
#include "frametime.h"
#include "gameObject.h"
//#include "bullet.h"
//#include "item.h"

HANDLE			hSendEvent;
HANDLE			hCalculateEvent;

// 전역변수
int				thread_count = 0;	
bool			start_game = false;
const int		MAX_BULLET = 15;
unordered_map<int, Player> clients;
//array<bullet, MAX_BULLET> bullets;

DWORD WINAPI	clientThread(LPVOID arg);			
DWORD WINAPI	sendPacket(LPVOID arg);				

void err_display(const char* msg);
void err_quit(const char* msg);

//void gameStart();
void send_login_ok_packet(SOCKET* client_socket, char client_id);
void send_other_info_packet(SOCKET* client_socket, int client_id, int other_id);
void send_start_game_packet(SOCKET* client_socket, int client_id);
void send_move_packet(SOCKET* client_socket, int client_id);
void recv_move_packet(int client_id, char* p);

void send_Init_Pos(SOCKET* client_socket);

double frame_time = 0.0;
int b_id = 0;

CRITICAL_SECTION cs;

#define OBSTACLE_SIZE 30
std::vector<obstacle*> obstacles;
//std::vector<item*> items;
//std::unordered_map<int, item> items;
std::vector<bullet*> bullets;

std::unordered_map<int, item*> items; // item 포인터를 저장하는 unordered_map

item* CreateItem(int id) {
	bool collicheck{};
	while (true) {
		collicheck = false;
		item* newitem = new item;
		
		// 장애물과의 충돌 검사
		for (auto& obstacle : obstacles) {
			if (obstacle->CheckCollision(newitem)) {
				delete newitem;
				collicheck = true;
				break;
				//return nullptr;
			}
		}
		if (collicheck == 1)continue;

		// 기존 아이템과의 충돌 검사
		for (auto& pair : items) {
			if (pair.second->CheckCollision(newitem)) {
				delete newitem;
				collicheck = true;
				break;
				//return nullptr;
			}
		}
		if (collicheck == true)continue;

		//int newid = newitem->getItemID(); // item 객체에서 고유 ID 가져오기

		items[id] = newitem; // 고유 ID를 key로 사용하여 아이템 추가
		return newitem;
	}
}


DWORD WINAPI TimerThread(LPVOID arg) {
	int itemCNT{};
	double item_time = 0.0;
	double bullet_update_time = 0.0;
	while (true) {
		frame_time = GetFrameTime();
		bullet_update_time += frame_time;
		item_time += frame_time;
		if (item_time > CREATE_ITEM_TIME) {
			item* NEW = CreateItem(itemCNT);
			NEW->setItemID(itemCNT);
			itemCNT++;
			//패킷 보내기
			sc_item createdItem{};
			createdItem.size = sizeof(sc_item);
			createdItem.type = SC_P_ITEM;
			createdItem.x = NEW->GetPosX();
			createdItem.y = NEW->GetPosY();
			createdItem.item_type = NEW->getItemType();
			createdItem.id = NEW->getItemID();
			//std::cout << NEW->getItemID() << std::endl;
			//std::cout << createdItem.item_type << ' ' << createdItem.x << ' ' << createdItem.y << ' ' << std::endl;
			for (int x = 0; x < 3; ++x) {
				int retval = send(clients[x].c_socket, (char*)&createdItem, sizeof(sc_item), 0);
			}			
			item_time -= CREATE_ITEM_TIME;
		}
		if (bullet_update_time > 0.03) { // 총알 위치는 0.03초마다 업데이트(약 30프레임)
			EnterCriticalSection(&cs);
			for (auto it = bullets.begin(); it != bullets.end();) {
				(*it)->update();
				if ((*it)->GetDirX() + (*it)->GetDirY() == 0) {
					it = bullets.erase(it);
				}
				else
					++it;
			}
			for (auto& client : clients) { // 모든 플레이어에게 업데이트된 모든 총알 전송
				for (auto& bullet : bullets) {
					sc_bullet* packet = new sc_bullet;
					packet->size = sizeof(sc_bullet);
					packet->type = SC_P_BULLET;
					packet->id = bullet->GetID();
					packet->b_id = bullet->b_id;
					packet->x = bullet->GetPosX();
					packet->y = bullet->GetPosY();
					send(client.second.c_socket, (char*)packet, sizeof(sc_bullet), 0);
					std::cout << client.second.m_id << "에게 총알 정보 전송" << std::endl;
				}
			}
			LeaveCriticalSection(&cs);
			bullet_update_time -= 0.03;
		}
	}
}
std::vector<int> keysToDelete; //없앨 아이템 id를 클라에 다 보내준 뒤 vector안의 내용을 지운다.
bool CheckPlayerItemCollision(Player& player) {

	for (const auto& pair : items) {
		if (player.CheckCollision(pair.second)) {
			keysToDelete.push_back(pair.first);
		}
	}

	for (int key : keysToDelete) {
		items.erase(key); // 키를 사용하여 아이템 삭제
		std::cout << key;
	}

	return !keysToDelete.empty(); // 충돌이 하나라도 있었다면 true 반환
}


void CreateObstacles()
{
	// 좌측 상단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(300 + i * OBSTACLE_SIZE, 300);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(300, 300 + i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// 우측 상단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(900 - i * OBSTACLE_SIZE, 300);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(900, 300 + i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// 좌측 하단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(300 + i * OBSTACLE_SIZE, 900);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(300, 900 - i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// 우측 하단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(900 - i * OBSTACLE_SIZE, 900);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(900, 900 - i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

}

int main(int argc, char* argv[])
{
	InitializeCriticalSection(&cs);
	CreateObstacles();
	int retval;		// 오류 검출 변수

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	int addrlen;

	// 이벤트 사용 준비

	//hSendEvent = CreateEvent(NULL, FALSE, TRUE, NULL);		// 콘솔창에 쓰는것은 비신호로 시작
	hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset Event
	if (hSendEvent == NULL) return 1;
	hCalculateEvent = CreateEvent(NULL, FALSE, TRUE, NULL);		// 콘솔창에 쓰는것은 비신호로 시작
	if (hCalculateEvent == NULL) return 1;
	
	HANDLE hThread;
	while (thread_count < 3) {
		// accept()
		addrlen = sizeof(client_addr);
		client_sock = accept(sock, (SOCKADDR*)&client_addr, &addrlen);
		if (client_sock == INVALID_SOCKET) { err_display("ACCEPT()"); break; }
		thread_count += 1;
		Player* player = new Player(client_sock, thread_count);
		clients.insert(std::make_pair(thread_count, *player));

		// Thread 생성후 소켓 전달
		hThread = CreateThread(NULL, 0, clientThread, (LPVOID)player, 0, NULL);
		if (hThread == NULL) { closesocket(player->c_socket); }
		else { CloseHandle(hThread); }
		//thread_count++;
	}

	//// 3명이 접속 완료
	HANDLE hSend;
	hThread = CreateThread(NULL, 0, sendPacket, NULL, 0, NULL);
	if (hThread == NULL) { std::cout << "쓰레드 생성 에러" << std::endl; }
	HANDLE ITEM = CreateThread(NULL, 0, TimerThread, NULL, 0, NULL);
	if (hThread == NULL) { std::cout << "쓰레드 생성 에러" << std::endl; }

	//SetEvent(hSendEvent);
		//반복문, 클라이언트가 모두 죽었는가?? 한명 남았는가ㅣ?
		//반복문인 트루면

	//Send 모두 플레이어에게 등수 보내주기

	while (thread_count > 1) {
		
	}
	
	DeleteCriticalSection(&cs);
	closesocket(sock);
	WSACleanup();
	return 0;
}


DWORD WINAPI clientThread(LPVOID arg)
{

	int retval; // 오류 검출 변수

	// 전달된 소켓 저장
	Player* player = (Player*)arg;
	SOCKET& client_sock = player->c_socket;
	char* buf;
	int len;
	char id = (char)thread_count;
	player->m_id = id;
	std::cout << player->m_id << std::endl;
	buf = player->m_buf;
	len = BUFSIZE;
	
	send_login_ok_packet(&client_sock, id);//id부여

	// 클라이언트와 데이터 통신
	while (true) {
		retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			std::cout << id << "강제 연결 끊김" << std::endl;
			//for (auto& cl : clients) {
			//	if (cl.second.m_id == id) continue;
			//	//send_dead_packet(&cl.second.c_socket, id);
			//}
		}
		recv_move_packet(id, buf);
		SetEvent(hSendEvent);
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (clients[id].act == false) break;
	}

	while (1) {		// 죽으면 보내주는것을 받기는 하지만 처리는 해주지 말자
		retval = recv(client_sock, buf, len, 0);
	}
	closesocket(client_sock);
	return 0;
}

DWORD WINAPI sendPacket(LPVOID arg) {
	for (auto& client : clients) {
		send_Init_Pos(&client.second.c_socket);
		std::cout << client.second.m_id << "에게 초기 정보 전송" << std::endl;
	}

	while (true) {
		WaitForSingleObject(hSendEvent, INFINITE);
		for (auto& client : clients) {
			for (int i = 1; i <= 3; ++i) {
				send_move_packet(&client.second.c_socket, i);
			}
				
			std::cout << client.second.m_id << "에게 정보 전송" << std::endl;
			// bullet
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}
}

//void gameStart()
//{
//	std::cout << "게임시작" << std::endl;
//	//이때 시간을 초기화 하여 보낸다.
//
//
//	for (auto& cl : clients) {
//		send_start_game_packet(&cl.second.c_socket, cl.second.m_id);
//		cl.second.update(0.001f);
//	}
//	start_game = true;
//}

void send_Init_Pos(SOCKET* client_socket)
{
	sc_InitPlayer packet;
	packet.size = sizeof(packet);
	packet.type = SC_P_INIT;

	clients[1].SetPosY(100);
	clients[1].SetPosX(100);
	/*clients[1].pos_y = 100;
	clients[1].pos_x = 100;*/
	clients[1].m_id = 1;

	packet.id = 1;
	packet.y = 100;
	packet.x = 100;
	packet.color = RGB(255, 0, 0);
	send(*client_socket, (char*)(&packet), sizeof(packet), 0);
	std::cout << clients[1].m_id << "의 초기 정보 전송" << std::endl;


	clients[2].SetPosY(1100);
	clients[2].SetPosX(100);
	/*clients[2].pos_y = 1100;
	clients[2].pos_x = 100;*/
	clients[2].m_id = 2;
	
	packet.id = 2;
	packet.y = 1100;
	packet.x = 100;
	packet.color = RGB(0, 255, 0);
	send(*client_socket, (char*)(&packet), sizeof(packet), 0);
	std::cout << clients[2].m_id << "의 초기 정보 전송" << std::endl;

	clients[3].SetPosY(100);
	clients[3].SetPosX(1100);
	/*clients[3].pos_y = 100;
	clients[3].pos_x = 1100;*/
	clients[3].m_id = 3;

	packet.id = 3;
	packet.y = 100;
	packet.x = 1100;
	packet.color = RGB(0, 0, 255);
	send(*client_socket, (char*)(&packet), sizeof(packet), 0);
	std::cout << clients[3].m_id << "의 초기 정보 전송" << std::endl;
}


void send_login_ok_packet(SOCKET* client_socket, char client_id)
{
	sc_login packet;
	//std::cout << sizeof(packet);
	packet.size = sizeof(packet);
	packet.type = SC_P_LOGIN;
	packet.id = client_id;
	send(*client_socket, (char*)&packet, sizeof(packet), 0);
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
	packet.pos_x = clients[client_id].GetPosX();
	packet.pos_y = clients[client_id].GetPosY();
	packet.fdir_x = clients[client_id].fdir_x;
	packet.fdir_y = clients[client_id].fdir_y;
	packet.id = client_id;
	send(*client_socket, reinterpret_cast<const char*>(&packet), packet.size, 0);
}

void recv_move_packet(int client_id, char* p)
{
	unsigned char packet_type = p[1];
	Player& cl = clients[client_id];
	cs_move* packet = (cs_move*)(p);
	if (packet->type == CS_P_MOVE) {
		switch (packet->dir)
		{
		case VK_UP:
			cl.SetPosY(cl.GetPosY() - cl.speed);
			
			//cl.pos_y -= cl.speed;
			cl.fdir_x = 0;
			cl.fdir_y = -1;
			//std::cout << client_id << "위로 이동" << std::endl;
			//clients[client_id].SetPosY(cl.GetPosY());

			break;
		case VK_DOWN:
			cl.SetPosY(cl.GetPosY() + cl.speed);
			//cl.pos_y += cl.speed;
			cl.fdir_x = 0;
			cl.fdir_y = 1;
			//std::cout << client_id << "아래로 이동" << std::endl;
			//clients[client_id].SetPosY(cl.GetPosY());

			break;
		case VK_LEFT:
			cl.SetPosX(cl.GetPosX() - cl.speed);
			//cl.pos_x -= cl.speed;
			cl.fdir_x = -1;
			cl.fdir_y = 0;
			//std::cout << client_id << "왼쪽으로 이동" << std::endl;
			//clients[client_id].SetPosX(cl.GetPosY());

			break;
		case VK_RIGHT:
			cl.SetPosX(cl.GetPosX() + cl.speed);
			//cl.pos_x += cl.speed;
			cl.fdir_x = 1;
			cl.fdir_y = 0;
			//std::cout << client_id << "오른쪽으로 이동" << std::endl;

			break;
		default:
			std::cout << "잘못된값이 왔습니다 종료합니다 " << client_id << std::endl;

			exit(-1);
			cl.update(0.001f);

			break;

		}
		
			

		
		if (CheckPlayerItemCollision(cl)) {
			EnterCriticalSection(&cs);
			std::cout << keysToDelete[0] << ": 충돌 발생" << std::endl;
			//아이템과 플레이어 충돌체크
			sc_Itemhit nowHitItem{};
			nowHitItem.size = sizeof(sc_Itemhit);
			nowHitItem.type = SC_P_ITEM_HIT;
			if (!keysToDelete.empty()) {
				nowHitItem.ItemID = keysToDelete[0];
			}

			//모든 클라한테 업데이트
			for (auto& c : clients) {
				int ret = send(c.second.c_socket, (char*)(&nowHitItem), sizeof(sc_Itemhit), 0);
			}
			keysToDelete.clear(); //충돌한 아이템 리스트 삭제
			LeaveCriticalSection(&cs);
		}
		
		
	}	
	else if (packet_type == CS_P_ATTACK) {
		EnterCriticalSection(&cs);
		for (auto& client : clients) {
			if (client_id == client.second.m_id && client.second.heat < 10) {
				bullets.emplace_back(new bullet(client.second.m_id, ++b_id, client.second.GetPosX(),
					client.second.GetPosY(), client.second.fdir_x, client.second.fdir_y));
				client.second.heat++;
				client.second.heat_count = 0.0;
			}
		}
		LeaveCriticalSection(&cs);
		std::cout << client_id << "총알 발사" << std::endl;
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