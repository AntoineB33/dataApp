#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/file.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>

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
    char* url;

    size_t size = 0;
    FILE *clipboard = popen("powershell.exe Get-Clipboard", "r");
    if (clipboard == NULL) {
        pclose(clipboard);
        printf("Failed to open clipboard.\n");
        getline(&url, &size, stdin);
        return -1;
    }
    if (getline(&url, &size, clipboard) == -1) {
        pclose(clipboard);
        printf("Error reading clipboard.\n");
        getline(&url, &size, stdin);
        return -1;
    }
    pclose(clipboard);
    // url = strtok(firstLine, "\n");
    char* prefix = "https";
    if(strncmp(url, prefix, strlen(prefix)) != 0) {
        printf("Not starting with %s.\n",prefix);
        getline(&url, &size, stdin);
        return -1;
    }
    url[strlen(url)-2] = '\0';
    char *hash = sha256(url);
    
    clipboard = popen("clip.exe", "w");
    if (clipboard == NULL) {
        printf("Error opening clipboard");
        getline(&url, &size, stdin);
        return -1;
    }
    fprintf(clipboard, "%s", hash);
    pclose(clipboard);
}