#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "file.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#define BUFSIZE 4048
#define HEADBUF 8

void err_quit(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg); // 에러 메시지 출력
	exit(EXIT_FAILURE);                 // 프로그램 종료
}

int main(int argc, char* argv[]) {
	// Set console code page to UTF-8 (65001). #include <windows.h> 필요
	// Set locale to Korean (South Korea)
	// 
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
	char headbuf[HEADBUF];

	//accept() 연결 요청 수락 
	addrlen = sizeof(clientaddr);
	//연결이루어질 소켓 지정
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen); //형변환시켜서
	// socket함수 호출시 반환된 소켓 식별 번호, accept 성공시 연결된 클라이언트의 주소와 포트가 저장될 구조체, 구조체의 크기
	printf("요청을 받았습니다.\n");

	while (1) {
		int headre = 0;
		int bodyre = 0;
		//header 수신
		headre = recv(client_sock, headbuf, HEADBUF, 0);
		if (headre == SOCKET_ERROR) {
			printf("헤더패킷을 수신XXXXX.\n");
			break;
		}
		printf("헤더패킷을 수신하였습니다.\n");
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
					perror("데이터 수신 실패");
					free(buf);
					exit(EXIT_FAILURE);
				}
				else if (size == 0) {
					printf("서버 연결이 종료되었습니다.\n");
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
			printf("바디패킷을 수신XXXXX.\n");
			break;
		}
		printf("바디패킷을 수신하였습니다.\n");

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
	//원속 종료
	WSACleanup();
	return 0;

}