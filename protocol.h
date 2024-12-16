#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

typedef enum {
	MESSAGE = 1,
	CONNECT = 2,
	DISCONNECT = 3,
	FILETRANS = 4
} MessageType;

// 프로토콜 구조체 정의
//uintx_t > bit수 고정 > 이식성 증가
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



ProtocolHeader headDeserialize(const uint8_t* buffer, uint32_t bufferSize);
ProtocolBody bodyDeserialize(const uint8_t* buffer, uint32_t bufferSize);
ProtocolFile fileDeserialize(const uint8_t* buffer, uint32_t bufferSize);
void printmsgPacket(ProtocolBody body);
void printfilePacket(ProtocolFile file);