#include "protocol.h"

ProtocolHeader headDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolHeader header;
    // 1. 헤더 복원
    memcpy(&header, buffer, sizeof(ProtocolHeader));
    return header;
};

ProtocolBody bodyDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolBody body;

    // 버퍼 크기 확인
    if (bufferSize < sizeof(uint32_t)) {
        fprintf(stderr, "버퍼 크기가 너무 작습니다.\n");
        exit(EXIT_FAILURE);
    }

    // 메시지 길이 복원
    memcpy(&body.messageLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);
    bufferSize -= sizeof(uint32_t);

    // 메시지 길이 유효성 검증
    if (bufferSize < body.messageLength) {
        fprintf(stderr, "메시지 길이가 유효하지 않습니다. (body.messageLength: %u, bufferSize: %u)\n",
            body.messageLength, bufferSize);
        exit(EXIT_FAILURE);
    }

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
}