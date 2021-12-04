#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <time.h>
#include "Player.h"
#include "Foothold.h"

#define SERVERPORT 9000
//#define SERVERIP "192.168.82.96"
#define SERVERIP "127.0.0.1"

volatile int custom_counter = 0;

clock_t serverInit_time;
clock_t serverPre_time;
clock_t serverDelta_time;

struct SendGameData{
	PlayerMgr PMgrs[CLIENT_NUM];
	clock_t ServerTime;
	Foothold Bottom[N * N * N];
};

bool IsCollisionPandF;
bool IsCollisionP1andP2;

bool IsNeedUpdateLoc;

bool isPlayingGame;

int portnum;

bool Win;

vector<Foothold> Bottom;
map<DWORD, PlayerMgr*> ClientManager;

SendGameData ServerGameData;

HANDLE hClientThread; //클라이언트와 데이터 통신을 위한 쓰레드 핸들 변수
HANDLE hFootholdEvent; //발판 동기화 작업을 위한 이벤트 핸들 변수
HANDLE hGameEvent, hTimeEvent;	// 소켓 동기화 작업을 위한 이벤트 핸들 변수

void ServerInit();
BOOL IsOkGameStart(int PlayerCount);
void RecTimer();
void UpdateTimer();
float timeInterpolation();
void CreateMainGameScene();
void CheckCollideFoothold(vector<Foothold>& Bottom);

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer* player);

void InitServerSendData();
void SetCilentData();
void FootHoldInit();
void PlayerInit();
DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI ProcessTime(LPVOID arg);

void UpdatePlayerLocation(CPlayer* p, InputData input);
void UpdateFootholdbyPlayer(CPlayer* player,vector<Foothold>& Bottom);
void SettingPlayersMine(DWORD ThreadId);
void CheckInsertPlayerMgrData(DWORD ThreadId);

bool IsGameOver(CPlayer* player);
void CheckGameWin(DWORD ThreadId);

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
	ServerInit();

	hGameEvent = CreateEvent(NULL, FALSE, TRUE, NULL);	// TRUE: 게임 정보 보내기 완료
	if (hGameEvent == NULL)
		return 1;
	hTimeEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // TRUE: 시간 정보 보내기 완료
	if (hTimeEvent == NULL)
		return 1;

	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;

	HANDLE hClientThread[2] = {};
	HANDLE hTimeThread = {};

	SOCKET client_socks[CLIENT_NUM] = {};

	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_socks[i] = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		//int opt_val = TRUE;
		//setsockopt(client_socks[i], IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(opt_val));
		if (client_socks[i] == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 신호 상태
		hFootholdEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		if (hFootholdEvent == NULL)
			return 1;
	}

	custom_counter = CLIENT_NUM;
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		hClientThread[i] = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_socks[i], 0, NULL);
		if (hClientThread[i] == NULL)
			closesocket(client_socks[i]);
	}

	hTimeThread = CreateThread(NULL, 0, ProcessTime, (LPVOID)client_socks, 0, NULL);

	WaitForMultipleObjects(2, hClientThread, TRUE, INFINITE);
	if (hTimeThread != NULL)
		WaitForSingleObject(hTimeThread, INFINITE);
	
	CloseHandle(hGameEvent);
	CloseHandle(hTimeEvent);

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

void UpdateFootholdbyPlayer(CPlayer* player, vector<Foothold>& Bottom)
{
	(*player).fall = true;
	for (size_t i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].Del) continue;

		if (IsCollideFootholdByPlayer(Bottom[i], player)) {
			(*player).fall = false;
			(*player).dy = 0;
			
			if(!Bottom[i].startDel)
				(*player).m_nScore += Bottom[i].score;
			
			Bottom[i].startDel = true;
			break;
		}
	}
}

void CheckCollideFoothold(vector<Foothold>& Bottom)
{
	for (size_t i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].Del)
			Bottom[i].startDel = false;
	}

	for (size_t i = 0; i < Bottom.size(); ++i) {
		if (Bottom[i].startDel)
		{
			Bottom[i].Delete();
		}
	}
}

bool IsCollideFootholdByPlayer(Foothold foot, CPlayer* player)
{
	float b_maxX, b_minX, p_maxX, p_minX;
	float b_maxY, b_minY, p_maxY, p_minY;
	float b_maxZ, b_minZ, p_maxZ, p_minZ;
	p_maxX = (*player).x + 0.15f; p_minX = (*player).x - 0.15f;
	p_maxY = (*player).y + 0.1f; p_minY = (*player).y - 0.1f;
	p_maxZ = (*player).z + 0.15f; p_minZ = (*player).z - 0.15f;

	b_maxX = foot.mx + 0.4f; b_minX = foot.mx - 0.4f;
	b_maxY = foot.my + 0.35f; b_minY = foot.my + 0.25f;
	b_maxZ = foot.mz + 0.4f; b_minZ = foot.mz - 0.4f;

	if (b_maxX < p_minX || b_minX > p_maxX)
		return false;
	if (b_maxZ < p_minZ || b_minZ > p_maxZ)
		return false;
	if (b_maxY < p_minY || b_minY > p_maxY)
		return false;
	(*player).y = foot.my + 0.3f;
	return true;
}

void UpdatePlayerLocation(CPlayer* p, InputData input)
{

	if ((*p).dz) (*p).dz = 0.0f;
	if ((*p).dx) (*p).dx = 0.0f;

	if (input.bUp) (*p).dz = -0.1f;
	if (input.bDown) (*p).dz = 0.1f;
	if (input.bLeft) (*p).dx = -0.1f;
	if (input.bRight) (*p).dx = 0.1f;
	if (input.bSpace) (*p).Jump();

	// 업데이트 중인지 판단 -> dx dz로 판단
	// 현재 키 입력 전부 안된상태 -> 0으로 초기화 (업데이트 중지)
	if ((*p).dz && !input.bUp && !input.bDown) (*p).dz = 0.0f;
	if ((*p).dx && !input.bLeft && !input.bRight) (*p).dx = 0.0f;
}

void FootHoldInit()
{
	Bottom.clear();
	MakeFoothold(Bottom);
	//DeleteRandomFoothold(Bottom);
}

void PlayerInit()
{
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		ServerGameData.PMgrs[i].player.x = 0;	// 좌표 값 어떻게 설정할 것인지(랜덤 or 지정) 나중에 상의!
		ServerGameData.PMgrs[i].player.y = 5;
		ServerGameData.PMgrs[i].player.z = 0;
		ServerGameData.PMgrs[i].player.dx = 0;
		ServerGameData.PMgrs[i].player.dy = 0;
		ServerGameData.PMgrs[i].player.dz = 0;
		ServerGameData.PMgrs[i].player.fall = true;
		ServerGameData.PMgrs[i].player.Locate();
	}
}

DWORD __stdcall ProcessClient(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg;
	SOCKADDR_IN clientAddr = {};
	int addrlen = 0;

	addrlen = sizeof(clientAddr);
	getpeername(clientSock, (SOCKADDR*)&clientAddr, &addrlen);

	send(clientSock, (char*)&custom_counter, sizeof(int), 0);

	SendPlayerData ClientData;
	//int nClientDataLen = 0;
	int nServerDataLen = sizeof(SendGameData);
	int nClientDataLen = sizeof(SendPlayerData);
	short opcode = 0;

	while (1)
	{
		Sleep(40);
		WaitForSingleObject(hTimeEvent, INFINITE);

		DWORD retval = WaitForSingleObject(hFootholdEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) 
			break;

		DWORD threadId = GetCurrentThreadId();
		CheckInsertPlayerMgrData(threadId);

		send(clientSock, (char*)&opcode, sizeof(short), 0);

		//recvn(clientSock, (char*)&nClientDataLen, sizeof(int), 0);       
		retval = recvn(clientSock, (char*)&ClientData, nClientDataLen, 0);
		if (retval == SOCKET_ERROR) 
			err_display("");

		SettingPlayersMine(threadId);
		UpdatePlayerLocation(&(*ClientManager[threadId]).player, ClientData.Input);
		(*ClientManager[threadId]).player.Update();
		UpdateFootholdbyPlayer(&(*ClientManager[threadId]).player,Bottom);
		CheckCollideFoothold(Bottom);

		(*ClientManager[threadId]).bGameOver = IsGameOver(&(*ClientManager[threadId]).player);
		CheckGameWin(threadId);

		SetCilentData();

		//send(clientSock, (char*)&nServerDataLen, sizeof(int), 0);
		retval = send(clientSock, (char*)&ServerGameData, nServerDataLen, 0);
		if (retval == SOCKET_ERROR) 
			err_display("");

		SetEvent(hFootholdEvent);
		SetEvent(hGameEvent);
	}
	return 0;
}

DWORD WINAPI ProcessTime(LPVOID arg)
{
	SOCKET* socks = (SOCKET*)arg;
	SOCKET clientSocks[2] = { socks[0], socks[1] };
	
	int GameTime = 120;
	clock_t CurrentTime = clock();
	
	short opcode = 1;

	while (1)
	{
		WaitForSingleObject(hGameEvent, INFINITE);

		clock_t newTime = clock();
		if ((newTime - CurrentTime) > CLOCKS_PER_SEC)
		{
			
			--GameTime;
			for (int i = 0; i < CLIENT_NUM; ++i)
			{
				send(clientSocks[i], (char*)&opcode, sizeof(short), 0);
				send(clientSocks[i], (char*)&GameTime, sizeof(int), 0);
			}
			CurrentTime = newTime;
			cout << GameTime << endl;
			
		}

		SetEvent(hTimeEvent);
	}

	return 0;
}

void InitServerSendData()
{
	//ServerGameData.ServerTime;
}

void SetCilentData()
{
	copy(Bottom.begin(), Bottom.end(), ServerGameData.Bottom);
}

// 클라이언트에서 자신의 정보와 타인의 정보 구분을 위한 멤버변수 세팅을 위한 함수
void SettingPlayersMine(DWORD ThreadId)
{
	for (auto iter = ClientManager.begin(); iter != ClientManager.end(); ++iter)
		iter->second->mine = false;
	(*ClientManager[ThreadId]).mine = true;
}

// threadId를 통해서 플레이어 구분해서 map으로 관리
void CheckInsertPlayerMgrData(DWORD ThreadId)
{
	auto manager = ClientManager.find(ThreadId);
	if (manager == ClientManager.end())
	{
		for (int i = 0; i < CLIENT_NUM; ++i) {
			if (!ServerGameData.PMgrs[i].threadId)
			{
				ServerGameData.PMgrs[i].threadId = ThreadId;
				ClientManager.insert({ ThreadId, &ServerGameData.PMgrs[i] });
				break;
			}
		}
	}
}

// 시간이 초과되거나 플레이어가 추락했을 경우를 검사
bool IsGameOver(CPlayer* player)
{
	if ((*player).y < UNDER) return true;
	// 시간 끝났을때 조건도 추가필요
	return false;
}

bool compare(PlayerMgr& p1, PlayerMgr& p2)
{
	return p1.player.m_nScore > p2.player.m_nScore;
}

void CheckGameWin(DWORD ThreadId)
{
	sort(ServerGameData.PMgrs, ServerGameData.PMgrs + CLIENT_NUM, compare);
	(*ClientManager[ThreadId]).Win = (ThreadId == ServerGameData.PMgrs[0].threadId) ? true : false;
}