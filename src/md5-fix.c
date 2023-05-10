#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/md5.h>

#define SAFELOADER_PREAMBLE_SIZE	0x14

/**
   Salt for the MD5 hash

   Fortunately, TP-LINK seems to use the same salt for most devices which use
   the new image format.
*/
static const uint8_t md5_salt[16] = {
        0x7a, 0x2b, 0x15, 0xed,
        0x9b, 0x98, 0x59, 0x6d,
        0xe5, 0x04, 0xab, 0x44,
        0xac, 0x2a, 0x9f, 0x4e,
};

static void put_md5(uint8_t *md5, uint8_t *buffer, unsigned int len) {
        MD5_CTX ctx;

        MD5_Init(&ctx);
        MD5_Update(&ctx, md5_salt, (unsigned int)sizeof(md5_salt));
        MD5_Update(&ctx, buffer, len);
        MD5_Final(md5, &ctx);
}

/** Stores a uint32 as big endian */
static inline void put32(uint8_t *buf, uint32_t val) {
        buf[0] = val >> 24;
        buf[1] = val >> 16;
        buf[2] = val >> 8;
        buf[3] = val;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <firmware>\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("File not exists!\nUsage: %s <firmware>\n", argv[0]);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    u_int32_t file_size = ftell(fp);
    uint8_t *image = malloc(file_size);
    fseek(fp, 0, SEEK_SET);
    fread(image, file_size, 1, fp);
    fclose(fp);

    // write file length to the first 4 byte in network byte order
    put32(image, file_size);

    // write md5
    put_md5(image + 0x04, image + SAFELOADER_PREAMBLE_SIZE, file_size - SAFELOADER_PREAMBLE_SIZE);
    fp = fopen(argv[1], "w");
    if (fp == NULL) {
        printf("File not writable!\n");
        return 1;
    }

    fwrite(image, file_size, 1, fp);

    return 0;
}
