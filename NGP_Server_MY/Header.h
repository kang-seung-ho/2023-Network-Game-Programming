#pragma once
#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <array>

#include "protocol.h"

#define BUFSIZE 2048
#define NOBJECTS 42


using std::array;
using std::unordered_map;