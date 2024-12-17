#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<time.h>
#include<sys/stat.h>

#define BUFF_SIZE 4048

typedef enum {
    MESSAGE = 1,
    CONNECT = 2, 
    DISCONNECT = 3,
    FILETRANS = 4
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
    uint32_t filenameLength;
    uint8_t* fileName;
    uint32_t fileLength;
    uint8_t* myFile;
    time_t accessTime;
    time_t modifyTime;
}ProtocolFile;


uint8_t* headSerialize(ProtocolHeader header) {
    uint8_t* buffer = malloc(sizeof(ProtocolHeader));

    memcpy(buffer, &header, sizeof(ProtocolHeader));
    return buffer;
}

uint8_t* msgSerialize(ProtocolBody body, int size) {
    uint8_t* buffer = malloc(size);
    uint8_t* current = buffer;
    // 현재 위치를 추적하는 포인터

    // 2. 메시지 길이 복사
    memcpy(current, &body.messageLength, sizeof(uint32_t));
    printf("message length : %u\n", body.messageLength);
    current += sizeof(uint32_t);

    // 3. 메시지 내용 복사
    memcpy(current, body.messageContent, body.messageLength);
    printf("message : %s\n", body.messageContent);

    printf("\n");

    return buffer;
}

uint8_t* fileSerialize(ProtocolFile filePacket,int size) {
    uint8_t* buffer = malloc(size);
    uint8_t* current = buffer;

    memcpy(current, &filePacket.filenameLength, sizeof(uint32_t));
    current += sizeof(uint32_t);

    memcpy(current, filePacket.fileName, filePacket.filenameLength);
    current += filePacket.filenameLength;

    memcpy(current, &filePacket.fileLength, sizeof(uint32_t));
    current += sizeof(uint32_t);

    memcpy(current, filePacket.myFile, filePacket.fileLength);
    current += filePacket.fileLength;

    memcpy(current, filePacket.accessTime, sizeof(time_t));
    current += sizeof(time_t);

    memcpy(current, filePacket.modifyTime, sizeof(time_t));

    return buffer;
}

int main(int argc, char** argv) {
    int client_socket;
    struct sockaddr_in server_addr;
    int command;
    char message[BUFF_SIZE];
    char fileNm[BUFF_SIZE];
    FILE* file = NULL;
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == client_socket) {
        printf("socket fail");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.10.41");


    if (-1 == connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        printf("connecting fail");
        exit(1);
    }

    while (1) {
        uint8_t* headerBuffer = NULL;
        uint8_t* msgBuffer = NULL;
        uint8_t* fileBuffer = NULL;
	struct stat sb;
        printf("명령 입력 : ");
        scanf("%d", &command);
	getchar();
        ProtocolHeader header = { 0 };
        ProtocolBody body = { 0 };
        ProtocolFile filePacket = { 0 };

        if (command == 1) {
            printf("메세지 입력 : ");
            scanf(" %[^\n]s", message);
	    getchar();
            // 패킷 생성 및 초기화
            header.messageType = MESSAGE;
            body.messageLength = strlen(message) + 1;
            body.messageContent = (uint8_t*)malloc(body.messageLength);
            if (body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(body.messageContent, message, body.messageLength);
            header.bodyLength = sizeof(uint32_t) + body.messageLength;

            headerBuffer = headSerialize(header);

            write(client_socket, headerBuffer, sizeof(ProtocolHeader));
            printf("------send header packet------\n");
        }
        else if (command == 2) {

            // 패킷 생성 및 초기화
            header.messageType = CONNECT;
            body.messageLength = 8;
            body.messageContent = (uint8_t*)malloc(body.messageLength);
            if (body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(body.messageContent, "connect", body.messageLength);
            header.bodyLength = sizeof(uint32_t) + body.messageLength;
            headerBuffer = headSerialize(header);

            write(client_socket, headerBuffer, sizeof(ProtocolHeader));
	    free(headerBuffer);
            printf("------send header packet------\n");
        }
        else if (command == 3) {

            // 패킷 생성 및 초기화
            header.messageType = DISCONNECT;
            body.messageLength = 11;
            body.messageContent = (uint8_t*)malloc(body.messageLength);
            if (body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(body.messageContent, "disconnect", body.messageLength);
            header.bodyLength = sizeof(uint32_t) + body.messageLength;
            headerBuffer = headSerialize(header);

            write(client_socket, headerBuffer, sizeof(ProtocolHeader));
            printf("------send header packet------\n");
        }
        else if (command == 4) {
            printf("file name input :");
            scanf(" %[^\n]s", fileNm);
            getchar();
            uint32_t fsize = 0;
            file = fopen(fileNm, "rb");
            fseek(file, 0, SEEK_END);
            fsize = ftell(file);
            fseek(file, 0, SEEK_SET);


            header.messageType = FILETRANS;
            filePacket.filenameLength = strlen(fileNm)+1;
            filePacket.fileName = (uint8_t*)malloc(filePacket.filenameLength);
            memcpy(filePacket.fileName, fileNm, filePacket.filenameLength);
            filePacket.fileLength = fsize;
            filePacket.myFile = (uint8_t*)malloc(filePacket.fileLength);
            uint8_t* fileData = (uint8_t*)malloc(fsize);
            fread(fileData, 1, fsize, file);
            filePacket.myFile = fileData;
            header.bodyLength = sizeof(uint32_t) + filePacket.filenameLength + sizeof(uint32_t) + fsize + sizeof(time_t) + sizeof(time_t);
	    printf("------body length ------ : %u\n", header.bodyLength);
            headerBuffer = headSerialize(header);
            write(client_socket, headerBuffer, sizeof(ProtocolHeader));
            printf("------send header packet------\n");

            printf("file name : %s\n", filePacket.fileName);
            printf("file name length : %u\n", filePacket.filenameLength);
            printf("file length : %u\n", filePacket.fileLength);
            fclose(file);

	    if(stat(fileNm, &sb) == -1){
                perror("stat");
                return 1;
            }
	    filePacket.accessTime = &sb.st_atime;
	    filePacket.modifyTime = &sb.st_mtime;
	    printf("a time : %s\n", ctime(&sb.st_atime));
	    printf("m time : %s\n", ctime(&sb.st_mtime));
        }
        // 직렬화
        uint32_t bufferSize = header.bodyLength;
	uint8_t* buffer = NULL;
	
	if (header.messageType != 4) {
            buffer = msgSerialize(body, bufferSize);
            if (write(client_socket, buffer, bufferSize) < 0) {
                perror("데이터 전송 실패");
                free(body.messageContent);
                free(headerBuffer);
                free(buffer);
                break;
            }
        }
        else if (header.messageType == 4) {
           buffer = fileSerialize(filePacket, bufferSize);

           if (header.bodyLength > 1024) {
               int bytesSent = 0; // 누적 전송 바이트
               int remaining = header.bodyLength; // 남은 데이터 크기
               uint8_t* current = buffer;

               while (remaining > 0) {

                   int chunkSize = (remaining >= 1024) ? 1024 : remaining; // 이번에 보낼 크기 결정

                   int size = write(client_socket, current, chunkSize); // 데이터 전송
                   if (size <= 0) {
                       perror("데이터 전송 실패");
                       free(filePacket.fileName);
                       free(filePacket.myFile);
                       free(headerBuffer);
                       free(buffer);
                       break;
                   }

                   bytesSent += size; // 전송된 크기 누적
                   current += size;   // 현재 포인터 이동
                   remaining -= size; // 남은 크기 감소
               }

               printf("총 %d 바이트를 전송했습니다.\n", bytesSent);
           }
           else {
               // 데이터 크기가 1024바이트 이하인 경우, 한 번에 전송
               int size = write(client_socket, buffer, header.bodyLength);
               if (size <= 0) {
                   perror("데이터 전송 실패");
                   free(filePacket.fileName);
                   free(filePacket.myFile);
                   free(headerBuffer);
                   free(buffer);
               }
               else {
                   printf("총 %d 바이트를 전송했습니다.\n", size);
               }
           }
        }


        printf("client message shoot\n");
        free(body.messageContent);
        free(filePacket.fileName);
        free(filePacket.myFile);
        free(buffer);
	
        if (command == 3) {
            break;
        }
    }
    close(client_socket);

    return 0;
}


