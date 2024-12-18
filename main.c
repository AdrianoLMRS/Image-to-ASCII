#include <opencv2/opencv.hpp>   // For image manipulation
#include <stdio.h>              // Default lib for input/output
#include <stdlib.h>             // For basic functions

#define WIDTH_SCALE 10  // Width factor for scaling (characters are more long and taller)
#define HEIGHT_SCALE 10 // Height factor for scaling (characters are more long and taller)
#define ASCII_CHARS " .:-=+*#%@" // Permited ASCII characters

// Fuction for maping intensity for ASCII characters
// The value can be between 0 and 255
char intensityToASCII(int intensity) {
    int len = strlen(ASCII_CHARS);
    return ASCII_CHARS[(intensity * (len - 1)) / 255];
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
    if (argc != 2) {
        printf("Uso: %s <caminho_para_imagem>\n", argv[0]);
        return -1;
    }

    // Loads image in gray scale format
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        printf("Erro ao carregar a imagem.\n");
        return -1;
    }

    // Image dimensions
    int rows = image.rows / HEIGHT_SCALE; // Reduce height to adjust proportion
    int cols = image.cols / WIDTH_SCALE;  // Reduce width to adjust proportion

    // Resize image for better result
    cv::resize(image, image, cv::Size(cols, rows), 0, 0, cv::INTER_LINEAR);

    // Create a .txt dile as output
    FILE* file = fopen("output.txt", "w");
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
            char asciiChar = intensityToASCII(intensity); // Convert intensity to ASCII
            fprintf(file, "%c", asciiChar); // Writes pixels in the output file
            printf("%c", asciiChar); // Writes pixels in the terminal
        }
        // Adds breakline in the final of each row
        fprintf(file, "\n");
        printf("\n");
    }

    fclose(file); // Closes file
    printf("Conversão concluída! Resultado salvo em 'output.txt'\n"); // Success log

    return 0; // Success
}
