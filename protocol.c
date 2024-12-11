#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "protocol.h"
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