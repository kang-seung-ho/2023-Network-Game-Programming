#define CREATE_ITEM_TIME 3.0


const short SERVERPORT = 9000;

//client -> server
const char CS_P_MOVE = 1;
const char CS_P_ATTACK = 2;

//server -> client
const char SC_P_LOGIN = 1;
const char SC_P_OTHER_INFO = 2;
const char SC_P_GAME_START = 3;
const char SC_P_DEAD = 4;
const char SC_P_MOVE = 5;
const char SC_P_ITEM = 6;
const char SC_P_OBSTACLE = 7;
const char SC_P_BULLET = 8;
const char SC_P_HIT = 9;
const char SC_P_INIT = 10;
const char SC_P_ITEM_HIT = 11;
const char SC_P_itemApply = 12;

// client -> server
struct cs_move {
	char size;
	char type;
	char id;
	//char	direction;			// 0 : up, 1: down, 2:left, 3:right
	WPARAM dir;
};

struct sc_InitPlayer {
	char size;
	char type;
	char id;
	int x;
	int y;
	COLORREF color;
};

/*
struct cs_attack {
	char size;
	char type;
	float aim_x;
	float aim_y;
};
*/

// server->client
struct sc_login {
	char size;
	char type;
	char id;
};

struct sc_other_info {
	char size;
	char type;
	char id;
};

struct sc_game_start {
	char size;
	char type;
};

struct sc_move {
	char size;
	char type;
	char id;
	float pos_x, pos_y;
	int fdir_x, fdir_y;
};

struct sc_dead {
	char size;
	char type;
	char id;
};

struct sc_player {
	char size;
	char type;
	char id;
};

struct sc_item {
	char size;
	char type;
	char id;
	char item_type; 
	float x;  
	float y; 
};

struct sc_bullet {
	char size;
	char type;
	char id;
	int b_id;
	float x;
	float y;
};

struct sc_obstacle {
	char size;
	char type;
	int x;
	int y;
};

struct sc_hit {
	char size;
	char type;
	char id;
	char bullet_id;
};

struct sc_Itemhit {
	char size;
	char type;
	char ItemID;
};

struct sc_itemApply {
	char size;
	char type;
	char clientID;
	char itemType;
	char coin;
	char hp;
	char heat;
};