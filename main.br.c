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

// Function to convert RGB values to an ANSI escape code for text foreground color
char* rgbToAnsiColor(cv::Vec3b pixel) {
    static char colorCode[20];
    // Convert RGB to an ANSI escape code for foreground color (e.g., 38;2;R;G;B)
    snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", pixel[2], pixel[1], pixel[0]);
    return colorCode;  // Return the ANSI escape code for the color
}

// Function to see if format is correct for colored image
int endsWithAllowedFormat(const char* path) {
    // Allowed extensions
    const char* allowedFormats[] = {".png", ".jpeg", ".webp"};
    int numFormats = 3;  // Número de formatos permitidos

    for (int i = 0; i < numFormats; i++) {
        int len = strlen(allowedFormats[i]);
        int pathLen = strlen(path);
        if (pathLen >= len && strcmp(path + pathLen - len, allowedFormats[i]) == 0) {
            return 1;  // If is allowed return 1
        }
    }

    return 0;  // If not allowed return 0
}

// Display help if --help flag
void helpFlag(const char* programName) {
    printf("Manual de Uso - Conversor de Imagens para ASCII\n");
    printf("\nUso: %s <caminho_para_imagem> [opções]\n", programName);
    printf("\nOpções:\n");
    printf("  --help             Exibe este manual de uso.\n");
    printf("  --default          Usa os valores padrão sem solicitar entrada do usuário.\n");
    printf("\nEntradas do Usuário:\n");
    printf("  - Preferência de cor: Digite 1 para usar cor, ou outro número para preto e branco.\n");
    printf("  - Caminho para a imagem: O arquivo de entrada deve ser uma imagem válida.\n");
    printf("  - Escala de largura e altura: Fatores de escala para ajustar a proporção.\n");
    printf("  - Caracteres ASCII: Conjunto de caracteres para representar a imagem.\n");
    printf("\nExemplo de Uso:\n");
    printf("  %s imagem.png\n", programName);
    printf("  %s imagem.png --default\n", programName);
    printf("  %s imagem.png --help\n", programName);
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
    cv::setNumThreads(1);

    // Display help manual if --help is passed
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        helpFlag(argv[0]); // Display help manual in terminal
        return 0; // Exit after displaying help
    }

    int widthScale = DEFAULT_WIDTH_SCALE;     // Default
    int heightScale = DEFAULT_HEIGHT_SCALE;   // Default
    int colorChoice = 0;                      // Default
    char asciiChars[100];                     // Array to store user-defined ASCII characters (100 MAX)
    char userPath[200];                       // Array to store user input output path
    char outputPath[300];                     // Array to store user-defined output path
    strcpy(asciiChars, DEFAULT_ASCII_CHARS);  // Initialize with default value
    strcpy(outputPath, DEFAULT_OUTPUT_PATH);  // Initialize with default value

    // If no path || invalid
    if (argc < 2) {
        printf("Uso: %s <caminho_para_imagem> [opções]\n", argv[0]);
        printf("\n\nDigite --help para mais informações.");
        return -1;
    }

    // Check if the user provided the --default flag
    bool useDefaults = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--default") == 0) {
            useDefaults = true;
            break;
        }
    }
    
    char input[10];  // Array to hold user input
    if (useDefaults) {
        // Use default values, no need to prompt the user
        printf("Usando valores padrão...\n");
    } else {
        // IF THE --DEFAULT FLAG IS NOT USED - User inputs
        // Ask the user for the color preference
        printf("Digite 1 para transformar com cor ou algo diferente continuar sem cor: ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // If input is empty (i.e., just Enter)
            if (input[0] == '\n') {
                colorChoice = 0;  // Default to 0 if input is empty
            } else {
                // Try to convert the input to an integer
                if (sscanf(input, "%d", &colorChoice) != 1) {
                    colorChoice = 0;  // Set to 0 if conversion fails
                }
            }
        }
        if (colorChoice != 1) {
            // Ask the user for the output file path
            printf("Digite o caminho e nome do arquivo de saída (padrão: \"%s\"): ", DEFAULT_OUTPUT_PATH);
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
        } else {
            // Solicita o caminho do arquivo
            printf("Digite o caminho do arquivo (com a extensão): ");
            scanf("%s", userPath);

            // Verifica se o colorChoice é 1 e se o userPath não termina com um formato permitido
            if (colorChoice == 1 && !endsWithAllowedFormat(userPath)) {
                // Se não terminar com .png, .jpeg ou .webp, coloca .png
                strcat(userPath, ".png");
                printf("O arquivo será salvo como: %s\n", userPath);
            } else {
                printf("O arquivo será salvo como: %s\n", userPath);
            }
            strcpy(outputPath, userPath);
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
    }

    // Loads image in gray scale format
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (image.empty()) {
        printf("Erro ao carregar a imagem.\n");
        return -1;
    }

    // Image dimensions
    int rows = image.rows / heightScale; // Reduce height to adjust proportion
    int cols = image.cols / widthScale;  // Reduce width to adjust proportion

    // Resize image for better result
    cv::resize(image, image, cv::Size(cols, rows), 0, 0, cv::INTER_LINEAR);

    // Create a .txt file as output
    FILE* file = fopen(outputPath, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return -1;
    }

    // Convert image to ASCII version
    // Two for loops for rows & columns
    // The if statement is for color conversion
    if (colorChoice == 1) {
        cv::Mat output(rows * heightScale, cols * widthScale, CV_8UC3, cv::Scalar(0, 0, 0)); // Canvas

        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(i, j); // RGB pixel
                char asciiChar = intensityToASCII(image.at<uchar>(i, j), asciiChars); // Grayscale intensity

                // Draw ASCII character on the image
                cv::putText(
                    output,
                    std::string(1, asciiChar),  // Single ASCII character as string
                    cv::Point(j * widthScale, i * heightScale),  // Position
                    cv::FONT_HERSHEY_SIMPLEX,  // Font type
                    0.5,                       // Font scale
                    cv::Scalar(pixel[0], pixel[1], pixel[2]),  // Color (BGR)
                    1                          // Thickness
                );
            }
        }

        // Save the rendered image
        cv::imwrite(outputPath, output);
        printf("Imagem gerada e salva como: %s\n", outputPath);

    } else {
        // Convert to grayscale
        cv::Mat grayImage;
        if (image.channels() > 1) {
            cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        } else {
            grayImage = image;
        }

        // Process each pixel
        for (int i = 0; i < grayImage.rows; i++) {
            for (int j = 0; j < grayImage.cols; j++) {
                int intensity = grayImage.at<uchar>(i, j);  // Intensity (0 - 255)
                char asciiChar = intensityToASCII(intensity, asciiChars);  // Convert to ASCII
                fprintf(file, "%c", asciiChar); // Write in file
                printf("%c", asciiChar);       // Show in terminal
            }
            fprintf(file, "\n");  // File breakline at the end of each row
            printf("\n");         // Terminal breakline at the end of each row
        }
    }

    fclose(file); // Closes file
    printf("Conversão concluída! Resultado salvo em '%s'\n", outputPath); // Success log

    return 0; // Success
}