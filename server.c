#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#pragma comment(lib, "ws2_32");

#define BUFSIZE 1024
#define PLENGTH 4

void err_quit(const char* msg) {
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	exit(-1);

}
typedef enum {
	MSG = 1,
	CNT = 2,
	DCNT = 3
}MsgType;
typedef struct {
	uint32_t  type;//4
	uint32_t  bodyLength;//4
} HeaderPacket;
typedef struct {
	uint32_t  messagelength;//4
	uint8_t* message;
} BodyPacket;
typedef struct {
	HeaderPacket header;
	BodyPacket body;
} MyPacket;

int main(int argc, char* argv[]) {
	int retval;

	//�����ʱ�ȭ 
	WSADATA wsa;
	//������ ���� �ʱ�ȭ ������ ������ �ִ� ����ü
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // ��������� �����ϱ�
		return -1;
	//MessageBox(NULL, "���� �ʱ�ȭ ����", "�˸�", MB_OK);

	//socket() ���� ����
	/*
	socket(int af, int type, int protocol)
	af > �����ϴ� ����� ����
	type > �����͸� ��ȯ�ϴ� ��� ����
	protocol > ����� ���������� ���� > AF_INET������ ��� 0�� �־ ��
	*/
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET : IPv4�� ����ϴ� ��
	//SOCK_STREAM : ��/��� �۵��� ��Ʈ�� ������� ������ ��ȯ
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	printf("������ �����Ǿ����ϴ�\n");
	//MessageBox(NULL, "TCP ���ϼ���", "�˸�", MB_OK);

	//bind() ���� �ּ� �Ҵ�
	//sockaddr ����ü���� af_inet�� ����� ��쿡 ���Ǵ� ����ü
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //��� ����Ʈ�� 0���� ����> ����ü�� ������ �ʵ忡 �� �� ���� ���� ���� ����
	serveraddr.sin_family = AF_INET; //�ּ�ü��
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //32��Ʈ IP�ּ�
	serveraddr.sin_port = htons(4000); //16��Ʈ ��Ʈ ��ȣ

	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	// ������ ����, ������ ���ε��� �ּ�, ��Ʈ  ���� �ּ� ����ü�� ũ��
	printf("Bind �Ϸ� �Ǿ����ϴ�.\n");

	//listen() ���� ��û ���
	listen(listen_sock, SOMAXCONN);
	// ���� ��ũ���� ��ȣ, �����û�� ����ϴ� ť�� ũ��
	printf("connect ������ ��ٸ��� ��..\n");

	//��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//accept() ���� ��û ����
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	// socket�Լ� ȣ��� ��ȯ�� ���� �ĺ� ��ȣ, accept ������ ����� Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ�� ����� ����ü, ����ü�� ũ��
	printf("��û�� �޾ҽ��ϴ�.\n");

	//recv() ������ ����
	retval = recv(client_sock, buf, BUFSIZE, 0); // ���� �������� ũ�⸦ ��ȯ
	//����� ��ü�� �Ǵ� ���� ��ũ����, ���� �޼����� ������ ���� ������, ���ۻ�����, �ɼ�
	printf("�޼����� �����Ͽ����ϴ�.\n");

	//�޴� ������ ���
	buf[retval] = '\0'; // ���ڿ� ǥ������ ���๮��
	printf("[TCP/%s:%d] %s\n",
		inet_ntoa(clientaddr.sin_addr), // 32��Ʈ ���� > ���ڿ��� ����
		ntohs(clientaddr.sin_port), buf);
	//�򿣵�ȿ��� ��Ʋ��������� 
	/*
	�� ����� : ū �������� �޸𸮿� ���� ���
	��Ʋ ����� : ���� �������� �޸𸮿� ���� ���
	*/
	//closesocket
	closesocket(listen_sock);

	//���� ����
	WSACleanup();
	return 0;

}