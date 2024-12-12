/*#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>

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
}ProtocolFile;

typedef struct {
    ProtocolHeader header;
    ProtocolBody body;
    ProtocolFile file;
} ProtocolPacket;

void serializePacket(const ProtocolPacket* packet, uint8_t** buffer, uint32_t* bufferSize) {
    if (packet->header.messageType != 4) {
        // 전체 버퍼 크기 계산
        *bufferSize = sizeof(ProtocolHeader) + sizeof(uint32_t) + packet->body.messageLength;
        // 버퍼 할당
        *buffer = (uint8_t*)malloc(*bufferSize);
        if (*buffer == NULL) {
            perror("메모리 할당 실패");
            exit(EXIT_FAILURE);
        }

        // 버퍼에 데이터 복사
        uint8_t* ptr = *buffer;

        // 1. 헤더 복사
        memcpy(ptr, &packet->header, sizeof(ProtocolHeader));
        ptr += sizeof(ProtocolHeader);

        // 2. 메시지 길이 복사
        memcpy(ptr, &packet->body.messageLength, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        // 3. 메시지 내용 복사
        memcpy(ptr, packet->body.messageContent, packet->body.messageLength);
    }
    else {
        *bufferSize = sizeof(ProtocolHeader) + sizeof(uint32_t) + packet->file.filenameLength + sizeof(uint32_t) + packet->file.fileLength;

        *buffer = (uint8_t*)malloc(*bufferSize);
        if (*buffer == NULL) {
            perror("메모리 할당 실패");
            exit(EXIT_FAILURE);
        }
        // 버퍼에 데이터 복사
        uint8_t* ptr = *buffer;

        // 1. 헤더 복사
        memcpy(ptr, &packet->header, sizeof(ProtocolHeader));
        ptr += sizeof(ProtocolHeader);

        memcpy(ptr, &packet->file.filenameLength, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        memcpy(ptr, packet->file.fileName, packet->file.filenameLength);
        ptr += packet->file.filenameLength;

        memcpy(ptr, &packet->file.fileLength, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        memcpy(ptr, packet->file.myFile, packet->file.fileLength);
    }
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
        printf("명령 입력 : ");
        scanf("%d", &command);
        ProtocolPacket packet = { 0 };

        if (command == 1) {
            printf("메세지 입력 : ");
            scanf(" %[^\n]s", message);

            // 패킷 생성 및 초기화
            packet.header.messageType = MESSAGE;
            packet.body.messageLength = strlen(message) + 1;
            packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
            if (packet.body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(packet.body.messageContent, message, packet.body.messageLength);
            packet.header.bodyLength = sizeof(uint32_t) + packet.body.messageLength;
            printf("head : %d", sizeof(ProtocolHeader));
            printf("body : %u", packet.header.bodyLength);
        }
        else if (command == 2) {

            // 패킷 생성 및 초기화
            packet.header.messageType = CONNECT;
            packet.body.messageLength = 8;
            packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
            if (packet.body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(packet.body.messageContent, "connect", packet.body.messageLength);
            packet.header.bodyLength = sizeof(uint32_t) + packet.body.messageLength;

        }
        else if (command == 3) {

            // 패킷 생성 및 초기화
            packet.header.messageType = DISCONNECT;
            packet.body.messageLength = 11;
            packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
            if (packet.body.messageContent == NULL) {
                perror("메모리 할당 실패");
                return -1;
            }
            memcpy(packet.body.messageContent, "disconnect", packet.body.messageLength);
            packet.header.bodyLength = sizeof(uint32_t) + packet.body.messageLength;
        }
        else if (command == 4) {
            printf("file name input :");
            scanf(" %[^\n]s", fileNm);
            uint32_t fsize = 0;
            file = fopen(fileNm, "rb");
            fseek(file, 0, SEEK_END);
            fsize = ftell(file);
            fseek(file, 0, SEEK_SET);


            packet.header.messageType = FILETRANS;
            packet.file.filenameLength = 20;
            packet.file.fileName = (uint8_t*)malloc(packet.file.filenameLength);
            memcpy(packet.file.fileName, fileNm, packet.file.filenameLength);
            packet.file.fileLength = fsize;
            packet.file.myFile = (uint8_t*)malloc(packet.file.fileLength);
            uint8_t* fileData = (uint8_t*)malloc(fsize);
            fread(fileData, 1, fsize, file);
            packet.file.myFile = fileData;
            packet.header.bodyLength = sizeof(uint32_t) + packet.file.filenameLength + sizeof(uint32_t) + fsize;
            printf("file name : %s\n", packet.file.fileName);
            printf("file name length : %u\n", packet.file.filenameLength);
            printf("file length : %u\n", packet.file.fileLength);
        }
        // 직렬화
        uint8_t* buffer = NULL;
        uint32_t bufferSize = 0;
        serializePacket(&packet, &buffer, &bufferSize);
        printf("buffer size is : %u\n", bufferSize);


        if (write(client_socket, buffer, bufferSize) < 0) {
            perror("데이터 전송 실패");
            free(packet.body.messageContent);
            free(buffer);
            break;
        }

        printf("client message shoot\n");
        free(packet.body.messageContent);
        free(packet.file.fileName);
        free(buffer);
        if (command == 3) {
            break;
        }
    }
    fclose(file);
    close(client_socket);

    return 0;
}*/