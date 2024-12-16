#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "file.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#define BUFSIZE 4048
#define HEADBUF 8

void err_quit(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg); // ���� �޽��� ���
	exit(EXIT_FAILURE);                 // ���α׷� ����
}

int main(int argc, char* argv[]) {
	// Set console code page to UTF-8 (65001). #include <windows.h> �ʿ�
	// Set locale to Korean (South Korea)
	// 
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
	char headbuf[HEADBUF];

	//accept() ���� ��û ���� 
	addrlen = sizeof(clientaddr);
	//�����̷���� ���� ����
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //����ȯ���Ѽ�
	// socket�Լ� ȣ��� ��ȯ�� ���� �ĺ� ��ȣ, accept ������ ����� Ŭ���̾�Ʈ�� �ּҿ� ��Ʈ�� ����� ����ü, ����ü�� ũ��
	printf("��û�� �޾ҽ��ϴ�.\n");

	while (1) {
		int headre = 0;
		int bodyre = 0;
		//header ����
		headre = recv(client_sock, headbuf, HEADBUF, 0);
		if (headre == SOCKET_ERROR) {
			printf("�����Ŷ�� ����XXXXX.\n");
			break;
		}
		printf("�����Ŷ�� �����Ͽ����ϴ�.\n");
		printf("\n");
		ProtocolHeader header = headDeserialize(headbuf, HEADBUF);
		ProtocolBody body = { 0 };
		ProtocolFile file = { 0 };
		printf("header packet type : %u\n", header.messageType);

		int bodylength = header.bodyLength;
		int8_t* buf = (int8_t*)malloc(bodylength);
		int bytesReceived = 0;
		int remaining = header.bodyLength;
		int8_t* current = buf;

		if (header.messageType == 4 && bodylength > 1024) {
			while (remaining > 0) {
				int size = recv(client_sock, current, remaining, 0);
				if (size < 0) {
					perror("������ ���� ����");
					free(buf);
					exit(EXIT_FAILURE);
				}
				else if (size == 0) {
					printf("���� ������ ����Ǿ����ϴ�.\n");
					break;
				}

				bytesReceived += size; 
				current += size;
				remaining -= size;
			}
			printf("total file byte : %d \n", bytesReceived);
		}
		else {
			recv(client_sock, buf, bodylength, 0);
		}

		printf("\n");


		if (bodyre == SOCKET_ERROR) {
			printf("�ٵ���Ŷ�� ����XXXXX.\n");
			break;
		}
		printf("�ٵ���Ŷ�� �����Ͽ����ϴ�.\n");

		printf("[TCP/%s:%d]\n",
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port));

		if (header.messageType == 1) {
			body = bodyDeserialize(buf, bodylength);
			printmsgPacket(body);
		}
		else if (header.messageType == 2) {
			body = bodyDeserialize(buf, bodylength);
			printmsgPacket(body);
		}
		else if (header.messageType == 3) {
			body = bodyDeserialize(buf, bodylength);
			printmsgPacket(body);
			break;
		}
		else if (header.messageType == 4) {
			file = fileDeserialize(buf, bodylength);
			printfilePacket(file);
			FILE* newFile = fileOpen(file);
			fileWrite(file, newFile);
			fclose(newFile);
			printf("File transport success!\n");
		}
		free(body.messageContent);
		free(file.fileName);
		free(file.myFile);
		free(buf);

	}
	
	//closesocket
	closesocket(listen_sock);
	closesocket(client_sock);
	//���� ����
	WSACleanup();
	return 0;

}