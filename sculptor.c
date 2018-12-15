#include "sculptor.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lodepng.h"

#ifdef PERF
#define NUM_SEAM_ROWS(height) (height)
#define SEAM_PREV_ROW(row) (row - 1)
#define SEAM_CURRENT_ROW(row) (row)
#define SEAM_LAST_ROW(image) (image->height - 1)
#else
#define NUM_SEAM_ROWS(height) 2
#define SEAM_PREV_ROW(row) 0
#define SEAM_CURRENT_ROW(row) 1
#define SEAM_LAST_ROW(image) 1
#endif

int main(int argc, char **argv) {
    // Verify and extract arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: ./sculptor input output seams\n");
        exit(1);
    }
    char *input_name = argv[1];
    char *output_name = argv[2];
    int to_carve = atoi(argv[3]);
    assert(to_carve >= 0);

    clock_t start = clock();

    // Load input image
    im_t *image = image_load(input_name);

    uint16_t original_width = image->width;
    uint16_t original_height = image->height;
    assert((original_width > 1) && (original_height > 1));

    if (to_carve >= original_width) {
        fprintf(stderr, "Number of seams to carve too large\n");
        exit(1);
    }

    double **energies = energies_init(original_width, original_height);
    seam_t **seams = seams_init(original_width, original_height);

    for (int iteration = 0; iteration < to_carve; iteration++) {
        clock_t energy_start = clock();
        calculate_energies(energies, image);
        clock_t energy_end = clock();
        printf("Energy time consumed: %ld ms\n", (energy_end - energy_start) * 1000 / CLOCKS_PER_SEC);

        clock_t seams_start = clock();
        fill_seams(seams, energies, image);
        clock_t seams_end = clock();
        printf("Seams time consumed: %ld ms\n", (seams_end - seams_start) * 1000 / CLOCKS_PER_SEC);

        seam_t *lowest_energy_seam = &seams[SEAM_LAST_ROW(image)][0];
        for (uint16_t col = 1; col < image->width; col++) {
            seam_t *seam = &seams[SEAM_LAST_ROW(image)][col];
            if (seam->energy < lowest_energy_seam->energy) lowest_energy_seam = seam;
        }

        remove_seam(image, lowest_energy_seam);
    }

    // Save output image
    image_save(image, output_name);

    // Cleanup
    energies_destroy(energies, original_height);
    seams_destroy(seams, original_width, original_height);
    image_destroy(image, original_height);

    clock_t end = clock();
    printf("Time consumed: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);

    return 0;
}

im_t *image_load(char *filename) {
    unsigned char *input;
    unsigned int width, height;

    if (lodepng_decode24_file(&input, &width, &height, filename) != 0) {
        fprintf(stderr, "Error loading input image\n");
        free(input);
        exit(1);
    }

    uint16_t swidth = (uint16_t)width, sheight = (uint16_t)height;
    assert((swidth == width) && (sheight == height));

    // Convert pixel data to 2d arrays
    rgb_pixel_t **pixels = malloc(height * sizeof(rgb_pixel_t *));
    for (uint16_t y = 0; y < height; y++) {
        pixels[y] = malloc(width * sizeof(rgb_pixel_t));
        memcpy(pixels[y], &input[y * width * sizeof(rgb_pixel_t)],
               width * sizeof(rgb_pixel_t));
    }

    free(input);

    im_t *image = malloc(sizeof(im_t));
    image->width = swidth;
    image->height = sheight;
    image->pixels = pixels;

    return image;
}

void image_save(im_t *image, char *filename) {
    uint32_t width = image->width, height = image->height;

    unsigned char *raw = malloc(width * height * sizeof(rgb_pixel_t));
    for (uint32_t row = 0; row < height; row++) {
        memcpy(&raw[row * width * sizeof(rgb_pixel_t)], image->pixels[row],
               width * sizeof(rgb_pixel_t));
    }

    lodepng_encode24_file(filename, raw, width, height);
    free(raw);
}

static inline uint16_t brightness(rgb_pixel_t pix) { return pix.red + pix.green + pix.blue; }

static inline int weighted_sum(uint16_t *l, const int8_t *weights, uint16_t len) {
    int sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += l[i] * weights[i];
    }
    return sum;
}

double energy(im_t *im, int x, int y) {
    const int8_t x_weights[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    const int8_t y_weights[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    uint16_t width = im->width, height = im->height;
    uint16_t brightnesses[9];
    memset(&brightnesses, 0, 9 * sizeof(uint16_t));

    for (int row_offset = -1; row_offset <= 1; row_offset++) {
        int row = y + row_offset;
        if ((row < 0) || (row >= height))
            continue;
        rgb_pixel_t *row_pixels = im->pixels[row];
        for (int col_offset = -1; col_offset <= 1; col_offset++) {
            int col = x + col_offset;
            if ((col < 0) || (col >= width))
                continue;
            int idx = (row_offset + 1) * 3 + (col_offset + 1);
            brightnesses[idx] = brightness(row_pixels[col]);
        }
    }

    int x_energy = weighted_sum(brightnesses, x_weights, 9);
    int y_energy = weighted_sum(brightnesses, y_weights, 9);

    return sqrt(x_energy * x_energy + y_energy * y_energy);
}

double **energies_init(uint16_t width, uint16_t height) {
    double **energies = malloc(height * sizeof(double *));
    for (uint16_t row = 0; row < height; row++) {
        energies[row] = malloc(width * sizeof(double));
    }
    return energies;
}

void calculate_energies(double **energies, im_t *image) {
    for (uint16_t row = 0; row < image->height; row++) {
        for (uint16_t col = 0; col < image->width; col++) {
            energies[row][col] = energy(image, col, row);
        }
    }
}

void energies_destroy(double **energies, uint16_t original_height) {
    for (uint16_t row = 0; row < original_height; row++) {
        free(energies[row]);
    }
    free(energies);
}

seam_t **seams_init(uint16_t width, uint16_t height) {
    seam_t **seams = malloc(NUM_SEAM_ROWS(height) * sizeof(seam_t *));

    for (uint16_t row = 0; row < NUM_SEAM_ROWS(height); row++) {
        seams[row] = malloc(width * sizeof(seam_t));
        for (uint16_t col = 0; col < width; col++) {
#ifdef PERF
            seams[row][col].positions = malloc((row + 1) * sizeof(uint16_t));
#else
            seams[row][col].positions = malloc(height * sizeof(uint16_t));
#endif
        }
    }

    return seams;
}

void fill_seams(seam_t **seams, double **energies, im_t *image) {
    uint16_t width = image->width, height = image->height;

    for (uint16_t col = 0; col < width; col++) {
        seams[0][col].energy = energies[0][col];
        seams[0][col].positions[0] = col;
    }

    for (uint16_t row = 1; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            seam_t *min_seam = &seams[SEAM_PREV_ROW(row)][col];

            seam_t *right = (col + 1 < width) ? &seams[SEAM_PREV_ROW(row)][col + 1] : NULL;
            seam_t *left = (col >= 1) ? &seams[SEAM_PREV_ROW(row)][col - 1] : NULL;

            if ((right) && right->energy < min_seam->energy) min_seam = right;
            if ((left) && left->energy < min_seam->energy) min_seam = left;

            seams[SEAM_CURRENT_ROW(row)][col].energy = min_seam->energy + energies[row][col];
            memcpy(seams[SEAM_CURRENT_ROW(row)][col].positions, min_seam->positions, row * sizeof(uint16_t));
            seams[SEAM_CURRENT_ROW(row)][col].positions[row] = col;
        }

#ifndef PERF
        for (uint16_t i = 0; i < width; i++) {
            seams[SEAM_PREV_ROW(row)][i].energy = seams[SEAM_CURRENT_ROW(row)][i].energy;
            memcpy(seams[SEAM_PREV_ROW(row)][i].positions, seams[SEAM_CURRENT_ROW(row)][i].positions,
                   (row + 1) * sizeof(uint16_t));
        }
#endif
    }
}

void remove_seam(im_t *image, seam_t *seam) {
    for (uint16_t row = 0; row < image->height; row++) {
        uint16_t to_remove = seam->positions[row];
        memmove(&image->pixels[row][to_remove],
                &image->pixels[row][to_remove + 1],
                (image->width - to_remove - 1) * sizeof(rgb_pixel_t));
    }
    image->width--;
}

void seams_destroy(seam_t **seams, uint16_t original_width, uint16_t original_height) {
    for (uint16_t row = 0; row < NUM_SEAM_ROWS(original_height); row++) {
        for (uint16_t col = 0; col < original_width; col++)
            free(seams[row][col].positions);
        free(seams[row]);
    }
    free(seams);
}

void image_destroy(im_t *image, uint16_t original_height) {
    for (uint16_t row = 0; row < original_height; row++) {
        free(image->pixels[row]);
    }
    free(image->pixels);
    free(image);
}