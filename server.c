#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS//fopen > 사용

#include <winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"protocol.h"
#pragma comment(lib, "ws2_32")

#define BUFSIZE 4048

void err_quit(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg); // 에러 메시지 출력
	exit(EXIT_FAILURE);                 // 프로그램 종료
}

int main(int argc, char* argv[]) {
	int retval;
	//윈속초기화 
	WSADATA wsa;
	//윈도우 소켓 초기화 정보를 가지고 있는 구조체 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 윈도우소켓 시작하기 버전입력
		return -1;

	//socket() 소켓 생성
	/*
	socket(int af, int type, int protocol)
	af > 접속하는 방식을 지정
	type > 데이터를 교환하는 방식 지정
	protocol > 사용할 프로토콜을 지정 > AF_INET에서는 상수 0만 넣어도 됨
	*/
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	//AF_INET : IPv4를 사용하는 망
	//SOCK_STREAM : 입/출력 작동의 스트림 방식으로 데이터 교환

	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	printf("소켓이 생성되었습니다\n");

	//bind() 소켓 주소 할당
	//sockaddr 구조체에서 af_inet을 사용할 경우에 사용되는 구조체
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //모든 바이트를 0으로 설정> 구조체의 나머지 필드에 알 수 없는 값이 들어감을 방지
	serveraddr.sin_family = AF_INET; //주소체계
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //32비트 IP주소
	serveraddr.sin_port = htons(4000); //16비트 포트 번호

	bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	// 생성된 소켓, 소켓이 바인딩될 주소, 포트  소켓 주소 구조체의 크기
	printf("Bind 완료 되었습니다.\n");

	//listen() 연결 요청 대기
	listen(listen_sock, SOMAXCONN);
	// 소켓 디스크립터 번호, 연결요청을 대기하는 큐의 크기
	printf("connect 연결을 기다리는 중..\n");

	//통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	FILE* file = NULL;

	//accept() 연결 요청 수락 
	addrlen = sizeof(clientaddr);
	//연결이루어질 소켓 지정
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //형변환시켜서
	// socket함수 호출시 반환된 소켓 식별 번호, accept 성공시 연결된 클라이언트의 주소와 포트가 저장될 구조체, 구조체의 크기
	printf("요청을 받았습니다.\n");

	while (1) {

		//recv() 데이터 수신 윈속에서는 read, write 대신 recv send 사용
		retval = recv(client_sock, buf, BUFSIZE, 0); // 받은 데이터의 크기를 반환
		
		printf("메세지를 수신하였습니다.\n");
		for (int i = 0; i < retval; i++) {
			printf("%02X ", buf[i]);
		}
		printf("\n");

		ProtocolPacket packet = deserializePacket(buf, BUFSIZE);
		//받는 데이터 출력
		printf("[TCP/%s:%d]\n",
			inet_ntoa(clientaddr.sin_addr), // 32비트 숫자 > 문자열로 리턴
			ntohs(clientaddr.sin_port));

		// 데이터 확인
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
	//원속 종료
	WSACleanup();
	return 0;

}