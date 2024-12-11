#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 

#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#pragma comment(lib, "ws2_32")

#define BUFSIZE 256

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
	MESSAGE = 1,
	CONNECT = 2,
	DISCONNECT = 3
} MessageType;

// �������� ����ü ����
typedef struct {
	uint32_t messageType;
	uint32_t bodyLength;
} ProtocolHeader;

typedef struct {
	uint32_t messageLength;
	uint8_t* messageContent;
} ProtocolBody;

typedef struct {
	ProtocolHeader header;
	ProtocolBody body;
} ProtocolPacket;

// ������ȭ �Լ�
ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize) {
	ProtocolPacket packet;

	// 1. ��� ����
	memcpy(&packet.header, buffer, sizeof(ProtocolHeader));
	buffer += sizeof(ProtocolHeader);

	// 2. �޽��� ���� ����
	memcpy(&packet.body.messageLength, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);

	// 3. �޽��� ���� ����
	packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
	if (packet.body.messageContent == NULL) {
		perror("�޸� �Ҵ� ����");
		exit(EXIT_FAILURE);
	}
	memcpy(packet.body.messageContent, buffer, packet.body.messageLength);

	return packet;
}et;

int main(int argc, char* argv[]) {
	int retval;

	//�����ʱ�ȭ 
	WSADATA wsa;
	//������ ���� �ʱ�ȭ ������ ������ �ִ� ����ü
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // ��������� �����ϱ�
		return -1;

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

	while (1) {

		//recv() ������ ����
		retval = recv(client_sock, buf, BUFSIZE, 0); // ���� �������� ũ�⸦ ��ȯ
		//����� ��ü�� �Ǵ� ���� ��ũ����, ���� �޼����� ������ ���� ������, ���ۻ�����, �ɼ�
		printf("�޼����� �����Ͽ����ϴ�.\n");
		ProtocolPacket packet = deserializePacket(buf, BUFSIZE);
		//�޴� ������ ���
		buf[retval] = '\0'; // ���ڿ� ǥ������ ���๮��
		printf("[TCP/%s:%d]\n",
			inet_ntoa(clientaddr.sin_addr), // 32��Ʈ ���� > ���ڿ��� ����
			ntohs(clientaddr.sin_port));

		// ������ Ȯ��
		if (packet.header.messageType == 1) {
			printf("msg notice\n");
			printf("Message Type: %u\n", packet.header.messageType);
			printf("Body Length: %u\n", packet.header.bodyLength);
			printf("Message Length: %u\n", packet.body.messageLength);
			printf("Message Content: %s\n", packet.body.messageContent);
		}
		else if (packet.header.messageType == 2) {
			printf("connect\n");
			printf("Message Type: %u\n", packet.header.messageType);
			printf("Body Length: %u\n", packet.header.bodyLength);
			printf("Message Length: %u\n", packet.body.messageLength);
			printf("Message Content: %s\n", packet.body.messageContent);
		}
		else if (packet.header.messageType == 3) {
			printf("disconnect\n");
			printf("Message Type: %u\n", packet.header.messageType);
			printf("Body Length: %u\n", packet.header.bodyLength);
			printf("Message Length: %u\n", packet.body.messageLength);
			printf("Message Content: %s\n", packet.body.messageContent);
			break;
		}

	}

	//closesocket
	closesocket(listen_sock);
	closesocket(client_sock);
	//���� ����
	WSACleanup();
	return 0;

}