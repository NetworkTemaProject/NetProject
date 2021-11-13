#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include "Player.h"
#include "Foothold.h"

#define SERVERPORT 9000
#define BUFSIZE 512

int custom_counter;
// vector<Foothold> Bottom;

struct PlayerMgr
{
	DWORD portnum;
	// CPlayer player;
};

clock_t serverInit_time;
clock_t serverPre_time;
clock_t serverDelta_time;

bool IsCollisionPandF;
bool IsCollisionP1andP2;

bool IsNeedUpdateLoc;

bool isPlayingGame;

int portnum;

bool Win;

PlayerMgr Players[3];


HANDLE hClientThread; //클라이언트와 데이터 통신을 위한 쓰레드 핸들 변수
HANDLE hFootholdEvent; //발판 동기화 작업을 위한 이벤트 핸들 변수


void ServerInit();
BOOL IsOkGameStart();
void RecTimer();
void UpdateTimer();
float timeInterpolation();
void CreateMainGameScene();
void CheckCollideFoothold();

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player);
bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b);

void CheckCollidePlayers();
void UpdatePlayerLocation();
void UpdateClientKeyInput();
void CheckGameOver();
void SetCilentData(DWORD portnum);
void FootHoldInit();
void PlayerInit();
bool IsReadytoPlay(bool isReady);
DWORD WINAPI ProcessClient(LPVOID arg);

vector<Foothold> Bottom;

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0) break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}

using namespace std;
int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	// listen()
	retval = listen(listen_sock, SOMAXCONN);

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len, fsize;
	int cnt = 0;

	HANDLE hClientThread;

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
			break;


		

		// closesocket()
		closesocket(client_sock);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void ServerInit()
{

}

BOOL IsOkGameStart()
{
	return 0;
}

void RecTimer()
{
}

void UpdateTimer()
{
}

float timeInterpolation()
{
	return 0;
}

void CreateMainGameScene()
{
}

void CheckCollideFoothold()
{
}

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player)
{
	return false;
}

bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b)
{
	return false;
}

void CheckCollidePlayers()
{
}

void UpdatePlayerLocation()
{
}

void UpdateClientKeyInput()
{
}

void CheckGameOver()
{
}

void SetCilentData(DWORD portnum)
{

}

void FootHoldInit()
{
	Bottom.clear();

	srand(time(0));
	float r, g, b;

	for (int k = 0; k < N; ++k)
	{
		r = rand() / MAX;
		g = rand() / MAX;
		b = rand() / MAX;
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				Bottom.push_back(Foothold(-2.0f + (foothold_sizex + 0.1f) * j, 2.0f - 5.0f * k, -2.0f + (foothold_sizez + 0.1f) * i
					, r, g, b));
			}	// -화면크기 + (발판사이즈 + 간격) 
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		Bottom[rand() % 25].Del = true;
		Bottom[rand() % 25 + 25].Del = true;
		Bottom[rand() % 25 + 50].Del = true;
		Bottom[rand() % 25 + 75].Del = true;
		Bottom[rand() % 25 + 100].Del = true;
	}
	for (int i = Bottom.size() - 1; i >= 0; --i)
	{
		if (Bottom[i].Del)
		{
			Bottom.erase(Bottom.begin() + i);
		}
	}
}

void PlayerInit()
{
}

DWORD __stdcall ProcessClient(LPVOID arg)
{
	return 0;
}

bool IsReadytoPlay(bool isReady)
{

}