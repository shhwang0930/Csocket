#define _CRT_SECURE_NO_WARNINGS
#include"protocol.h"

FILE* fileOpen(ProtocolFile file);
void fileWrite(ProtocolFile file, FILE* newFile);
void unix_time_to_filetime(time_t t, FILETIME* ptf);
void convertToWideChar(const char* src, wchar_t* dest, size_t dest_size);
void updateMetaData(FILETIME accessTime, FILETIME modifyTime, char* fileNm);