#include "file.h"

FILE* fileOpen(ProtocolFile file) {
    FILE* newFile = fopen(file.fileName, "wb");
    if (!newFile) {
        perror("fopen failed");
        return NULL;
    }
    return newFile;
}

void fileWrite(ProtocolFile file, FILE* newFile) {
    size_t written = fwrite(file.myFile, 1, file.fileLength, newFile);
    if (written != file.fileLength) {
        perror("fwrite failed");
        fclose(newFile);
        return 1;
    }
}

void unix_time_to_filetime(time_t t, FILETIME* ptf) {
    // 100나노초 단위로 변환: 1초 = 10000000 100나노초
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    ptf->dwLowDateTime = (DWORD)ll;          // 하위 32비트
    ptf->dwHighDateTime = (DWORD)(ll >> 32); // 상위 32비트
}

void convertToWideChar(const char* src, wchar_t* dest, size_t dest_size) {
	MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, (int)dest_size);
}

void updateMetaData(FILETIME ft_access, FILETIME ft_modify, char* fileNm) {
	wchar_t wide_filedata[256];
	convertToWideChar(fileNm, wide_filedata, sizeof(wide_filedata) / sizeof(wchar_t));

	HANDLE file = CreateFile(wide_filedata, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE) {
		printf("Error opening file: %lu\n", GetLastError());
		return 1;
	}

	if (!SetFileTime(file, NULL, &ft_access, &ft_modify)) {
		printf("Error setting file time: %lu\n", GetLastError());
		CloseHandle(file);
		return 1;
	}

	printf("File timestamps updated successfully.\n");
	CloseHandle(file);
}