#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ALPHABET_SIZE 26

char encode_key[ALPHABET_SIZE] = "QWERTYUIOPASDFGHJKLZXCVBNM";

void encode(char *message);
void decode(char *message);
void processInputFile(FILE *input, FILE *output, void (*operation)(char *));
void removeSpacesAndPunctuation(char *str);

int main() {
    char choice;
    char message[1000];

    printf("Do you want to encode (E) or decode (D) a message? ");
    scanf(" %c", &choice);

    printf("Is the message in a text file? (Y/N) ");
    scanf(" %c", &choice);

    if (toupper(choice) == 'Y') {
        char inputFileName[100], outputFileName[100];

        printf("Enter the name of the input file: ");
        scanf("%s", inputFileName);

        printf("Enter the name of the output file: ");
        scanf("%s", outputFileName);

        FILE *inputFile = fopen(inputFileName, "r");
        FILE *outputFile = fopen(outputFileName, "w");

        if (inputFile == NULL || outputFile == NULL) {
            perror("Error opening files");
            return 1;
        }

        if (toupper(choice) == 'E') {
            processInputFile(inputFile, outputFile, encode);
        } else if (toupper(choice) == 'D') {
            processInputFile(inputFile, outputFile, decode);
        }

        fclose(inputFile);
        fclose(outputFile);
    } else {
        printf("Enter the message: ");
        scanf(" %[^\n]s", message);

        if (toupper(choice) == 'E') {
            encode(message);
            printf("Encoded message: %s\n", message);
        } else if (toupper(choice) == 'D') {
            decode(message);
            printf("Decoded message: %s\n", message);
        }
    }

    return 0;
}

void encode(char *message) {
    removeSpacesAndPunctuation(message);

    for (int i = 0; message[i] != '\0'; i++) {
        if (isalpha(message[i])) {
            int offset = toupper(message[i]) - 'A';
            message[i] = encode_key[offset];
        }
    }
}

void decode(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (isalpha(message[i])) {
            int offset = toupper(message[i]) - 'A';
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                if (encode_key[j] == toupper(message[i])) {
                    message[i] = 'A' + j;
                    break;
                }
            }
        }
    }
}

void processInputFile(FILE *input, FILE *output, void (*operation)(char *)) {
    char message[1000];
    while (fgets(message, sizeof(message), input) != NULL) {
        operation(message);
        fprintf(output, "%s", message);
    }
}

void removeSpacesAndPunctuation(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace(str[i]) || ispunct(str[i])) {
            for (int j = i; str[j] != '\0'; j++) {
                str[j] = str[j + 1];
            }
            i--; // Decrement i to reprocess the current position after removal
        }
    }
}
