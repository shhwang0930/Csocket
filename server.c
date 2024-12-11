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

// 프로토콜 구조체 정의
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

// 역직렬화 함수
ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize) {
	ProtocolPacket packet;

	// 1. 헤더 복원
	memcpy(&packet.header, buffer, sizeof(ProtocolHeader));
	buffer += sizeof(ProtocolHeader);

	// 2. 메시지 길이 복원
	memcpy(&packet.body.messageLength, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);

	// 3. 메시지 내용 복원
	packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
	if (packet.body.messageContent == NULL) {
		perror("메모리 할당 실패");
		exit(EXIT_FAILURE);
	}
	memcpy(packet.body.messageContent, buffer, packet.body.messageLength);

	return packet;
}et;

int main(int argc, char* argv[]) {
	int retval;

	//윈속초기화 
	WSADATA wsa;
	//윈도우 소켓 초기화 정보를 가지고 있는 구조체
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 윈도우소켓 시작하기
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
	//MessageBox(NULL, "TCP 소켓성공", "알림", MB_OK);

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

	//accept() 연결 요청 수락
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	// socket함수 호출시 반환된 소켓 식별 번호, accept 성공시 연결된 클라이언트의 주소와 포트가 저장될 구조체, 구조체의 크기
	printf("요청을 받았습니다.\n");

	while (1) {

		//recv() 데이터 수신
		retval = recv(client_sock, buf, BUFSIZE, 0); // 받은 데이터의 크기를 반환
		//통신의 주체가 되는 소켓 디스크립터, 받은 메세지를 저장할 버퍼 포인터, 버퍼사이즈, 옵션
		printf("메세지를 수신하였습니다.\n");
		ProtocolPacket packet = deserializePacket(buf, BUFSIZE);
		//받는 데이터 출력
		buf[retval] = '\0'; // 문자열 표기위한 개행문자
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

	}

	//closesocket
	closesocket(listen_sock);
	closesocket(client_sock);
	//원속 종료
	WSACleanup();
	return 0;

}