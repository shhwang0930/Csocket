#include "protocol.h"

ProtocolHeader headDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolHeader header;
    // 1. ��� ����
    memcpy(&header, buffer, sizeof(ProtocolHeader));
    return header;
};

ProtocolBody bodyDeserialize(const uint8_t* buffer, uint32_t bufferSize) {
    ProtocolBody body;

    // ���� ũ�� Ȯ��
    if (bufferSize < sizeof(uint32_t)) {
        fprintf(stderr, "���� ũ�Ⱑ �ʹ� �۽��ϴ�.\n");
        exit(EXIT_FAILURE);
    }

    // �޽��� ���� ����
    memcpy(&body.messageLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);
    bufferSize -= sizeof(uint32_t);

    // �޽��� ���� ��ȿ�� ����
    if (bufferSize < body.messageLength) {
        fprintf(stderr, "�޽��� ���̰� ��ȿ���� �ʽ��ϴ�. (body.messageLength: %u, bufferSize: %u)\n",
            body.messageLength, bufferSize);
        exit(EXIT_FAILURE);
    }

    // �޽��� ���� ����
    body.messageContent = (uint8_t*)malloc(body.messageLength);
    if (body.messageContent == NULL) {
        perror("�޸� �Ҵ� ����");
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
        perror("�޸� �Ҵ� ����1");
        exit(EXIT_FAILURE);
    }
    memcpy(file.fileName, buffer, file.filenameLength);
    buffer += file.filenameLength;

    memcpy(&file.fileLength, buffer, sizeof(uint32_t));
    buffer += sizeof(uint32_t);

    file.myFile = (uint8_t*)malloc(file.fileLength);
    if (file.myFile == NULL) {
        perror("�޸� �Ҵ� ����2");
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