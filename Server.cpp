#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include "Player.h"
#include "Foothold.h"

#define SERVERPORT 9000

static int custom_counter;
vector<Foothold> Bottom;

clock_t serverInit_time;
clock_t serverPre_time;
clock_t serverDelta_time;

struct SendGameData{
	PlayerMgr* PMgrs;
	clock_t ServerTime;
	vector<Foothold>& Bottom;
};

bool IsCollisionPandF;
bool IsCollisionP1andP2;

bool IsNeedUpdateLoc;

bool isPlayingGame;

int portnum;

bool Win;

PlayerMgr Players[CLIENT_NUM];
SendGameData* ServerGameData;

HANDLE hClientThread; //클라이언트와 데이터 통신을 위한 쓰레드 핸들 변수
HANDLE hFootholdEvent; //발판 동기화 작업을 위한 이벤트 핸들 변수
SOCKET client_sock;

void ServerInit();
BOOL IsOkGameStart(int PlayerCount);
void RecTimer();
void UpdateTimer();
float timeInterpolation();
void CreateMainGameScene();
void CheckCollideFoothold();

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player);
bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b);

void CheckCollidePlayers();
void UpdatePlayerLocation(CPlayer& p, InputData& input);
void UpdateFootholdbyPlayer(CPlayer& player);
void InitServerSendData();
void UpdateClientKeyInput();
void CheckGameOver();
void SetCilentData(DWORD portnum);
void FootHoldInit();
void PlayerInit();
bool IsReadytoPlay(bool isReady);
DWORD WINAPI ProcessClient(LPVOID arg);

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
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE hClientThread = {};

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		hClientThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);

		if (IsOkGameStart(++custom_counter))
		{
			ServerInit();
			break;
		}
		

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
	FootHoldInit();
	PlayerInit();
	InitServerSendData();
}

BOOL IsOkGameStart(int PlayerCount)
{
	if (PlayerCount == CLIENT_NUM)
		return TRUE;
	return FALSE;
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


void UpdateFootholdbyPlayer(CPlayer& player)
{
	player.fall = true;
	for (int i = 0; i < Bottom.size(); ++i) {
		if (IsCollideFootholdByPlayer(Bottom[i], player)) {
			player.fall = false;
			player.dy = 0;
			Bottom[i].startDel = true;
			break;
		}
	}
}

void CheckCollideFoothold()
{
	for (int i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].Del)
			Bottom.erase(Bottom.begin() + i);
	}

	for (int i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].startDel)
			Bottom[i].Delete();
	}

	for (int i = Bottom.size() - 1; i >= 0; --i) {
		if (Bottom[i].Del)
		{
			// 발판 삭제 후 점수 등 추가내용 반영
			// score += Bottom[i].score;
			//++cnt;
			Bottom.erase(Bottom.begin() + i);
		}
	}
}

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer& player)
{
	float b_maxX, b_minX, p_maxX, p_minX;
	float b_maxY, b_minY, p_maxY, p_minY;
	float b_maxZ, b_minZ, p_maxZ, p_minZ;
	p_maxX = player.x + 0.15; p_minX = player.x - 0.15;
	p_maxY = player.y + 0.1; p_minY = player.y - 0.1;
	p_maxZ = player.z + 0.15; p_minZ = player.z - 0.15;

	b_maxX = foot.mx + 0.4; b_minX = foot.mx - 0.4;
	b_maxY = foot.my + 0.35; b_minY = foot.my + 0.25;
	b_maxZ = foot.mz + 0.4; b_minZ = foot.mz - 0.4;

	if (b_maxX < p_minX || b_minX > p_maxX)
		return false;
	if (b_maxZ < p_minZ || b_minZ > p_maxZ)
		return false;
	if (b_maxY < p_minY || b_minY > p_maxY)
		return false;
	player.y = foot.my + 0.3;
	return true;
}

bool IsCollidePlayerByPlayer(CPlayer& a, CPlayer& b)
{
	return false;
}

void CheckCollidePlayers()
{
}

void UpdatePlayerLocation(CPlayer& p, InputData& input)
{
	if (input.bUp) p.dz = -0.1f;
	if (input.bDown) p.dz = 0.1f;
	if (input.bLeft) p.dx = -0.1f;
	if (input.bRight) p.dx = 0.1f;

	// 업데이트 중인지 판단 -> dx dz로 판단
	// 현재 키 입력 전부 안된상태 -> 0으로 초기화 (업데이트 중지)
	if (p.dz && !input.bUp && !input.bDown) p.dz = 0.0f;
	if (p.dx && !input.bLeft && !input.bRight) p.dx = 0.0f; 
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
	MakeFoothold(Bottom);
	DeleteRandomFoothold(Bottom);
}

void PlayerInit()
{
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		Players[i].player.x = 0;	// 좌표 값 어떻게 설정할 것인지(랜덤 or 지정) 나중에 상의!
		Players[i].player.y = 5;
		Players[i].player.z = 0;
		Players[i].player.dx = 0;
		Players[i].player.dy = 0;
		Players[i].player.dz = 0;
		Players[i].player.fall = true;
		Players[i].player.Locate();
	}
}

DWORD __stdcall ProcessClient(LPVOID arg)
{
	// 플레이어 구분 후 좌표업데이트
	
	//CPlayer* tPlayer;
	//for (int i = 0; i < CLIENT_NUM; ++i)
	//	if (Players[i].threadId == GetCurretnThreadId()) tPlayer == &player[i].player;
	// 포트넘버 or sock으로 수정필요, threadId론 클라 상에서 구분불가능

	while (1)
	{
		//UpdatePlayerLocation(*tPlayer,PlayerMgr.input);
		//UpdateFootholdbyPlayer(*tPlayer);
		CheckCollideFoothold();
		int nServerDataLen = sizeof(SendGameData);
		send(client_sock, (char*)&nServerDataLen, sizeof(int), 0);
		send(client_sock,(char*)&ServerGameData, nServerDataLen, 0);
	}
}

bool IsReadytoPlay(bool isReady)
{

}

void InitServerSendData()
{
	ServerGameData->PMgrs = Players;
	ServerGameData->Bottom = Bottom;
	//ServerGameData.ServerTime;
	//ServerGameData.Win;
}