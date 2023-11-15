#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>


FILE* clipboard;
char* firstLine = NULL;


void freeVar(void *var) {
    if(var != NULL) {
        free(var);
    }
}

void freeList(char **var, int len) {
    if(var != NULL) {
        for(int i = 0; i<len; i++) {
            free(var[i]);
        }
        free(var);
    }
}

void freeAll(char* msg) {
    freeVar(firstLine);
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

void hash_url(const char *url, unsigned char *hash) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned int md_len;

    OpenSSL_add_all_digests();
    md = EVP_sha256();
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, url, strlen(url));
    EVP_DigestFinal_ex(mdctx, hash, &md_len);
    EVP_MD_CTX_free(mdctx);
}

int main() {



    size_t size = 0;
    clipboard = popen("powershell.exe Get-Clipboard", "r");
    if (clipboard == NULL) {
        pclose(clipboard);
        freeAll("Failed to open clipboard.\n");
    }
    if (getline(&firstLine, &size, clipboard) == -1) {
        pclose(clipboard);
        freeAll("Error reading clipboard.\n");
    }
    pclose(clipboard);
    printf("%s\n", firstLine);

    
    unsigned char hash[EVP_MAX_MD_SIZE];
    hash_url(firstLine, hash);

    // Store the hash in a variable
    char storedHash[EVP_MAX_MD_SIZE * 2 + 1];  // Twice the size for hexadecimal representation + 1 for null terminator

    for (unsigned int i = 0; i < EVP_MD_size(EVP_sha256()); i++) {
        sprintf(storedHash + 2 * i, "%02x", hash[i]);
    }
    printf("%s\n", storedHash);

    
    clipboard = popen("clip.exe", "w");
    if (clipboard == NULL) {
        freeAll("Error opening clipboard");
    }

    // Write the text to the clipboard
    fprintf(clipboard, "%s", storedHash);

    // Close the pipe
    if (pclose(clipboard) != 0) {
        freeAll("Error closing clipboard");
    }
    

    freeAll("Success");
}
