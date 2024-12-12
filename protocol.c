#include "protocol.h"

// 역직렬화 함수
ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize) {
	ProtocolPacket packet;

	// 1. 헤더 복원
	memcpy(&packet.header, buffer, sizeof(ProtocolHeader));
	buffer += sizeof(ProtocolHeader);

	if (packet.header.messageType != 4) {
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
	}
	else {
		memcpy(&packet.file.filenameLength, buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);

		packet.file.fileName = (uint8_t*)malloc(packet.file.filenameLength);
		if (packet.file.fileName == NULL) {
			perror("메모리 할당 실패");
			exit(EXIT_FAILURE);
		}
		memcpy(packet.file.fileName, buffer, packet.file.filenameLength);
		buffer += packet.file.filenameLength;

		memcpy(&packet.file.fileLength, buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);

		packet.file.myFile = (uint8_t*)malloc(packet.file.fileLength);
		if (packet.file.myFile == NULL) {
			perror("메모리 할당 실패");
			exit(EXIT_FAILURE);
		}
		memcpy(packet.file.myFile, buffer, packet.file.fileLength);
	}

	return packet;
}et;