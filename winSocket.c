#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winSocket.h"

SOCKET initServer(WSADATA wsa, SOCKET listen_sock
	, SOCKADDR_IN serveraddr, SOCKET client_sock,
	SOCKADDR_IN clientaddr, int addrlen) 
{
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return -1;
	}

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_sock == INVALID_SOCKET) { 
		err_quit("socket()"); 
	}
	printf("소켓이 생성되었습니다\n");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(4000);
	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	printf("Bind 완료 되었습니다.\n");

	listen(listen_sock, SOMAXCONN);
	printf("connect 연결을 기다리는 중..\n");

	addrlen = sizeof(clientaddr);
	//연결이루어질 소켓 지정
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //형변환시켜서
	// socket함수 호출시 반환된 소켓 식별 번호, accept 성공시 연결된 클라이언트의 주소와 포트가 저장될 구조체, 구조체의 크기
	printf("요청을 받았습니다.\n");

	return client_sock;
}
