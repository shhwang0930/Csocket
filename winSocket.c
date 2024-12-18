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
	printf("������ �����Ǿ����ϴ�\n");

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(4000);
	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	printf("Bind �Ϸ� �Ǿ����ϴ�.\n");

	listen(listen_sock, SOMAXCONN);
	printf("connect ������ ��ٸ��� ��..\n");

	addrlen = sizeof(clientaddr);
	//�����̷���� ���� ����
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //����ȯ���Ѽ�
	// socket�Լ� ȣ��� ��ȯ�� ���� �ĺ� ��ȣ, accept ������ ����� Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ�� ����� ����ü, ����ü�� ũ��
	printf("��û�� �޾ҽ��ϴ�.\n");

	return client_sock;
}
