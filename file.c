#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// Error reporting function
int print_error(char *path, int errnum) {
    return fprintf(stdout, "%s: cannot determine (%s)\n", path, strerror(errnum));  // Standard error message
}

// Check if the file is empty
int is_empty(FILE *file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size == 0;
}

// Check if the file contains only ASCII text
int is_ascii(FILE *file) {
    unsigned char byte;
    while (fread(&byte, 1, 1, file) == 1) {
        if ((byte < 0x07 || byte > 0x7E) && byte != 0x0A && byte != 0x0D) {
            return 0;  // Non-ASCII character found
        }
    }
    return 1;
}

// Check if the file contains valid ISO-8859-1 characters
int is_iso_8859_1(FILE *file) {
    unsigned char byte;
    while (fread(&byte, 1, 1, file) == 1) {
        if ((byte < 0x07 || byte > 0x7E) && (byte < 0xA0 || byte > 0xFF) && byte != 0x0A && byte != 0x0D) {
            return 0;  // Non-ISO-8859-1 character found
        }
    }
    return 1;
}

// Check if the file contains valid UTF-8 characters
int is_utf8(FILE *file) {
    unsigned char byte;
    int continuation_bytes = 0;

    while (fread(&byte, 1, 1, file) == 1) {
        if (continuation_bytes == 0) {
            // Determine how many continuation bytes to expect
            if (byte >= 0x80) {
                if (byte >= 0xC2 && byte <= 0xDF) {
                    continuation_bytes = 1;  // 2-byte sequence
                } else if (byte >= 0xE0 && byte <= 0xEF) {
                    continuation_bytes = 2;  // 3-byte sequence
                } else if (byte >= 0xF0 && byte <= 0xF4) {
                    continuation_bytes = 3;  // 4-byte sequence
                } else {
                    return 0;  // Invalid UTF-8 sequence
                }
            }
        } else {
            // Check if it's a valid continuation byte (10xxxxxx)
            if ((byte & 0xC0) != 0x80) {
                return 0;  // Invalid continuation byte
            }
            continuation_bytes--;
        }
    }

    return continuation_bytes == 0;  // Return true if all sequences are valid
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        // Call print_error if the file cannot be opened
        print_error(argv[1], errno);
        return EXIT_SUCCESS;
    }

    // Check if the file is empty
    if (is_empty(file)) {
        printf("%s: empty\n", argv[1]);  // Print "empty"
        fclose(file);
        return EXIT_SUCCESS;
    }

    // Rewind file for further checks
    rewind(file);

    // Check if the file is ASCII
    if (is_ascii(file)) {
        printf("%s: ASCII text\n", argv[1]);  // Print "ASCII text"
        fclose(file);
        return EXIT_SUCCESS;
    }

    // Rewind file for further checks
    rewind(file);

    // Check if the file is ISO-8859-1
    if (is_iso_8859_1(file)) {
        printf("%s: ISO-8859 text\n", argv[1]);  // Print "ISO-8859 text"
        fclose(file);
        return EXIT_SUCCESS;
    }

    // Rewind file for further checks
    rewind(file);

    // Check if the file is valid UTF-8
    if (is_utf8(file)) {
        printf("%s: UTF-8 Unicode text\n", argv[1]);  // Print "UTF-8 Unicode text"
    } else {
        // If none of the encodings matched, output "data"
        printf("%s: data\n", argv[1]);  // Print "data"
    }

    fclose(file);
    return EXIT_SUCCESS;
}







