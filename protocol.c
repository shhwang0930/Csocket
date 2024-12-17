#include "protocol.h"
#pragma warning(disable : 4996)

ProtocolHeader headDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolHeader header;
    // 1. 헤더 복원
    memcpy(&header, buffer, sizeof(ProtocolHeader));
    return header;
};

ProtocolBody bodyDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolBody body;

    // 메시지 길이 복원
    memcpy(&body.messageLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);
    bufferSize -= sizeof(uint32_t);

    // 메시지 내용 복원
    body.messageContent = (uint8_t*)malloc(body.messageLength);
    if (body.messageContent == NULL) {
        perror("메모리 할당 실패");
        exit(EXIT_FAILURE);
    }
    memcpy(body.messageContent, buffer, body.messageLength);

    return body;
};

ProtocolFile fileDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolFile file;

    memcpy(&file.filenameLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);

    file.fileName = (uint8_t*)malloc(file.filenameLength);
    if (file.fileName == NULL) {
        perror("메모리 할당 실패1");
        exit(EXIT_FAILURE);
    }
    memcpy(file.fileName, buffer, file.filenameLength);
    buffer += file.filenameLength;

    memcpy(&file.fileLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);

    file.myFile = (uint8_t*)malloc(file.fileLength);
    if (file.myFile == NULL) {
        perror("메모리 할당 실패2");
        exit(EXIT_FAILURE);
    }
    memcpy(file.myFile, buffer, file.fileLength);
    buffer += file.fileLength;

    memcpy(&file.accessTime, buffer, sizeof(time_t));
    buffer += sizeof(time_t);

    memcpy(&file.modifyTime, buffer, sizeof(time_t));

    return file;
}

void printmsgPacket(ProtocolBody body) {
    printf("Message Length: %u\n", body.messageLength);
    printf("Message Content: %s\n", body.messageContent);
}

void printfilePacket(ProtocolFile file) {
    printf("file name length : %u\n", file.filenameLength);
    printf("file name : %s\n", file.fileName);
    printf("file length : %u\n", file.fileLength);
    printf("acess time : %s", ctime(&file.accessTime));
    printf("modify time : %s", ctime(&file.modifyTime));
}