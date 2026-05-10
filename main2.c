#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lodepng.h"

unsigned char* load_image(const char* filename, unsigned int* width, unsigned int* height) {
    unsigned char* image = NULL;
    if (lodepng_decode32_file(&image, width, height, filename)) {
        return NULL;
    }
    return image;
}

void to_grayscale(unsigned char* rgba, unsigned char* gray, int pixels) {
    for (int i = 0; i < pixels; i++) {
        int r = rgba[i * 4 + 0];
        int g = rgba[i * 4 + 1];
        int b = rgba[i * 4 + 2];
        gray[i] = (r + g + b) / 3;
    }
}

void binarize(unsigned char* gray, unsigned char* bw, int pixels, int threshold) {
    for (int i = 0; i < pixels; i++) {
        bw[i] = (gray[i] > threshold) ? 255 : 0;
    }
}

void remove_noise(unsigned char* image, int width, int height) {
    int size = width * height;
    unsigned char* temp = (unsigned char*)malloc(size);
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            int all_white = 1;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (image[(y + dy) * width + (x + dx)] == 0) {
                        all_white = 0;
                    }
                }
            }
            temp[idx] = all_white ? 255 : 0;
        }
    }
    
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            int any_white = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (temp[(y + dy) * width + (x + dx)] == 255) {
                        any_white = 1;
                    }
                }
            }
            image[idx] = any_white ? 255 : 0;
        }
    }
    
    free(temp);
}

int count_clusters(unsigned char* image, unsigned char* visited, int width, int height, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    int idx = y * width + x;
    if (visited[idx]) return 0;
    if (image[idx] == 0) return 0;
    
    visited[idx] = 1;
    
    count_clusters(image, visited, width, height, x + 1, y);
    count_clusters(image, visited, width, height, x - 1, y);
    count_clusters(image, visited, width, height, x, y + 1);
    count_clusters(image, visited, width, height, x, y - 1);
    
    return 1;
}

int main() {
    const char* filename = "IMG_B478180424DF-1.png";
    unsigned int width, height;
    unsigned char* image_rgba = load_image(filename, &width, &height);
    if (!image_rgba) {
        printf("0\n");
        return 1;
    }
    
    int pixels = width * height;
    
    unsigned char* grayscale = (unsigned char*)malloc(pixels);
    unsigned char* binary = (unsigned char*)malloc(pixels);
    unsigned char* visited = (unsigned char*)calloc(pixels, 1);
    
    to_grayscale(image_rgba, grayscale, pixels);
    
    int threshold = 200;
    binarize(grayscale, binary, pixels, threshold);
    
    remove_noise(binary, width, height);
    
    int tanker_count = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            if (binary[idx] == 255 && !visited[idx]) {
                tanker_count += count_clusters(binary, visited, width, height, x, y);
            }
        }
    }
    
    printf("%d\n", tanker_count);
    
    free(image_rgba);
    free(grayscale);
    free(binary);
    free(visited);
    
    return 0;
}
