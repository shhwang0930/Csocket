#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

typedef enum {
	MESSAGE = 1,
	CONNECT = 2,
	DISCONNECT = 3,
	FILETRANS = 4
} MessageType;

// �������� ����ü ����
//uintx_t > bit�� ���� > �̽ļ� ����
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


ProtocolPacket deserializePacket(const uint8_t* buffer, uint32_t bufferSize);