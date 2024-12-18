#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <winsock2.h>
#include<windows.h>
#include<time.h>
#pragma comment(lib, "ws2_32")

FILE* fileOpen(uint8_t* fileNm);
void fileWrite(uint8_t* file, uint32_t fileLength, FILE* newFile);
void unix_time_to_filetime(time_t t, FILETIME* ptf);
void convertToWideChar(const char* src, wchar_t* dest, size_t dest_size);
void updateMetaData(FILETIME accessTime, FILETIME modifyTime, char* fileNm);