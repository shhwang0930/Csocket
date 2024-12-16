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