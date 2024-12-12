#include "protocol.h"

// ������ȭ �Լ�
ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize) {
	ProtocolPacket packet;

	// 1. ��� ����
	memcpy(&packet.header, buffer, sizeof(ProtocolHeader));
	buffer += sizeof(ProtocolHeader);

	if (packet.header.messageType != 4) {
		// 2. �޽��� ���� ����
		memcpy(&packet.body.messageLength, buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);

		// 3. �޽��� ���� ����
		packet.body.messageContent = (uint8_t*)malloc(packet.body.messageLength);
		if (packet.body.messageContent == NULL) {
			perror("�޸� �Ҵ� ����");
			exit(EXIT_FAILURE);
		}
		memcpy(packet.body.messageContent, buffer, packet.body.messageLength);
	}
	else {
		memcpy(&packet.file.filenameLength, buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);

		packet.file.fileName = (uint8_t*)malloc(packet.file.filenameLength);
		if (packet.file.fileName == NULL) {
			perror("�޸� �Ҵ� ����");
			exit(EXIT_FAILURE);
		}
		memcpy(packet.file.fileName, buffer, packet.file.filenameLength);
		buffer += packet.file.filenameLength;

		memcpy(&packet.file.fileLength, buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);

		packet.file.myFile = (uint8_t*)malloc(packet.file.fileLength);
		if (packet.file.myFile == NULL) {
			perror("�޸� �Ҵ� ����");
			exit(EXIT_FAILURE);
		}
		memcpy(packet.file.myFile, buffer, packet.file.fileLength);
	}

	return packet;
}et;