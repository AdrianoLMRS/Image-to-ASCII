#include <opencv2/opencv.hpp>   // For image manipulation
#include <stdio.h>              // Default lib for input/output
#include <stdlib.h>             // For basic functions
#include <string.h>             // For string manipulation

// * Default values
#define DEFAULT_WIDTH_SCALE 10            // Width factor for scaling (characters are more long and taller)
#define DEFAULT_HEIGHT_SCALE 10           // Height factor for scaling (characters are more long and taller)
#define DEFAULT_ASCII_CHARS " .:-=+*#%@"  // Permited ASCII characters
#define DEFAULT_OUTPUT_PATH "output.txt"  // Default output file name and path

// Fuction for maping intensity for ASCII characters
// The value can be between 0 and 255
char intensityToASCII(int intensity, const char* asciiChars) {
    int len = strlen(asciiChars);
    return asciiChars[(intensity * (len - 1)) / 255];
}

/**
 * @brief Main function for the image to ASCII conversion program.
 *
 * This function takes an image file as input, converts it to ASCII art, and saves the result to a text file.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments. The first argument is the program name, and the second argument is the path to the image file.
 *
 * @return 0 if the program runs successfully, -1 if an error occurs.
 */
int main(int argc, char** argv) {
    int widthScale = DEFAULT_WIDTH_SCALE;    // Default
    int heightScale = DEFAULT_HEIGHT_SCALE;  // Default
    char asciiChars[100];                    // Array to store user-defined ASCII characters (100 MAX)
    char outputPath[300];                    // Array to store user-defined output path
    strcpy(asciiChars, DEFAULT_ASCII_CHARS); // Initialize with default value

    if (argc != 2) {
        printf("Uso: %s <caminho_para_imagem>\n", argv[0]);
        return -1;
    }

    // Ask the user for the output file path
    printf("Digite o caminho e nome do arquivo de saída (padrão: \"%s\"): ", DEFAULT_OUTPUT_PATH);
    char userPath[200];
    if (fgets(userPath, sizeof(userPath), stdin) != NULL) {
        if (userPath[0] == '\n') {
            strcpy(userPath, DEFAULT_OUTPUT_PATH); // Use default if input is empty
        } else {
            userPath[strcspn(userPath, "\n")] = 0;  // Remove the newline at the end
        }
    }

    // If user only provided a path but no filename, use "output.txt" as filename
    if (userPath[strlen(userPath) - 1] == '/' || userPath[strlen(userPath) - 1] == '\\') {
        // Check if the length of the path + "output.txt" fits into the buffer
        if (strlen(userPath) + strlen("output.txt") < sizeof(outputPath)) {
            snprintf(outputPath, sizeof(outputPath), "%soutput.txt", userPath);  // Add "output.txt" to the given path
        } else {
            printf("Erro: O caminho é muito longo para o arquivo de saída.\n");
            return -1; // If the path is too long
        }
    } else {
        // If the user specified a file name but no extension, add ".txt"
        if (strstr(userPath, ".txt") == NULL) {
            // Check if the user path fits within the buffer when adding ".txt"
            if (strlen(userPath) + strlen(".txt") < sizeof(outputPath)) {
                snprintf(outputPath, sizeof(outputPath), "%s.txt", userPath);  // Add ".txt" extension
            } else {
                printf("Erro: O caminho é muito longo para o arquivo de saída.\n");
                return -1; // If the path is too long
            }
        } else {
            strncpy(outputPath, userPath, sizeof(outputPath) - 1);
            outputPath[sizeof(outputPath) - 1] = '\0';  // Ensure null termination
        }
    }

    // Ask the user for the width scale
    printf("Digite o fator de escala para a largura (padrão: %d): ", DEFAULT_WIDTH_SCALE);
    char input[10];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] != '\n') {  // If the user didn't just press Enter
            if (sscanf(input, "%d", &widthScale) != 1) {
                widthScale = DEFAULT_WIDTH_SCALE; // Use default if invalid input
            }
        }
    }

    // Ask the user for the height scale
    printf("Digite o fator de escala para a altura (padrão: %d): ", DEFAULT_HEIGHT_SCALE);
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (input[0] != '\n') {  // If the user didn't just press Enter
            if (sscanf(input, "%d", &heightScale) != 1) {
                heightScale = DEFAULT_HEIGHT_SCALE; // Use default if invalid input
            }
        }
    }

    // Ask the user for ASCII characters
    printf("Digite os caracteres ASCII para usar (padrão: \"%s\"): ", DEFAULT_ASCII_CHARS);
    if (fgets(asciiChars, sizeof(asciiChars), stdin) != NULL) {
        if (asciiChars[0] == '\n') {
            strcpy(asciiChars, DEFAULT_ASCII_CHARS); // Use default if input is empty
        } else {
            asciiChars[strcspn(asciiChars, "\n")] = 0;  // Remove the newline at the end
        }
    }

    // Loads image in gray scale format
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        printf("Erro ao carregar a imagem.\n");
        return -1;
    }

    // Image dimensions
    int rows = image.rows / heightScale; // Reduce height to adjust proportion
    int cols = image.cols / widthScale;  // Reduce width to adjust proportion

    // Resize image for better result
    cv::resize(image, image, cv::Size(cols, rows), 0, 0, cv::INTER_LINEAR);

    // Create a .txt dile as output
    FILE* file = fopen(outputPath, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return -1;
    }

    // Convert image to ASCII version
    // Two for loops for rows & columns
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            // Maps intensity of each pixel, convert to ASCII and writes on the output file
            int intensity = image.at<uchar>(i, j);  // Takes each value of pixels (0-255)
            char asciiChar = intensityToASCII(intensity, asciiChars); // Convert intensity to ASCII
            fprintf(file, "%c", asciiChar); // Writes pixels in the output file
            printf("%c", asciiChar); // Writes pixels in the terminal
        }
        // Adds breakline in the final of each row
        fprintf(file, "\n");
        printf("\n");
    }

    fclose(file); // Closes file
    printf("Conversão concluída! Resultado salvo em '%s'\n", outputPath); // Success log

    return 0; // Success
}
