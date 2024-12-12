#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS//fopen > ���

#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"protocol.h"
#pragma comment(lib, "ws2_32")

#define BUFSIZE 4048

void err_quit(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg); // ���� �޽��� ���
	exit(EXIT_FAILURE);                 // ���α׷� ����
}

int main(int argc, char* argv[]) {
	int retval;
	//�����ʱ�ȭ 
	WSADATA wsa;
	//������ ���� �ʱ�ȭ ������ ������ �ִ� ����ü 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // ��������� �����ϱ� �����Է�
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
	FILE* file = NULL;

	//accept() ���� ��û ���� 
	addrlen = sizeof(clientaddr);
	//�����̷���� ���� ����
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //����ȯ���Ѽ�
	// socket�Լ� ȣ��� ��ȯ�� ���� �ĺ� ��ȣ, accept ������ ����� Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ�� ����� ����ü, ����ü�� ũ��
	printf("��û�� �޾ҽ��ϴ�.\n");

	while (1) {

		//recv() ������ ���� ���ӿ����� read, write ��� recv send ���
		retval = recv(client_sock, buf, BUFSIZE, 0); // ���� �������� ũ�⸦ ��ȯ
		
		printf("�޼����� �����Ͽ����ϴ�.\n");
		for (int i = 0; i < retval; i++) {
			printf("%02X ", buf[i]);
		}
		printf("\n");

		ProtocolPacket packet = deserializePacket(buf, BUFSIZE);
		//�޴� ������ ���
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
		else if (packet.header.messageType == 4) {
			printf("Message Type: %u\n", packet.header.messageType);
			printf("file name length : %u\n", packet.file.filenameLength);
			printf("file name : %s\n", packet.file.fileName);
			printf("file length : %u\n", packet.file.fileLength);

			FILE* newFile = fopen(packet.file.fileName, "wb");
			if (!newFile) {
				perror("fopen failed");
				return 1;
			}

			size_t written = fwrite(packet.file.myFile, 1, packet.file.fileLength, newFile);
			if (written != packet.file.fileLength) {
				perror("fwrite failed");
				fclose(newFile);
				return 1;
			}
			fclose(newFile);
			printf("File transport success!\n");
		}

	}
	
	//closesocket
	closesocket(listen_sock);
	closesocket(client_sock);
	//���� ����
	WSACleanup();
	return 0;

}