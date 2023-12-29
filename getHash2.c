#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <wincrypt.h>

#define MAX_HASH_STRING_LENGTH (2 * SHA256_DIGEST_LENGTH + 1)

char *sha256(const char *str) {
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    BYTE rgbHash[SHA256_DIGEST_LENGTH];
    DWORD cbHash = SHA256_DIGEST_LENGTH;
    static char hash_string[MAX_HASH_STRING_LENGTH];

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        printf("Error during CryptAcquireContext\n");
        return NULL;
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        printf("Error during CryptCreateHash\n");
        return NULL;
    }

    if (!CryptHashData(hHash, (const BYTE *)str, strlen(str), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        printf("Error during CryptHashData\n");
        return NULL;
    }

    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        printf("Error during CryptGetHashParam\n");
        return NULL;
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    for (DWORD i = 0; i < cbHash; i++)
        sprintf(&hash_string[i * 2], "%02x", rgbHash[i]);

    hash_string[2 * cbHash] = '\0';
    return hash_string;
}

int main() {
    char url[2048]; // Define the buffer size for the URL
    size_t size = sizeof(url);
    char* prefix = "https";

    if (!OpenClipboard(NULL)) {
        printf("Error opening clipboard.\n");
        return -1;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        CloseClipboard();
        printf("Error getting clipboard data.\n");
        return -1;
    }

    char* clipboardData = (char*)GlobalLock(hData);
    if (clipboardData == NULL) {
        CloseClipboard();
        printf("Error locking clipboard data.\n");
        return -1;
    }

    strncpy(url, clipboardData, size);
    GlobalUnlock(hData);
    CloseClipboard();

    if (strncmp(url, prefix, strlen(prefix)) != 0) {
        printf("URL does not start with %s.\n", prefix);
        return -1;
    }

    url[strlen(url) - 2] = '\0';
    char *hash = sha256(url);

    if (hash == NULL) {
        printf("Error generating hash.\n");
        return -1;
    }

    if (!OpenClipboard(NULL)) {
        printf("Error opening clipboard.\n");
        return -1;
    }

    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(hash) + 1);
    if (hMem == NULL) {
        CloseClipboard();
        printf("Error allocating memory for clipboard.\n");
        return -1;
    }

    char* clipboardDataOut = (char*)GlobalLock(hMem);
    if (clipboardDataOut == NULL) {
        CloseClipboard();
        printf("Error locking memory for clipboard.\n");
        return -1;
    }

    strcpy(clipboardDataOut, hash);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    return 0;
}
