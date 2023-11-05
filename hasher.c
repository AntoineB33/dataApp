#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h> // You'll need to install OpenSSL for hashing functions

// Function to generate a SHA-256 hash of a string
void sha256(const char *str, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, strlen(str));
    SHA256_Final(hash, &sha256);
}

int main() {
    // Unique URL
    const char *url = "https://www.youtube.com/watch?v=12spXYRwWNs";
    
    // Generate a unique identifier for the URL by hashing it
    unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256(url, hash);
    
    // Convert the hash to a string representation
    char hash_string[2 * SHA256_DIGEST_LENGTH + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hash_string[i * 2], "%02x", hash[i]);
    }
    hash_string[2 * SHA256_DIGEST_LENGTH] = '\0';
    
    // Create a unique file name based on the hash
    char file_name[256];
    snprintf(file_name, sizeof(file_name), "file_%s.txt", hash_string);

    // Output the unique file name
    printf("URL: %s\n", url);
    printf("File Name: %s\n", file_name);

    // Here, you can use the 'file_name' to save or access the file associated with the URL.

    return 0;
}