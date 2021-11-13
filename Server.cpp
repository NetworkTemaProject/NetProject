#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <Player.h>

#define SERVERPORT 9000
#define BUFSIZE 512
#define CLINET_NUM 3

using namespace std;

int custom_counter();
// vector<Foothold> Bottom;

struct InputData {
	bool bUp = false;
	bool bRight = false;
	bool bLeft = false;
	bool bDown = false;
	bool bSpace = false;
	bool bEnter = false;
};

struct PlayerMgr
{
	DWORD threadId;
	// CPlayer player;
};

struct SendGameData {
	PlayerMgr players[CLINET_NUM];
	clock_t ServerTime;
	bool Win;
	std::vector<Foothold> Bottom;
};

struct SendPlayerData {
	InputData Input;
	clock_t ClientTime;
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

PlayerMgr players[CLINET_NUM];



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




// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
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

	while (1) {
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) break;


		// 접속한 클라이언트 정보 출력
		cout << endl << "[TCP 서버] 클라이언트 접속: IP 주소=";
		cout << inet_ntoa(clientaddr.sin_addr) << ",포트번호=" << ntohs(clientaddr.sin_port) << endl;

		/* 클라이언트와 데이터 통신*/

		// 데이터 받기 - filename
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		retval = recvn(client_sock, buf, len, 0);
		if (retval == 0) break;

		buf[retval] = '\0';

		// 총 데이터 크기 받기
		retval = recvn(client_sock, (char*)&fsize, sizeof(int), 0);
		if (retval == 0 || retval > BUFSIZE) break;

		cout << fixed;
		cout.precision(1);

		while (cnt < fsize) {
			// 분할된 데이터 받기
			retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
			retval = recvn(client_sock, buf, len, 0);
			if (retval == 0) break;

			buf[retval] = '\0';
			
			cnt += len;
			cout << "전송중 - 전송률 " << (float)cnt/(float)fsize*100.0f <<"%" <<'\r'; 
		}
		if (cnt >= fsize) cout << "전송완료";

		// closesocket()
		closesocket(client_sock);
		cout << endl << "[TCP 서버] 클라이언트 종료: IP 주소=";
		cout << inet_ntoa(clientaddr.sin_addr) << ",포트번호=" << ntohs(clientaddr.sin_port) << endl;
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

void UpdatePlayerLocation(CPlayer& p, InputData& input)
{
	if (input.dUp) p.dz = -0.1;
	if (input.dDown) p.dz = 0.1;
	if (input.dLeft) p.dx = -0.1;
	if (input.dRight) p.dx = 0.1;

	// 업데이트 중인지 판단 -> dx dz로 판단
	// 현재 키 입력 전부 안된상태 -> 0으로 초기화 (업데이트 중지)
	if (p.dz && !input.dUp && !input.dDown) p.dz = 0.0;
	if (p.dx && !input.dLeft && !input.dRight) p.dx = 0.0; 
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
}

void PlayerInit()
{
}

DWORD __stdcall ProcessClient(LPVOID arg)
{
	CPlayer* tPlayer;
	for (int i = 0; i < CLIENT_NUM; ++i)
		if (players[i].threadId == GetCurrentThreadId()) tPlayer = &players[i].player;
	// map 사용하면 -> .find( ) .. 돌아가면 이걸로 바꾼는게 나은거같음

	while(1)
	{
		// recvn을 통해서 Inputdata , time 받아옴

		UpdatePlayerLocation(tPlayer, PlayerMgr.input);
		
	}
	return 0;
}

bool IsReadytoPlay(bool isReady)
{

}