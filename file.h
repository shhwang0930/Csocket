#define _CRT_SECURE_NO_WARNINGS
#include"protocol.h"
#include<stdint.h>

FILE* fileOpen(ProtocolFile file);
void fileWrite(ProtocolFile file, FILE* newFile);