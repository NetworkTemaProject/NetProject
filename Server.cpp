#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE 512

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