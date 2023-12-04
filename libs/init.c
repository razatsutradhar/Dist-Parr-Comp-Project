#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define VALUES_PER_LINE 100

void getValuesFromLine(const char *file_path, int line_number, int *values) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int current_line = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (current_line == line_number - 1) {
            char *token = strtok(line, ",");
            int i = 0;
            while (token != NULL && i < VALUES_PER_LINE) {
                values[i++] = atoi(token);
                token = strtok(NULL, ",");
            }
            break;
        }
        current_line++;
    }

    fclose(file);
}

int main() {
    const char *file_path = "../data/data.txt"; // Replace "your_file.txt" with your file name
    int line_number = 5; // Replace with the line number you want to extract
    int extracted_values[VALUES_PER_LINE];

    getValuesFromLine(file_path, line_number, extracted_values);

    printf("Values in line %d:\n", line_number);
    for (int i = 0; i < VALUES_PER_LINE; i++) {
        printf("%d ", extracted_values[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}