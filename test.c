#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

#define MAX_HASH_STRING_LENGTH (2 * EVP_MAX_MD_SIZE + 1)

char *sha256(const char *str) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned int md_len;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    static char hash_string[MAX_HASH_STRING_LENGTH]; // Static allocation for simplicity

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("sha256");

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str, strlen(str));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    EVP_cleanup();

    // Convert the hash bytes to a string representation
    for (unsigned int i = 0; i < md_len; i++)
        sprintf(&hash_string[i * 2], "%02x", md_value[i]);

    hash_string[2 * md_len] = '\0'; // Null-terminate the string

    return hash_string;
}

int main() {
    const char *data = "https://www.youtube.com/watch?v=ufszyK8X5g8";
    char *hash;

    hash = sha256(data);

    printf("SHA256 Hash: %s\n", hash);

    return 0;
}
