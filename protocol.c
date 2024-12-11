#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "protocol.h"
// ������ȭ �Լ�
ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize) {
	ProtocolPacket packet;

	// 1. ��� ����
	memcpy(&packet.header, buffer, sizeof(ProtocolHeader));
	buffer += sizeof(ProtocolHeader);

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

	return packet;
}et;