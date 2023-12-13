#include <stdio.h>
#include <string.h>

void encryptMessage(char *message, const char *encodingString) {
    int length = strlen(message);
    int encodingLength = strlen(encodingString);

    for (int i = 0; i < length; i++) {
        if (message[i] != ' ') {  // Ignore spaces
            // Apply replacement encoding
            int index = message[i] - 'A';  // Assuming uppercase letters only
            if (index >= 0 && index < encodingLength) {
                message[i] = encodingString[index];
            }
        }
    }
}

int main() {
    char message[] = "HELLO WORLD";
    const char encodingString[] = "XYZABCDEFGHIJKLMNOPQRSTUVW";  // Example encoding string

    printf("Original message: %s\n", message);

    encryptMessage(message, encodingString);

    printf("Encrypted message: %s\n", message);

    return 0;
}
