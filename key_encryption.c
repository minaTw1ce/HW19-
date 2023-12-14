#include <stdio.h>
#include <string.h>

void encryptMessage(char *message, const char *encodingKey) {
    int length = strlen(message);

    for (int i = 0; i < length; i++) {
        if (message[i] != ' ') {  // Ignore spaces
            // Apply replacement encoding
            char currentChar = message[i];
            char *replacement = strchr(encodingKey, currentChar);
            
            // Replace if found in the encoding key
            if (replacement != NULL) {
                int index = replacement - encodingKey;
                message[i] = 'A' + index;  // Assuming uppercase letters only
            }
        }
    }
}

int main() {
    char message[] = "HELLO WORLD";
    const char encodingKey[] = "ZYXWVUTSRQPONMLKJIHGFEDCBA";  // Example encoding key

    printf("Original message: %s\n", message);

    encryptMessage(message, encodingKey);

    printf("Encrypted message: %s\n", message);

    return 0;
}
