#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "file.h"
#include "protocol.h"
#include "winSocket.h"
#define HEADBUF 8

void err_quit(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	//testtest
	WSADATA wsa = {0};
	SOCKET listen_sock = NULL;
	SOCKADDR_IN serveraddr = { 0 };
	SOCKET client_sock = NULL;
	SOCKADDR_IN clientaddr = { 0 };
	int addrlen = NULL;

	char headbuf[HEADBUF];

	client_sock = initServer(wsa, listen_sock, serveraddr, client_sock, clientaddr, addrlen);

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
		printf("header msg Length : %u\n", header.bodyLength);

		int bytesReceived = 0;
		int bodylength = header.bodyLength;
		int remaining = header.bodyLength;
		int8_t* buf = (int8_t*)malloc(bodylength);
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
			FILE* newFile = fileOpen(file.fileName);
			fileWrite(file.myFile, file.fileLength, newFile);
			fclose(newFile);
			FILETIME act, mdt;
			unix_time_to_filetime(file.accessTime, &act);
			unix_time_to_filetime(file.modifyTime, &mdt);
			updateMetaData(act, mdt, file.fileName);
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