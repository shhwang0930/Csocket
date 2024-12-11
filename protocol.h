#pragma once
#include <cstdint>

typedef enum {
	MESSAGE = 1,
	CONNECT = 2,
	DISCONNECT = 3
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
	ProtocolHeader header;
	ProtocolBody body;
} ProtocolPacket;